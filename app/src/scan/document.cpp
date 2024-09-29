// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <QtCore/QDateTime>

#include "document.h"
#include "page.h"
#include "storage.h"
#include <QByteArray>
#include <QBuffer>
#include <QDebug>

DSDocument::DSDocument(QObject *parent)
    : DSDocument(QUuid::createUuid(), parent)
{
    m_name = QStringLiteral("Scan %1").arg(QDateTime::currentDateTime().toString(QStringLiteral("HH:mm dd.MM.yyyy")));
}

DSDocument::DSDocument(const QUuid &id, QObject *parent)
    : QObject(parent), m_id(id),
      m_pagesPath(DSStorage::dataDir().absoluteFilePath(QStringLiteral("document_%1_pages").arg(m_id.toString())))
{
    upload();
}

DSDocument::~DSDocument()
{
    const DSStorage * const storage = dynamic_cast<DSStorage *>(parent());
    if (!storage)
        remove();
}



QUuid DSDocument::id() const
{
    return m_id;
}

qint32 DSDocument::pagesCount() const
{
    return m_pages.count();
}

QString DSDocument::name() const
{
    return m_name;
}

void DSDocument::setName(const QString &name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(name);
}

QString DSDocument::preview() const
{
    return m_previewPath;
}

void DSDocument::remove()
{
    DSStorage *storage = dynamic_cast<DSStorage *>(parent());
    if (storage)
        storage->takeDocument(storage->indexOfDocument(this));

    QList<DSPage *> pages = m_pages;
    for (const auto &page : pages)
        page->remove();

    QFile::remove(m_pagesPath);
    deleteLater();
}

void DSDocument::unload()
{
    QStringList pageNames;
    for (const auto &page : m_pages) {
        pageNames.append(page->id().toString());
        page->setCache(false);
        delete page;
    }

    QFile file(m_pagesPath);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QDataStream stream(&file);
        stream << m_name;
        stream << pageNames;
        file.close();
    }
}
// qWarning() << previewPath;
void DSDocument::upload()
{
    QStringList pageNames;
    QFile file(m_pagesPath);
    if (file.open(QFile::ReadOnly)) {
        QDataStream stream(&file);
        stream >> m_name;
        stream >> pageNames;
        file.close();
    }

    qint32 index = 0;
    for (const auto &pageName : pageNames)
        insertPage(index++, new DSPage(QUuid(pageName), this));
}

qint32 DSDocument::indexOfPage(DSPage *page)
{
    return m_pages.indexOf(page);
}

DSPage *DSDocument::page(qint32 index)
{
    if (index < 0 || index >= m_pages.count())
        return nullptr;

    return m_pages.at(index);
}

DSPage *DSDocument::takePage(qint32 index)
{
    if (index < 0 || index >= m_pages.count())
        return nullptr;

    DSPage *page = m_pages.takeAt(index);
    page->setParent(nullptr);

    disconnect(page, &DSPage::imagePathChanged,
               this, &DSDocument::_updatePreview);
    disconnect(page, &DSPage::alignedImagePathChanged,
               this, &DSDocument::_updatePreview);

    emit pageTaken(index, page);
    emit pagesCountChanged(m_pages.count());

    _updatePreview();

    return page;
}

void DSDocument::insertPage(qint32 index, DSPage *page)
{
    page->setParent(this);

    connect(page, &DSPage::imagePathChanged,
            this, &DSDocument::_updatePreview);
    connect(page, &DSPage::alignedImagePathChanged,
            this, &DSDocument::_updatePreview);

    m_pages.insert(index, page);
    emit pageInserted(m_pages.indexOf(page), page);
    emit pagesCountChanged(m_pages.count());

    _updatePreview();
}

void DSDocument::movePage(qint32 indexFrom, qint32 indexTo)
{
    if (indexFrom < 0 || indexFrom >= m_pages.size() || indexTo < 0 || indexTo >= m_pages.size() || indexFrom == indexTo)
        return;

    DSPage *page = m_pages.at(indexFrom);
    if (!page)
        return;

    m_pages.move(indexFrom, indexTo);
    emit pageMoved(indexFrom, indexTo, page);

    _updatePreview();
}

void DSDocument::_updatePreview()
{
    QString previewPath;
    for (const auto &page : m_pages) {
        if (!page->previewImagePath().isEmpty()) {
            previewPath = page->previewImagePath();
            break;
        }
    }

    if (m_previewPath == previewPath)
        return;

    m_previewPath = previewPath;
    emit previewChanged(previewPath);
}


