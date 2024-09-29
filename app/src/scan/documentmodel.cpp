// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include "documentmodel.h"
#include "document.h"
#include "storage.h"

enum DMDataRole : qint32 {
    PreviewRole = Qt::UserRole + 1,
    NameRole,
};

DSDocumentModel::DSDocumentModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int DSDocumentModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)

    if (!m_storage)
        return 0;

    return m_storage->documentsCount();
}

QVariant DSDocumentModel::data(const QModelIndex &index, int role) const
{
    if (!m_storage || !index.isValid() || index.row() < 0 || index.row() >= m_storage->documentsCount())
        return QVariant();

    const DSDocument *document = m_storage->document(index.row());

    switch (role) {
    case PreviewRole:
        return QVariant::fromValue(document->preview());
    case NameRole:
        return QVariant::fromValue(document->name());
    }

    return QVariant();
}

QHash<int, QByteArray> DSDocumentModel::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        { PreviewRole, "preview" },
        { NameRole, "name" }
    };

    return roles;
}

DSStorage *DSDocumentModel::storage() const
{
    return m_storage;
}

void DSDocumentModel::setStorage(DSStorage *storage)
{
    if (m_storage == storage)
        return;

    if (m_storage) {
        disconnect(m_storage);
        for (qint32 i = 0; i < m_storage->documentsCount(); ++i)
            disconnect(m_storage->document(i));
    }

    m_storage = storage;

    if (m_storage) {
        _connectToDocuments(m_storage);
        for (qint32 i = 0; i < m_storage->documentsCount(); ++i)
            _connectToDocument(m_storage->document(i));
    }

    emit storageChanged(m_storage);
}

void DSDocumentModel::_connectToDocuments(DSStorage *storage)
{
    if (!storage)
        return;

    connect(storage, &DSStorage::documentInserted, this, &DSDocumentModel::onDocumentInsertedAction);
    connect(storage, &DSStorage::documentTaken, this, &DSDocumentModel::onDocumentTakenAction);
    connect(storage, &DSStorage::documentMoved, this, &DSDocumentModel::onDocumentMovedAction);
}

void DSDocumentModel::onDocumentInsertedAction(qint32 index, DSDocument *document) {
    _connectToDocument(document);
    beginInsertRows(QModelIndex(), index, index);
    endInsertRows();
}

void DSDocumentModel::onDocumentTakenAction(qint32 index, DSDocument *document) {
    disconnect(document);
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}

void DSDocumentModel::onDocumentMovedAction(qint32 indexFrom, qint32 indexTo, DSDocument *document) {
    Q_UNUSED(document)

    beginMoveRows(QModelIndex(), indexFrom, indexFrom, QModelIndex(), indexTo > indexFrom ? indexTo + 1 : indexTo);
    endMoveRows();
}

void DSDocumentModel::_connectToDocument(DSDocument *document)
{
    if (!document)
        return;

    connect(document, &DSDocument::previewChanged, this, &DSDocumentModel::updateModelWithPreviewRole);
    connect(document, &DSDocument::nameChanged, this, &DSDocumentModel::updateModelWithNameRole);
}

void DSDocumentModel::updateModelWithNameRole()
{
    _updateModel(sender(), NameRole);
}


void DSDocumentModel::updateModelWithPreviewRole()
{
    _updateModel(sender(), PreviewRole);
}

void DSDocumentModel::_updateModel(QObject *sender, qint32 dataRole)
{
    QModelIndex index = computeModelIndex(sender);
    auto role = { dataRole };
    emit dataChanged(index, index, role);
}

QModelIndex DSDocumentModel::computeModelIndex(QObject *sender) {
    DSDocument *document = qobject_cast<DSDocument *>(sender);
    if (!document)
        return QModelIndex();

    qint32 index = m_storage->indexOfDocument(document);
    if (index == -1)
        return QModelIndex();

    return this->index(index);
}
