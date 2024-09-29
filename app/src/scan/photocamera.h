// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DSPHOTOCAMERA_H
#define DSPHOTOCAMERA_H

#include <QtCore/QSize>
#include <QtGui/QTransform>
#include <QtMultimedia/QCameraInfo>
#include <QtMultimedia/QAbstractVideoSurface>

#include "types.h"

class QCameraImageCapture;
class QMetaDataWriterControl;

class DSPhotoCamera : public QObject
{
    Q_OBJECT

    Q_PROPERTY(DSFrame::ImageType frameType READ frameType NOTIFY frameTypeChanged)
    Q_PROPERTY(DSFrame frame READ frame NOTIFY frameChanged)
    Q_PROPERTY(QObject *qmlCamera READ qmlCamera WRITE setQmlCamera NOTIFY qmlCameraChanged)

public:
    explicit DSPhotoCamera(QObject *parent = nullptr);
    ~DSPhotoCamera() override;

    Q_INVOKABLE void catchPhoto();
    Q_INVOKABLE void clearPhoto();

    QObject *qmlCamera();
    DSFrame::ImageType frameType() const;
    DSFrame frame() const;
    void setQmlCamera(QObject* videoOutpute);

signals:
    void frameTypeChanged(const DSFrame::ImageType &frameType);
    void frameChanged(const DSFrame &frame);
    void qmlCameraChanged();

private slots:
    void updateTransform(Qt::ScreenOrientation orientation);
    void onCameraStatusChangedAction(QCamera::Status status);
    void onImageAvailableAction(qint32 id, const QVideoFrame &frame);
    void onImageSavedAction(qint32 id, const QString &fileName);
private:
    QCamera *m_camera { nullptr };
    QCameraImageCapture *m_cameraCapture { nullptr };
    QMetaDataWriterControl *m_metaDataControl { nullptr };
    QCameraInfo m_cameraInfo {  };
    QTransform m_transform {  };
    QSize m_streamResolution {  };
    QSize m_screenResolution {  };
    bool m_resolutionsIsSet { false };
    DSFrame m_frame {  };
};

Q_DECLARE_METATYPE(DSPhotoCamera *)

#endif // DSPHOTOCAMERA_H
