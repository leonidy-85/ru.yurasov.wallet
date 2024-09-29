// SPDX-FileCopyrightText: 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <QCameraImageCapture>
#include <QPointer>
#include <QImage>
#include <QCamera>

inline const int AURORA_SDK_MAJOR_VERSION {QString(AURORA_SDK_VERSION).split(".").first().toInt()};

class CameraHelper : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject *qcamera READ qcamera WRITE setQcamera NOTIFY qcameraChanged)

public:
    inline QObject *qcamera() const { return m_qcamera; }

    void setQcamera(QObject *qcamera);

    Q_INVOKABLE void capture();

signals:
    void qcameraChanged();
    void imageChanged(QImage newImage);

private:
    QPointer<QObject> m_qcamera;
    QCameraImageCapture *m_capture{ nullptr };
};
