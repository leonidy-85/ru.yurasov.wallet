// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <QtCore/QDir>
#include <QtCore/QElapsedTimer>
#include <QtCore/QStandardPaths>
#include <QtGui/QPainter>
#include <QtGui/QPdfWriter>
#include <QtConcurrent/QtConcurrent>
#include <nemonotifications-qt5/notification.h>

#include "pdfwriter.h"
#include "page.h"
#include "document.h"
#include "storage.h"
#include "service.h"

DSPdfWriter::DSPdfWriter(QObject *parent)
    : QObject(parent)
{
    connect(&m_thread, &QFutureWatcherBase::finished, this, &DSPdfWriter::initDocumentInfoAndTryToStartThread);
}


void DSPdfWriter::initDocumentInfoAndTryToStartThread() {
    m_documentInfoCur = DocumentInfo();
    _tryToStartThread();
}

DSPdfWriter::~DSPdfWriter()
{
    m_thread.waitForFinished();
}

void DSPdfWriter::save(DSDocument *document, const QString &fileName, const DSPageSize &size, qint32 dpi, bool deleteFromDraft)
{
    m_queue.append({ QPointer<DSDocument>(document), fileName, size, dpi, deleteFromDraft });

    _tryToStartThread();
}

void DSPdfWriter::_tryToStartThread()
{
    if (!m_thread.isFinished())
        return;

    if (m_queue.isEmpty())
        return;

    m_documentInfoCur = m_queue.takeFirst();

    m_thread.setFuture(QtConcurrent::run(this, &DSPdfWriter::_startThread));
}

void DSPdfWriter::_startThread()
{
    if (m_documentInfoCur == DocumentInfo())
        return;

    if (!m_documentInfoCur.document)
        return;

    if (m_documentInfoCur.fileName.isEmpty())
        return;

    if (m_documentInfoCur.document->pagesCount() == 0)
        return;

    if (m_documentInfoCur.deleteFromDraft) {
        DSStorage * const storage = dynamic_cast<DSStorage *>(m_documentInfoCur.document->parent());
        if (storage)
            storage->takeDocument(storage->indexOfDocument(m_documentInfoCur.document));
    }

    QDir fileDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = m_documentInfoCur.fileName;
    if (!fileName.endsWith(QStringLiteral(".pdf")))
        fileName.append(QStringLiteral(".pdf"));

    QPageSize pageSize(m_documentInfoCur.size.sizeMm(), QPageSize::Millimeter);
    QPageLayout pageLayout(pageSize, QPageLayout::Portrait, QMarginsF(5.0f, 5.0f, 5.0f, 5.0f), QPageLayout::Millimeter);
    pageLayout.setMode(QPageLayout::FullPageMode);
    QPdfWriter pdfWriter(fileDir.absoluteFilePath(fileName));
    pdfWriter.setTitle(fileName);
    pdfWriter.setPageLayout(pageLayout);
    pdfWriter.setResolution(m_documentInfoCur.dpi);

    QPainter painter(&pdfWriter);
    bool newPage = false;
    for (qint32 i = 0; i < m_documentInfoCur.document->pagesCount(); ++i) {
        if (newPage) {
            pdfWriter.setPageLayout(pageLayout);
            pdfWriter.setResolution(m_documentInfoCur.dpi);
            pdfWriter.newPage();
        }
        newPage = true;

        QElapsedTimer timer;
        timer.start();
        while (m_documentInfoCur.document->page(i)->alignedImagePath().isEmpty() && timer.elapsed() < 10000)
            continue;

        QImage image(m_documentInfoCur.document->page(i)->alignedImagePath());
        QRect pageRect = pageLayout.paintRectPixels(m_documentInfoCur.dpi);
        pageRect -= pageLayout.marginsPixels(m_documentInfoCur.dpi);
        QRect targetRect(QPoint(0, 0), image.size().scaled(pageRect.size(), Qt::KeepAspectRatio));
        targetRect.moveCenter(pageRect.center());

        painter.drawImage(targetRect, image, image.rect());
    }

    if (m_documentInfoCur.deleteFromDraft)
        m_documentInfoCur.document->remove();

    QVariant defaultAction = Notification::remoteAction(QStringLiteral("default"), tr("Open document"),
                                                        DSService::dbusService(),
                                                        DSService::dbusPath(),
                                                        DSService::dbusIface(),
                                                        DSService::dbusMethod(),
                                                        { fileDir.absoluteFilePath(fileName) });

    Notification notification;
    notification.setAppName(tr("Document Scanner"));
    notification.setSummary(tr("Document has been saved"));
    notification.setIcon(QStringLiteral("image://theme/icon-m-file-pdf-dark"));
    notification.setBody(fileName);
    notification.setIsTransient(false);
    notification.setItemCount(1);
    notification.setHintValue("x-nemo-feedback", "sms_exists");
    notification.setRemoteAction(defaultAction);
    notification.publish();
}
