// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DSPAGE_H
#define DSPAGE_H

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QFutureWatcher>

#include "types.h"

class DSPage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString imagePath READ imagePath NOTIFY imagePathChanged)
    Q_PROPERTY(QString alignedImagePath READ alignedImagePath NOTIFY alignedImagePathChanged)
    Q_PROPERTY(QString previewImagePath READ previewImagePath NOTIFY previewImagePathChanged)
    Q_PROPERTY(DSFrame frame READ frame WRITE setFrame NOTIFY frameChanged)
    Q_PROPERTY(bool cache READ cache WRITE setCache NOTIFY cacheChanged)
    Q_PROPERTY(bool savingActive READ savingActive NOTIFY savingActiveChanged)
    Q_PROPERTY(bool loadingActive READ loadingActive NOTIFY loadingActiveChanged)

public:
    explicit DSPage(QObject *parent = nullptr);
    DSPage(const QUuid &id, QObject *parent = nullptr);
    ~DSPage() override;

    QUuid id() const;

    QString imagePath() const;
    QString alignedImagePath() const;
    QString previewImagePath() const;

    DSFrame frame() const;
    void setFrame(const DSFrame &frame);

    bool cache() const;
    void setCache(bool cache);

    bool savingActive() const;
    bool loadingActive() const;

    Q_INVOKABLE void remove();

signals:
    void imagePathChanged(const QString &imagePath);
    void alignedImagePathChanged(const QString &alignedImagePath);
    void previewImagePathChanged(const QString &previewImagePath);
    void frameChanged(const DSFrame &frame);
    void cacheChanged(bool cache);
    void savingActiveChanged(bool savingActive);
    void loadingActiveChanged(bool loadingActive);

private slots:
    void _tryProcess();
    void _processSave(const DSFrame &frame) const;
    DSFrame _processLoad() const;

    void onSaveThreadStartedAction();
    void onSaveThreadFinishedAction();
    void onloadThreadFinishedAction();
    void onloadThreadStartedAction();
private:
    const QUuid m_id {  };
    const QString m_imagePath {  };
    const QString m_alignedImagePath {  };
    const QString m_pointsPath {  };

    QString m_imagePathCur {  };
    QString m_alignedImagePathCur {  };
    QString m_previewImagePathCur {  };

    bool m_removeRequest { false };
    bool m_loadRequest { false };
    bool m_cache { false };
    bool m_cacheUpdated { false };
    DSFrame m_frameCache {  };
    DSFrame m_frameSave {  };
    QFutureWatcher<void> m_saveThread {  };
    QFutureWatcher<DSFrame> m_loadThread {  };
    void updatePaths(bool castSignals, bool clear);
};

Q_DECLARE_METATYPE(DSPage *)

#endif // DSPAGE_H
