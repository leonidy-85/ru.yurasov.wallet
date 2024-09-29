// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <QtCore/QUuid>
#include <QtCore/QStandardPaths>

#include <QBuffer>
#include "storage.h"
#include "document.h"
#include "../image/imageconverter.h"

#include <QDebug>


DSStorage::DSStorage(QObject *parent)
    : QObject(parent), m_documentsPath(DSStorage::dataDir().absoluteFilePath(QStringLiteral("documents")))
{
    QStringList documentNames;
    QFile file(m_documentsPath);
    if (file.open(QFile::ReadOnly)) {
        QDataStream stream(&file);
        stream >> documentNames;
        file.close();
    }

    qint32 index = 0;
    for (const auto &documentName : documentNames)
        insertDocument(index++, new DSDocument(QUuid(documentName), this));
}

DSStorage::~DSStorage()
{
    QStringList pageNames;
    for (const auto &document : m_documents) {
        pageNames.append(document->id().toString());
        document->unload();
        delete document;
    }

    QFile file(m_documentsPath);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QDataStream stream(&file);
        stream << pageNames;
        file.close();
    }
}

QDir DSStorage::dataDir()
{
    static const QDir dataDir(QStringLiteral("%1%2%3")
                              .arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
                              .arg(QDir::separator()).arg(QStringLiteral("data")));

    if (!dataDir.exists())
        dataDir.mkpath(dataDir.absolutePath());

    return dataDir;
}

qint32 DSStorage::documentsCount() const
{
    return m_documents.count();
}

qint32 DSStorage::indexOfDocument(DSDocument *document)
{
    return m_documents.indexOf(document);
}

DSDocument *DSStorage::document(qint32 index)
{
    if (index < 0 || index >= m_documents.count())
        return nullptr;

    return m_documents.at(index);
}

DSDocument *DSStorage::takeDocument(qint32 index)
{
    if (index < 0 || index >= m_documents.count())
        return nullptr;

    DSDocument *document = m_documents.takeAt(index);
    document->setParent(nullptr);

    emit documentTaken(index, document);
    emit documentsCountChanged(m_documents.count());

    return document;
}


QString DSStorage::insertDocument(qint32 index, DSDocument *document)
{
    document->setParent(this);
    m_documents.insert(index, document);
    emit documentInserted(m_documents.indexOf(document), document);
    emit documentsCountChanged(m_documents.count());
     QString patch =  document->getPreviewImage();
//      ImageConverter Obj;
//   // qWarning() << patch;
//      QString res= Obj.imageToBase64(patch);
      return patch;
}

void DSStorage::moveDocument(qint32 indexFrom, qint32 indexTo)
{
    if (indexFrom < 0 || indexFrom >= m_documents.size() || indexTo < 0 || indexTo >= m_documents.size() || indexFrom == indexTo)
        return;

    DSDocument *document = m_documents.at(indexFrom);
    if (!document)
        return;

    m_documents.move(indexFrom, indexTo);
    emit documentMoved(indexFrom, indexTo, document);
}
