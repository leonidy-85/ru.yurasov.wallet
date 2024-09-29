// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DSPDFWRITER_H
#define DSPDFWRITER_H

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QFutureWatcher>

#include "types.h"
#include "document.h"

class DSPdfWriter : public QObject
{
    Q_OBJECT

public:
    explicit DSPdfWriter(QObject *parent = nullptr);
    ~DSPdfWriter() override;

    Q_INVOKABLE void save(DSDocument *document, const QString &fileName, const DSPageSize &size, qint32 dpi, bool deleteFromDraft);

private slots:
    void initDocumentInfoAndTryToStartThread();
private:
    struct DocumentInfo {
        QPointer<DSDocument> document;
        QString fileName;
        DSPageSize size;
        qint32 dpi;
        bool deleteFromDraft;

        inline bool operator==(const DocumentInfo &rhs) const {
            return document == rhs.document
                   && fileName == rhs.fileName
                   && size == rhs.size
                   && dpi == rhs.dpi
                   && deleteFromDraft == rhs.deleteFromDraft;
        }
        inline bool operator!=(const DocumentInfo &rhs) const {
            return !(*this == rhs);
        }
    };

    void _tryToStartThread();
    void _startThread();

private:
    QFutureWatcher<void> m_thread {  };
    QList<DocumentInfo> m_queue {  };
    DocumentInfo m_documentInfoCur {  };
};

Q_DECLARE_METATYPE(DSPdfWriter *)

#endif // DSPDFWRITER_H
