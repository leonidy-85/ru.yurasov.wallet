// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DSPAGEMODEL_H
#define DSPAGEMODEL_H

#include <QtCore/QAbstractItemModel>

class DSPage;
class DSDocument;

class DSPageModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(DSDocument *document READ document WRITE setDocument NOTIFY documentChanged)

public:
    explicit DSPageModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    DSDocument *document() const;
    void setDocument(DSDocument *document);

signals:
    void documentChanged(DSDocument *document);

private slots:
    void onPageInsertedAction(qint32 index, DSPage *page);
    void onPageTakenAction(qint32 index, DSPage *page);
    void onPageMovedAction(qint32 indexFrom, qint32 indexTo, DSPage *page);
    void updateModelWithImageRole();
    void updateModelWithAlignedImageRole();
    void updateModelWithPreviewImageRole();
private:
    void _connectToDocument(DSDocument *document);
    void _connectToPage(DSPage *page);
    void _updateModel(QObject *sender, qint32 dataRole);

private:
    DSDocument *m_document { nullptr };
    QModelIndex computeModelIndex(QObject *sender);
};

Q_DECLARE_METATYPE(DSPageModel *)

#endif // DSPAGEMODEL_H
