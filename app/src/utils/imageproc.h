// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef IMAGEPROC_H
#define IMAGEPROC_H

#include <QtCore/QVector>
#include <QtGui/QImage>
#include <QtMultimedia/QVideoFrame>

class ImageProcPrivate;
class ImageProc {
public:
    static QVector<QPointF> searchCornerPoints(const QImage &image, const QSize &maxSize);
    static QImage alignPerspective(const QImage &image, const QVector<QPointF> &points);
    static QImage imageFromFrame(const QVideoFrame &frame);

private:
    static ImageProcPrivate m_pData;
};

#endif // IMAGEPROC_H
