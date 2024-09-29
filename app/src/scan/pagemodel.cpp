// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include "pagemodel.h"
#include "page.h"
#include "document.h"

enum PMDataRole : qint32 {
    ImageRole = Qt::UserRole + 1,
    AlignedImageRole,
    PreviewImageRole,
};

DSPageModel::DSPageModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int DSPageModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)

    if (!m_document)
        return 0;

    return m_document->pagesCount();
}

QVariant DSPageModel::data(const QModelIndex &index, int role) const
{
    if (!m_document || !index.isValid() || index.row() < 0 || index.row() >= m_document->pagesCount())
        return QVariant();

    const DSPage *page = m_document->page(index.row());

    switch (role) {
    case ImageRole:
        return QVariant::fromValue(page->imagePath());
    case AlignedImageRole:
        return QVariant::fromValue(page->alignedImagePath());
    case PreviewImageRole:
        return QVariant::fromValue(page->previewImagePath());
    }

    return QVariant();
}

QHash<int, QByteArray> DSPageModel::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        { ImageRole, "image" },
        { AlignedImageRole, "alignedImage" },
        { PreviewImageRole, "previewImage" },
    };

    return roles;
}

DSDocument *DSPageModel::document() const
{
    return m_document;
}

void DSPageModel::setDocument(DSDocument *document)
{
    if (m_document == document)
        return;

    if (m_document) {
        disconnect(m_document);
        for (qint32 i = 0; i < m_document->pagesCount(); ++i)
            disconnect(m_document->page(i));
    }

    m_document = document;

    if (m_document) {
        _connectToDocument(m_document);
        for (qint32 i = 0; i < m_document->pagesCount(); ++i)
            _connectToPage(m_document->page(i));
    }

    emit documentChanged(m_document);
}

void DSPageModel::_connectToDocument(DSDocument *document)
{
    if (!document)
        return;

    connect(document, &DSDocument::pageInserted, this, &DSPageModel::onPageInsertedAction);
    connect(document, &DSDocument::pageTaken, this, &DSPageModel::onPageTakenAction);
    connect(document, &DSDocument::pageMoved, this, &DSPageModel::onPageMovedAction);
}

void DSPageModel::onPageInsertedAction(qint32 index, DSPage *page) {
    _connectToPage(page);
    beginInsertRows(QModelIndex(), index, index);
    endInsertRows();
}

void DSPageModel::onPageTakenAction(qint32 index, DSPage *page) {
    disconnect(page);
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}

void DSPageModel::onPageMovedAction(qint32 indexFrom, qint32 indexTo, DSPage *page) {
    Q_UNUSED(page)

    beginMoveRows(QModelIndex(), indexFrom, indexFrom, QModelIndex(), indexTo > indexFrom ? indexTo + 1 : indexTo);
    endMoveRows();
}

void DSPageModel::_connectToPage(DSPage *page)
{
    if (!page)
        return;

    connect(page, &DSPage::imagePathChanged, this, &DSPageModel::updateModelWithImageRole);
    connect(page, &DSPage::alignedImagePathChanged, this, &DSPageModel::updateModelWithAlignedImageRole);
    connect(page, &DSPage::previewImagePathChanged, this, &DSPageModel::updateModelWithPreviewImageRole);
}

void DSPageModel::updateModelWithImageRole() {
    _updateModel(sender(), ImageRole);
}

void DSPageModel::updateModelWithAlignedImageRole() {
    _updateModel(sender(), AlignedImageRole);
}
void DSPageModel::updateModelWithPreviewImageRole() {
    _updateModel(sender(), PreviewImageRole);
}

void DSPageModel::_updateModel(QObject *sender, qint32 dataRole)
{
    QModelIndex index = computeModelIndex(sender);
    emit dataChanged(index, index, { dataRole });
}

QModelIndex DSPageModel::computeModelIndex(QObject *sender) {
    DSPage *page = qobject_cast<DSPage *>(sender);
    if (!page)
        return QModelIndex();

    qint32 index = m_document->indexOfPage(page);
    if (index == -1)
        return QModelIndex();

    return this->index(index);
}
