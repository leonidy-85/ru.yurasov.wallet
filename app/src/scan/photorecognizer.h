// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DSPHOTORECOGNIZER_H
#define DSPHOTORECOGNIZER_H

#include <QtCore/QObject>
#include <QtCore/QFutureWatcher>

#include "types.h"

class DSPhotoRecognizer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool processing READ processing NOTIFY processingChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(DSFrame::ImageType frameType READ frameType NOTIFY frameTypeChanged)
    Q_PROPERTY(DSFrame frame READ frame WRITE setFrame NOTIFY frameChanged)

public:
    explicit DSPhotoRecognizer(QObject *parent = nullptr);
    ~DSPhotoRecognizer() override;

    bool processing() const;
    bool active() const;
    void setActive(bool active);

    DSFrame::ImageType frameType() const;
    DSFrame frame() const;
    void setFrame(const DSFrame &frame);

signals:
    void processingChanged(bool processing);
    void activeChanged(bool active);
    void frameTypeChanged(const DSFrame::ImageType &frameType);
    void frameChanged(const DSFrame &frame);

private slots:
    void onProcessingThreadFinishedAction();
    void onProcessingThreadStartedAction();
private:
    void _startThread(const DSFrame &frame);

private:
    bool m_active { false };
    qreal m_shapeArea { 0.0f };
    QSize m_imageSize {  };
    DSFrame m_frame {  };
    DSFrame m_frameCache {  };

    QFutureWatcher<QVector<QPointF>> m_processingThread {  };
};

Q_DECLARE_METATYPE(DSPhotoRecognizer *)

#endif // DSPHOTORECOGNIZER_H
