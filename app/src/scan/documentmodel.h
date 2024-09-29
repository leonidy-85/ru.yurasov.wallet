// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DSDOCUMENTMODEL_H
#define DSDOCUMENTMODEL_H

#include <QtCore/QAbstractItemModel>

class DSDocument;
class DSStorage;

class DSDocumentModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(DSStorage *storage READ storage WRITE setStorage NOTIFY storageChanged)

public:
    explicit DSDocumentModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    DSStorage *storage() const;
    void setStorage(DSStorage *storage);

signals:
    void storageChanged(DSStorage *storage);

private slots:
    void updateModelWithNameRole();
    void updateModelWithPreviewRole();
    void onDocumentInsertedAction(qint32 index, DSDocument *document);
    void onDocumentTakenAction(qint32 index, DSDocument *document);
    void onDocumentMovedAction(qint32 indexFrom, qint32 indexTo, DSDocument *document);
private:
    void _connectToDocuments(DSStorage *storage);
    void _connectToDocument(DSDocument *document);
    void _updateModel(QObject *sender, qint32 dataRole);

private:
    DSStorage *m_storage { nullptr };
    QModelIndex computeModelIndex(QObject *sender);
};

Q_DECLARE_METATYPE(DSDocumentModel *)

#endif // DSDOCUMENTMODEL_H
