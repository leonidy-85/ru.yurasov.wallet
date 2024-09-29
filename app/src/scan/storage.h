// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DSSTORAGE_H
#define DSSTORAGE_H

#include <QtCore/QObject>
#include <QtCore/QDir>

//#include "types.h"

class DSDocument;

class DSStorage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qint32 documentsCount READ documentsCount NOTIFY documentsCountChanged)

public:
    explicit DSStorage(QObject *parent = nullptr);
    ~DSStorage() override;

    static QDir dataDir();

    qint32 documentsCount() const;

    Q_INVOKABLE qint32 indexOfDocument(DSDocument *document);
    Q_INVOKABLE DSDocument *document(qint32 index);
    Q_INVOKABLE DSDocument *takeDocument(qint32 index);
    Q_INVOKABLE QString insertDocument(qint32 index, DSDocument *document);
    //Q_INVOKABLE QString documentToBase64(DSDocument *document);

    Q_INVOKABLE void moveDocument(qint32 indexFrom, qint32 indexTo);

signals:
    void documentsCountChanged(qint32 documentsCount);
    void documentInserted(qint32 index, DSDocument *document);
    void documentMoved(qint32 indexFrom, qint32 indexTo, DSDocument *document);
    void documentTaken(qint32 index, DSDocument *document);

private:
    const QString m_documentsPath {  };

    QList<DSDocument *> m_documents {  };
};

Q_DECLARE_METATYPE(DSStorage *)

#endif // DSSTORAGE_H
