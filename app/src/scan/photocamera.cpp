// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <QtCore/QFile>
#include <QtGui/QScreen>
#include <QtGui/QGuiApplication>
#include <QtMultimedia/QCameraImageCapture>
#include <QtMultimedia/QMetaDataWriterControl>

#include <limits>
#include "photocamera.h"
#include "imageproc.h"

std::tuple<qreal, quint64, const QSize> makeTupleByValues(qreal ratio, quint64 area, const QSize &size);
QList<std::tuple<qreal, quint64, QSize>> makeTupleList(const QList<QSize> &sizes);
QList<QSize> makeSizeList(const QList<std::tuple<qreal, quint64, QSize>> &tuple);
void filterTupleListByArea(QList<std::tuple<qreal, quint64, QSize>> &tuple, quint64 min, quint64 max);
void filterTupleListByRatio(QList<std::tuple<qreal, quint64, QSize>> &tuple, qreal bestRatio);

enum : qint32 {
    Ratio = 0,
    Area = 1,
    Size = 2,
};

QList<QSize> filterResolutions(const QList<QSize> &resolutions, quint64 minArea, quint64 maxArea, qreal ratio)
{
    auto resolutionTuples = makeTupleList(resolutions);
    filterTupleListByArea(resolutionTuples, minArea, maxArea);
    filterTupleListByRatio(resolutionTuples, ratio);

    return makeSizeList(resolutionTuples);
};

std::tuple<qreal, quint64, const QSize> makeTupleByValues(qreal ratio, quint64 area, const QSize &size)
{
    return std::make_tuple<>(ratio, area, size);
}

QList<std::tuple<qreal, quint64, QSize>> makeTupleList(const QList<QSize> &sizes)
{
    QList<std::tuple<qreal, quint64, QSize>> tuples;
    for (const auto &size : sizes) {
        const quint64 area = size.width() * size.height();
        const qreal ratio = static_cast<qreal>(size.width()) / static_cast<qreal>(size.height());
        tuples.append(makeTupleByValues(ratio, area, size));
    }

    return tuples;
}

QList<QSize> makeSizeList(const QList<std::tuple<qreal, quint64, QSize>> &tuple)
{
    QList<QSize> sizes;
    for (const auto &item : tuple)
        sizes.append(std::get<Size>(item));

    return sizes;
}

void filterTupleListByArea(QList<std::tuple<qreal, quint64, QSize>> &tuple, quint64 min, quint64 max)
{
    for (const auto &item : tuple) {
        if (std::get<Area>(item) >= min && std::get<Area>(item) <= max)
            continue;
        tuple.removeOne(item);
    }
}

void filterTupleListByRatio(QList<std::tuple<qreal, quint64, QSize>> &tuple, qreal bestRatio)
{
    qreal delta = std::numeric_limits<qreal>::max();
    qreal ratio = 0.0f;

    for (const auto &item : tuple) {
        const qreal itemRatio = std::get<Ratio>(item);
        const qreal itemDelta = qAbs(itemRatio - bestRatio);
        if (itemDelta < delta) {
            delta = itemDelta;
            ratio = itemRatio;
        }
    }

    for (const auto &item : tuple) {
        if (std::get<Ratio>(item) == ratio)
            continue;
        tuple.removeOne(item);
    }
}

DSPhotoCamera::DSPhotoCamera(QObject *parent)
    : QObject(parent)
{

}

void DSPhotoCamera::onCameraStatusChangedAction(QCamera::Status status)
{
    if (status == QCamera::LoadedStatus && !m_resolutionsIsSet) {
        constexpr quint64 videoResMin = 960 * 720; // 640 * 480
        constexpr quint64 videoResMax = 1440 * 1080;
        constexpr quint64 photoResMin = 1440 * 1080;
        constexpr quint64 photoResMax = 3264 * 2448;
        constexpr qreal pageA4Ratio = 297.0f / 210.0f;

        const QList<QSize> streamResolutions = filterResolutions(m_camera->supportedViewfinderResolutions(),
                                                                 videoResMin, videoResMax, pageA4Ratio);
        const QList<QSize> screenResolutions = filterResolutions(m_cameraCapture->supportedResolutions(),
                                                                 photoResMin, photoResMax, pageA4Ratio);

        if (!streamResolutions.isEmpty() && !screenResolutions.isEmpty()) {
            m_streamResolution = streamResolutions.first();
            m_screenResolution = screenResolutions.last();
            m_camera->unload();
        }
    } else if (status == QCamera::UnloadedStatus && !m_resolutionsIsSet && !m_streamResolution.isEmpty()) {
        QCameraViewfinderSettings viefinderSettings = m_camera->viewfinderSettings();
        viefinderSettings.setResolution(m_streamResolution);
        m_camera->setViewfinderSettings(viefinderSettings);

        QImageEncoderSettings encodingSettings = m_cameraCapture->encodingSettings();
        encodingSettings.setResolution(m_screenResolution);
        m_cameraCapture->setEncodingSettings(encodingSettings);

        m_resolutionsIsSet = true;
        m_camera->start();
    }
}

void DSPhotoCamera::onImageAvailableAction(qint32 id, const QVideoFrame &frame)
{
    Q_UNUSED(id)
    m_frame.image = ImageProc::imageFromFrame(frame).transformed(m_transform);
    m_frame.imageType = DSFrame::Photo;
    emit frameTypeChanged(m_frame.imageType);
    emit frameChanged(m_frame);
}

void DSPhotoCamera::onImageSavedAction(qint32 id, const QString &fileName)
{
    Q_UNUSED(id)
    if (QFile::exists(fileName))
        QFile::remove(fileName);
}

void DSPhotoCamera::updateTransform(Qt::ScreenOrientation orientation)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    const qint32 screenRotation = (360 - screen->angleBetween(screen->nativeOrientation(), orientation)) % 360;
    const qint32 cameraRotation = m_cameraInfo.orientation();
    const qint32 rotation = (720 + cameraRotation + screenRotation) % 360;

    QTransform transform;
    transform.rotate(-cameraRotation);
    m_transform = transform;

    if (m_metaDataControl)
        m_metaDataControl->setMetaData(QStringLiteral("Orientation"), rotation);
}

DSPhotoCamera::~DSPhotoCamera()
{
    m_camera->stop();
}

void DSPhotoCamera::catchPhoto()
{
    m_cameraCapture->cancelCapture();
    m_cameraCapture->capture();
}

void DSPhotoCamera::clearPhoto()
{
    m_cameraCapture->cancelCapture();
    m_frame.image = QImage();
    m_frame.imageType = DSFrame::Unknown;
    emit frameTypeChanged(m_frame.imageType);
    emit frameChanged(m_frame);
}

QObject *DSPhotoCamera::qmlCamera()
{
    return m_camera;
}

DSFrame::ImageType DSPhotoCamera::frameType() const
{
    return m_frame.imageType;
}

DSFrame DSPhotoCamera::frame() const
{
    return m_frame;
}

void DSPhotoCamera::setQmlCamera(QObject *qmlCamera)
{
    m_camera = qvariant_cast<QCamera *>(qmlCamera->property("mediaObject"));
    m_camera->setCaptureMode(QCamera::CaptureStillImage);
    m_camera->focus()->setFocusMode(QCameraFocus::ContinuousFocus);
    m_camera->exposure()->setFlashMode(QCameraExposure::FlashOff);

    m_cameraInfo = QCameraInfo(*m_camera);

    m_cameraCapture = new QCameraImageCapture(m_camera, this);
    m_cameraCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer
                                           | QCameraImageCapture::CaptureToFile);

    m_metaDataControl = m_camera->service() ? m_camera->service()->requestControl<QMetaDataWriterControl *>() : nullptr;
    connect(m_camera, &QCamera::statusChanged, this, &DSPhotoCamera::onCameraStatusChangedAction);

    connect(m_cameraCapture, &QCameraImageCapture::imageAvailable, this, &DSPhotoCamera::onImageAvailableAction);
    connect(m_cameraCapture, &QCameraImageCapture::imageSaved, this, &DSPhotoCamera::onImageSavedAction);

    QScreen *screen = QGuiApplication::primaryScreen();
    screen->setOrientationUpdateMask(Qt::PortraitOrientation
                                     | Qt::LandscapeOrientation
                                     | Qt::InvertedPortraitOrientation
                                     | Qt::InvertedLandscapeOrientation);

    connect(screen, &QScreen::orientationChanged,
            this, &DSPhotoCamera::updateTransform);
    updateTransform(screen->orientation());
    m_camera->start();

}

