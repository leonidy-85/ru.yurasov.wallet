// SPDX-FileCopyrightText: 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include "camerahelper.h"

#include <QCamera>
#include <QCameraImageCapture>
#include <QFile>
#include <QMediaObject>
#include <QPointer>

void CameraHelper::setQcamera(QObject *qcamera)
{
    if (m_qcamera == qcamera) {
        return;
    }

    if (m_capture) {
        m_capture->deleteLater();
    }

    m_qcamera = qcamera;
    if (m_qcamera) {
        // This property below holds the native media object for the camera
        if (QCamera *camera = qvariant_cast<QCamera *>(m_qcamera->property("mediaObject"))) {
            m_capture = new QCameraImageCapture(camera, this);

            if (!m_capture->isCaptureDestinationSupported(QCameraImageCapture::CaptureToBuffer)) {
                qDebug() << "The image capture to a buffer is not supported!";
                return;
            }

            if (AURORA_SDK_MAJOR_VERSION < 5) {
                m_capture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer
                                                 | QCameraImageCapture::CaptureToFile);
            } else {
                m_capture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
            }

            connect(m_capture, &QCameraImageCapture::imageAvailable, this,
                    [this](int id, const QVideoFrame &preview) {
                        QVideoFrame frame(preview); // make a copy because we can call map (non-const)
                        // Map the contents of a video frame to memory
                        frame.map(QAbstractVideoBuffer::ReadOnly);
                        int nbytes = frame.mappedBytes();
                        QImage currentImage = QImage::fromData(frame.bits(), nbytes);
                        frame.unmap();

                        qDebug() << "Buffered QImage info: " << id << " " << currentImage;
                        emit imageChanged(currentImage);
                    });

            if (AURORA_SDK_MAJOR_VERSION < 5) {
                // Qt bug:
                // https://bugreports.qt.io/browse/QTBUG-82572?jql=text%20~%20%22setCaptureDestination%22
                // If you set the destination to QCameraImageCapture::CaptureToBuffer this
                // method saves the image to the buffer and also to the file system. The
                // workaround is to add destination QCameraImageCapture::CaptureToFile to
                // receive signal to remove file.
                connect(m_capture, &QCameraImageCapture::imageSaved, this,
                        [](qint32 id, const QString &fileName) {
                    Q_UNUSED(id)

                    if (QFile::exists(fileName))
                        QFile::remove(fileName);
                });

            }
        } else {
            m_capture = nullptr;
        }
    }

    emit qcameraChanged();
}

void CameraHelper::capture()
{
    if (m_capture) {
        m_capture->capture();
    }
}
