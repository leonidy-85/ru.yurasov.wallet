// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DSDOCUMENT_H
#define DSDOCUMENT_H

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QImage>
#include <QFile>
#include <QDebug>

class DSPage;

class DSDocument : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString preview READ preview NOTIFY previewChanged)
    Q_PROPERTY(qint32 pagesCount READ pagesCount NOTIFY pagesCountChanged)

public:
    explicit DSDocument(QObject *parent = nullptr);
    DSDocument(const QUuid &id, QObject *parent = nullptr);
    ~DSDocument() override;

    QUuid id() const;

    QString name() const;
    void setName(const QString &name);

    QString preview() const;

    qint32 pagesCount() const;

    Q_INVOKABLE void remove();
    Q_INVOKABLE void unload();
    Q_INVOKABLE void upload();

    Q_INVOKABLE qint32 indexOfPage(DSPage *page);
    Q_INVOKABLE DSPage *page(qint32 index);
    Q_INVOKABLE DSPage *takePage(qint32 index);
    Q_INVOKABLE void insertPage(qint32 index, DSPage *page);
    Q_INVOKABLE void movePage(qint32 indexFrom, qint32 indexTo);


    QString getPreviewImage() const {
          if (m_previewPath.isEmpty()) {
              qWarning() << "Путь к изображению не задан!";
              return NULL;
          }

          return m_previewPath;
      }


signals:
    void nameChanged(const QString &name);
    void previewChanged(const QString &preview);
    void pagesCountChanged(qint32 pagesCount);
    void pageInserted(qint32 index, DSPage *page);
    void pageMoved(qint32 indexFrom, qint32 indexTo, DSPage *page);
    void pageTaken(qint32 index, DSPage *page);

private slots:
    void _updatePreview();

private:
    const QUuid m_id {  };
    const QString m_pagesPath {  };

    QString m_name {  };
    QString m_previewPath {  };
    QList<DSPage *> m_pages {  };
};

Q_DECLARE_METATYPE(DSDocument *)

#endif // DSDOCUMENT_H
