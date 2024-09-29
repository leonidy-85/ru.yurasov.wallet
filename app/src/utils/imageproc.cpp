// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <QtCore/QDebug>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/imgproc/imgproc.hpp"

#include "imageproc.h"

class ImageProcPrivate
{
public:
    enum MatColorOrder {
        MCO_BGR,
        MCO_RGB,
        MCO_BGRA = MCO_BGR,
        MCO_RGBA = MCO_RGB,
        MCO_ARGB,
    };

public:
    QVector<QPointF> searchCornerPoints(const QImage &image, const QSize &maxSize);
    QImage alignPerspective(const QImage &image, const QVector<QPointF> &points);
    QImage imageFromFrame(const QVideoFrame &frame);

private:
    MatColorOrder getColorOrderOfRGB32Format();
    cv::Mat argb2bgra(const cv::Mat &mat);
    cv::Mat adjustChannelsOrder(const cv::Mat &srcMat, MatColorOrder srcOrder, MatColorOrder targetOrder);
    QImage::Format findClosestFormat(QImage::Format formatHint);
    cv::Mat image2MatInPlace(const QImage &image, MatColorOrder *order = nullptr);
    cv::Mat image2Mat(const QImage &image, qint32 requiredMatType = CV_8UC(0), MatColorOrder requriedOrder = MCO_BGR);
    QImage mat2ImageInPlace(const cv::Mat &mat, QImage::Format formatHint = QImage::Format_Invalid);
    QImage mat2Image(const cv::Mat &mat, MatColorOrder order = MCO_BGR, QImage::Format formatHint = QImage::Format_Invalid);

    QPointF getIntersection(const QVector<QPointF> &points);
    QPointF getMassCenter(const QVector<QPointF> &points);
    QSizeF getTargetSize(const QVector<QPointF> &points);
    cv::Mat getTransform(const QVector<QPointF> &points, const QRectF &targetRect);

    quint32 yuvToArgb(qint32 y, qint32 rv, qint32 guv, qint32 bu, qint32 a = 255);
    void planarYuv420ToArgb(const uchar *y, const uchar *u, const uchar *v,
                            qint32 yStride, qint32 uStride, qint32 vStride,
                            qint32 uvPixelStride, quint32 *rgb, qint32 width, qint32 height);
    QImage convertYv12ToArgb(const QVideoFrame &frame);
    QImage convertNv12ToArgb(const QVideoFrame &frame);
    QImage convertNv21ToArgb(const QVideoFrame &frame);
    qreal computeAngle(cv::Point pt1, cv::Point pt2, cv::Point pt0);
};

qreal ImageProcPrivate::computeAngle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
    qreal dx1 = pt1.x - pt0.x;
    qreal dy1 = pt1.y - pt0.y;
    qreal dx2 = pt2.x - pt0.x;
    qreal dy2 = pt2.y - pt0.y;
    return (dx1 * dx2 + dy1 * dy2) / std::sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10f);
}

QVector<QPointF> ImageProcPrivate::searchCornerPoints(const QImage &image, const QSize &maxSize)
{
    if (image.isNull())
        return QVector<QPointF>();

    QImage img = image;
    qreal scale = 1.0f;
    if (maxSize.width() > 0 && maxSize.height() > 0) {
        img = img.scaled(maxSize, Qt::KeepAspectRatio);
        scale = static_cast<qreal>(image.width()) / static_cast<qreal>(img.width());
    }

    cv::Mat mat = image2Mat(img);
    qint32 blurRadius = static_cast<qint32>(20.0f / scale);
    if (blurRadius % 2 == 0)
        blurRadius += 1;

    cv::Mat blurred(mat);
    cv::Mat gray0(blurred.size(), CV_8U);
    cv::Mat gray;
    cv::medianBlur(mat, blurred, blurRadius);
    std::vector<std::vector<cv::Point>> contours;
    std::vector<std::vector<cv::Point>> squares;

    for (qint32 c = 0; c < 3; ++c) {
        qint32 ch[] = { c, 0 };
        cv::mixChannels(&blurred, 1, &gray0, 1, ch, 1);

        const qint32 thresholdLevel = 2;
        for (qint32 l = 0; l < thresholdLevel; ++l) {
            if (l == 0) {
                cv::Canny(gray0, gray, 10, 20, 3);
                cv::dilate(gray, gray, cv::Mat(), cv::Point(-1, -1));
            } else {
                gray = gray0 >= (l + 1) * 255 / thresholdLevel;
            }

            cv::findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

            std::vector<cv::Point> approx;
            for (size_t i = 0; i < contours.size(); ++i) {
                cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.02f, true);

                if (approx.size() == 4 && std::fabs(cv::contourArea(cv::Mat(approx))) > 1000 && cv::isContourConvex(cv::Mat(approx))) {
                    qreal maxCosine = 0.0f;

                    for (size_t j = 2; j < 5; ++j) {
                        qreal cosine = std::fabs(computeAngle(approx[j % 4], approx[j - 2], approx[j - 1]));
                        maxCosine = std::max(maxCosine, cosine);
                    }

                    if (maxCosine < 0.3)
                        squares.push_back(approx);
                }
            }
        }
    }

    QVector<QPointF> points;
    if (!squares.empty()) {
        QVector<QPointF> unorderedPoints;
        for (const auto &point : squares.at(0))
            unorderedPoints.append(QPointF(static_cast<qreal>(point.x), static_cast<qreal>(point.y)));

        const QPointF tlAnchorPos(0.0f, 0.0f);
        qreal minDistance = QLineF(tlAnchorPos, unorderedPoints[0]).length();
        qint32 minDistanceIndex = 0;
        for (qint32 i = 1; i < unorderedPoints.size(); ++i) {
            const qreal curDistance = QLineF(tlAnchorPos, unorderedPoints[i]).length();
            if (curDistance < minDistance) {
                minDistance = curDistance;
                minDistanceIndex = i;
            }
        }

        for (qint32 i = 0; i < unorderedPoints.size(); ++i)
            points.append(unorderedPoints[(minDistanceIndex + i) % unorderedPoints.size()]);
    }

    if (!qFuzzyCompare(scale, 1.0f)) {
        for (auto &point : points)
            point *= scale;
    }

    return points;
}

QImage ImageProcPrivate::alignPerspective(const QImage &image, const QVector<QPointF> &points)
{
    if (points.size() != 4)
        return QImage();

    QVector<QPointF> pointsCopy = points;
    QList<qreal> xs;
    QList<qreal> ys;
    for (const auto &point : pointsCopy) {
        xs << point.x();
        ys << point.y();
    }
    std::sort(xs.begin(), xs.end());
    std::sort(ys.begin(), ys.end());

    const qreal offsetX = xs[0];
    const qreal offsetY = ys[0];

    for (QPointF &point : pointsCopy)
        point -= QPointF(offsetX, offsetY);
    for (qreal &x : xs)
        x -= offsetX;
    for (qreal &y : ys)
        y -= offsetY;

    const QRectF boundingRect(QPointF(xs[0], ys[0]), QPointF(xs[3], ys[3]));
    QRectF sourceRect(boundingRect);
    sourceRect.moveTopLeft(QPointF(offsetX, offsetY));
    QRectF targetRect(QPointF(0.0f, 0.0f), getTargetSize(pointsCopy).scaled(boundingRect.size(), Qt::KeepAspectRatio));
    targetRect.moveCenter(boundingRect.center());
    QImage result = image.copy(sourceRect.toRect());
    const cv::Mat transform = getTransform(pointsCopy, targetRect);
    const cv::Mat input = image2Mat(result);
    cv::Mat output;
    cv::warpPerspective(input, output, transform, { result.width(), result.height() });

    return mat2Image(output).copy(targetRect.toRect());
}

QImage ImageProcPrivate::imageFromFrame(const QVideoFrame &frame)
{
    QImage image;
    QVideoFrame frameCopy = frame;

    if (!frameCopy.map(QAbstractVideoBuffer::ReadOnly)) {
        return image;
    }

    QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(frameCopy.pixelFormat());

    if (imageFormat != QImage::Format_Invalid) {
        image = QImage(frameCopy.bits(), frameCopy.width(), frameCopy.height(), frameCopy.bytesPerLine(), imageFormat).copy();
    } else {
        switch (frameCopy.pixelFormat()) {
        case QVideoFrame::Format_Jpeg:
            image.loadFromData(frameCopy.bits(), frameCopy.mappedBytes(), "JPG");
            break;
        case QVideoFrame::Format_YV12:
            image = convertYv12ToArgb(frameCopy);
            break;
        case QVideoFrame::Format_NV12:
            image = convertNv12ToArgb(frameCopy);
            break;
        case QVideoFrame::Format_NV21:
            image = convertNv21ToArgb(frameCopy);
            break;
        default:
            qWarning() << qPrintable(QStringLiteral("Unsupported pixel format %1").arg(frameCopy.pixelFormat()));
        }
    }

    frameCopy.unmap();

    return image;
}

ImageProcPrivate::MatColorOrder ImageProcPrivate::getColorOrderOfRGB32Format()
{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    return MCO_BGRA;
#else
    return MCO_ARGB;
#endif
}

cv::Mat ImageProcPrivate::argb2bgra(const cv::Mat &mat)
{
    Q_ASSERT(mat.channels() == 4);

    const qint32 ARGB2BGRA[] = { 0, 3, 1, 2, 2, 1, 3, 0 };
    cv::Mat newMat(mat.rows, mat.cols, mat.type());
    cv::mixChannels(&mat, 1, &newMat, 1, ARGB2BGRA, 4);

    return newMat;
}

cv::Mat ImageProcPrivate::adjustChannelsOrder(const cv::Mat &srcMat, MatColorOrder srcOrder, MatColorOrder targetOrder)
{
    Q_ASSERT(srcMat.channels() == 4);

    if (srcOrder == targetOrder)
        return srcMat.clone();

    cv::Mat destMat;
    if ((srcOrder == MCO_ARGB && targetOrder == MCO_BGRA) || (srcOrder == MCO_BGRA && targetOrder == MCO_ARGB)) {
        destMat = argb2bgra(srcMat);
    } else if (srcOrder == MCO_ARGB && targetOrder == MCO_RGBA) {
        const qint32 ARGB2RGBA[] = { 0, 3, 1, 0, 2, 1, 3, 2 };
        destMat = cv::Mat(srcMat.rows, srcMat.cols, srcMat.type());
        cv::mixChannels(&srcMat, 1, &destMat, 1, ARGB2RGBA, 4);
    } else if (srcOrder == MCO_RGBA && targetOrder == MCO_ARGB) {
        const qint32 RGBA2ARGB[] = { 0, 1, 1, 2, 2, 3, 3, 0 };
        destMat = cv::Mat(srcMat.rows, srcMat.cols, srcMat.type());
        cv::mixChannels(&srcMat, 1, &destMat, 1, RGBA2ARGB, 4);
    } else {
        cv::cvtColor(srcMat, destMat, CV_BGRA2RGBA);
    }

    return destMat;
}

QImage::Format ImageProcPrivate::findClosestFormat(QImage::Format formatHint)
{
    switch (formatHint) {
    case QImage::Format_Indexed8:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    case QImage::Format_RGB888:
    case QImage::Format_RGBX8888:
    case QImage::Format_RGBA8888:
    case QImage::Format_RGBA8888_Premultiplied:
    case QImage::Format_Alpha8:
    case QImage::Format_Grayscale8:
        return formatHint;
    case QImage::Format_Mono:
    case QImage::Format_MonoLSB:
        return QImage::Format_Indexed8;
    case QImage::Format_RGB16:
        return QImage::Format_RGB32;
    case QImage::Format_RGB444:
    case QImage::Format_RGB555:
    case QImage::Format_RGB666:
        return QImage::Format_RGB888;
    case QImage::Format_ARGB4444_Premultiplied:
    case QImage::Format_ARGB6666_Premultiplied:
    case QImage::Format_ARGB8555_Premultiplied:
    case QImage::Format_ARGB8565_Premultiplied:
        return QImage::Format_ARGB32_Premultiplied;
    default:
        return QImage::Format_ARGB32;
    }
}

cv::Mat ImageProcPrivate::image2MatInPlace(const QImage &image, MatColorOrder *order)
{
    if (image.isNull())
        return cv::Mat();

    switch (image.format()) {
    case QImage::Format_Indexed8:
        break;
    case QImage::Format_RGB888:
        if (order)
            *order = MCO_RGB;
        break;
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
        if (order)
            *order = getColorOrderOfRGB32Format();
        break;
    case QImage::Format_RGBX8888:
    case QImage::Format_RGBA8888:
    case QImage::Format_RGBA8888_Premultiplied:
        if (order)
            *order = MCO_RGBA;
        break;
    case QImage::Format_Alpha8:
    case QImage::Format_Grayscale8:
        break;
    default:
        return cv::Mat();
    }

    return cv::Mat(image.height(), image.width(), CV_8UC(image.depth() / 8),
                   const_cast<uchar *>(image.bits()), static_cast<size_t>(image.bytesPerLine()));
}

cv::Mat ImageProcPrivate::image2Mat(const QImage &image, qint32 requiredMatType, MatColorOrder requriedOrder)
{
    if (image.isNull())
        return cv::Mat();

    qint32 targetDepth = CV_MAT_DEPTH(requiredMatType);
    qint32 targetChannels = CV_MAT_CN(requiredMatType);

    Q_ASSERT(targetChannels == CV_CN_MAX || targetChannels == 1 || targetChannels == 3 || targetChannels == 4);
    Q_ASSERT(targetDepth == CV_8U || targetDepth == CV_16U || targetDepth == CV_32F);

    const qreal maxAlpha = targetDepth == CV_8U ? 255.0f : (targetDepth == CV_16U ? 65535.0f : 1.0f);

    QImage::Format format = findClosestFormat(image.format());
    QImage img = format == image.format() ? image : image.convertToFormat(format);

    MatColorOrder srcOrder;
    cv::Mat mat0 = image2MatInPlace(img, &srcOrder);
    cv::Mat matAdjustCn;

    if (mat0.empty())
        return mat0;

    if (targetChannels == CV_CN_MAX)
        targetChannels = mat0.channels();

    switch(targetChannels) {
    case 1:
        if (mat0.channels() == 3) {
            cv::cvtColor(mat0, matAdjustCn, CV_RGB2GRAY);
        } else if (mat0.channels() == 4) {
            if (srcOrder == MCO_BGRA)
                cv::cvtColor(mat0, matAdjustCn, CV_BGRA2GRAY);
            else if (srcOrder == MCO_RGBA)
                cv::cvtColor(mat0, matAdjustCn, CV_RGBA2GRAY);
            else
                cv::cvtColor(argb2bgra(mat0), matAdjustCn, CV_BGRA2GRAY);
        }
        break;
    case 3:
        if (mat0.channels() == 1) {
            cv::cvtColor(mat0, matAdjustCn, requriedOrder == MCO_BGR ? CV_GRAY2BGR : CV_GRAY2RGB);
        } else if (mat0.channels() == 3) {
            if (requriedOrder != srcOrder)
                cv::cvtColor(mat0, matAdjustCn, CV_RGB2BGR);
        } else if (mat0.channels() == 4) {
            if (srcOrder == MCO_ARGB) {
                const qint32 ARGB2RGB[] = { 1, 0, 2, 1, 3, 2 };
                const qint32 ARGB2BGR[] = { 1, 2, 2, 1, 3, 0 };
                matAdjustCn = cv::Mat(mat0.rows, mat0.cols, CV_MAKE_TYPE(mat0.type(), 3));
                cv::mixChannels(&mat0, 1, &matAdjustCn, 1, requriedOrder == MCO_BGR ? ARGB2BGR : ARGB2RGB, 3);
            } else if (srcOrder == MCO_BGRA) {
                cv::cvtColor(mat0, matAdjustCn, requriedOrder == MCO_BGR ? CV_BGRA2BGR : CV_BGRA2RGB);
            } else {
                cv::cvtColor(mat0, matAdjustCn, requriedOrder == MCO_BGR ? CV_RGBA2BGR : CV_RGBA2RGB);
            }
        }
        break;
    case 4:
        if (mat0.channels() == 1) {
            if (requriedOrder == MCO_ARGB) {
                const cv::Mat alphaMat(mat0.rows, mat0.cols, CV_MAKE_TYPE(mat0.type(), 1), cv::Scalar(maxAlpha));
                const cv::Mat in[] = { alphaMat, mat0 };
                const qint32 fromTo[] = { 0, 0, 1, 1, 1, 2, 1, 3 };
                matAdjustCn = cv::Mat(mat0.rows, mat0.cols, CV_MAKE_TYPE(mat0.type(), 4));
                cv::mixChannels(in, 2, &matAdjustCn, 1, fromTo, 4);
            } else if (requriedOrder == MCO_RGBA) {
                cv::cvtColor(mat0, matAdjustCn, CV_GRAY2RGBA);
            } else {
                cv::cvtColor(mat0, matAdjustCn, CV_GRAY2BGRA);
            }
        } else if (mat0.channels() == 3) {
            if (requriedOrder == MCO_ARGB) {
                const cv::Mat alphaMat(mat0.rows, mat0.cols, CV_MAKE_TYPE(mat0.type(), 1), cv::Scalar(maxAlpha));
                const cv::Mat in[] = { alphaMat, mat0 };
                const qint32 fromTo[] = { 0, 0, 1, 1, 2, 2, 3, 3 };
                matAdjustCn = cv::Mat(mat0.rows, mat0.cols, CV_MAKE_TYPE(mat0.type(), 4));
                cv::mixChannels(in, 2, &matAdjustCn, 1, fromTo, 4);
            } else if (requriedOrder == MCO_RGBA) {
                cv::cvtColor(mat0, matAdjustCn, CV_RGB2RGBA);
            } else {
                cv::cvtColor(mat0, matAdjustCn, CV_RGB2BGRA);
            }
        } else if (mat0.channels() == 4) {
            if (srcOrder != requriedOrder)
                matAdjustCn = adjustChannelsOrder(mat0, srcOrder, requriedOrder);
        }
        break;
    default:
        break;
    }

    if (targetDepth == CV_8U)
        return matAdjustCn.empty() ? mat0.clone() : matAdjustCn;

    if (matAdjustCn.empty())
        matAdjustCn = mat0;

    cv::Mat matAdjustDepth;
    matAdjustCn.convertTo(matAdjustDepth, CV_MAKE_TYPE(targetDepth, matAdjustCn.channels()),
                          targetDepth == CV_16U ? 255.0f : 1.0f / 255.0f);

    return matAdjustDepth;
}

QImage ImageProcPrivate::mat2ImageInPlace(const cv::Mat &mat, QImage::Format formatHint)
{
    if (mat.empty())
        return QImage();

    Q_ASSERT(mat.type() == CV_8UC1 || mat.type() == CV_8UC3 || mat.type() == CV_8UC4);

    if (mat.type() == CV_8UC1) {
        if (formatHint != QImage::Format_Indexed8
                && formatHint != QImage::Format_Alpha8
                && formatHint != QImage::Format_Grayscale8) {
            formatHint = QImage::Format_Indexed8;
        }
    } else if (mat.type() == CV_8UC3) {
        formatHint = QImage::Format_RGB888;
    } else if (mat.type() == CV_8UC4) {
        if (formatHint != QImage::Format_RGB32
                && formatHint != QImage::Format_ARGB32
                && formatHint != QImage::Format_ARGB32_Premultiplied
                && formatHint != QImage::Format_RGBX8888
                && formatHint != QImage::Format_RGBA8888
                && formatHint != QImage::Format_RGBA8888_Premultiplied) {
            formatHint = QImage::Format_ARGB32;
        }
    }

    QImage image(mat.data, mat.cols, mat.rows, static_cast<qint32>(mat.step), formatHint);
    if (formatHint == QImage::Format_Indexed8) {
        QVector<QRgb> colorTable;
        for (qint32 i = 0; i < 256; ++i)
            colorTable.append(qRgb(i, i, i));
        image.setColorTable(colorTable);
    }

    return image;
}

QImage ImageProcPrivate::mat2Image(const cv::Mat &mat, MatColorOrder order, QImage::Format formatHint)
{
    if (mat.empty())
        return QImage();

    Q_ASSERT(mat.channels() == 1 || mat.channels() == 3 || mat.channels() == 4);
    Q_ASSERT(mat.depth() == CV_8U || mat.depth() == CV_16U || mat.depth() == CV_32F);

    QImage::Format format;
    cv::Mat matAdjustCn;
    if (mat.channels() == 1) {
        format = formatHint;
        if (formatHint != QImage::Format_Indexed8
                && formatHint != QImage::Format_Alpha8
                && formatHint != QImage::Format_Grayscale8)
            format = QImage::Format_Indexed8;
    } else if (mat.channels() == 3) {
        format = QImage::Format_RGB888;
        if (order == MCO_BGR)
            cv::cvtColor(mat, matAdjustCn, CV_BGR2RGB);
    } else {
        format = findClosestFormat(formatHint);
        if (format != QImage::Format_RGB32
                && format != QImage::Format_ARGB32
                && format != QImage::Format_ARGB32_Premultiplied
                && format != QImage::Format_RGBX8888
                && format != QImage::Format_RGBA8888
                && format != QImage::Format_RGBA8888_Premultiplied)
            format = order == MCO_RGBA ? QImage::Format_RGBA8888 : QImage::Format_ARGB32;

        MatColorOrder requiredOrder = getColorOrderOfRGB32Format();
        if (formatHint == QImage::Format_RGBX8888
                || formatHint == QImage::Format_RGBA8888
                || formatHint == QImage::Format_RGBA8888_Premultiplied)
            requiredOrder = MCO_RGBA;

        if (order != requiredOrder)
            matAdjustCn = adjustChannelsOrder(mat, order, requiredOrder);
    }

    if (matAdjustCn.empty())
        matAdjustCn = mat;

    cv::Mat matAdjustDepth = matAdjustCn;
    if (mat.depth() != CV_8U)
        matAdjustCn.convertTo(matAdjustDepth, CV_8UC(matAdjustCn.channels()),
                              mat.depth() == CV_16U ? 1.0f / 255.0f : 255.0f);

    QImage image = mat2ImageInPlace(matAdjustDepth, format);
    if (format == formatHint || formatHint == QImage::Format_Invalid)
        return image.copy();

    return image.convertToFormat(formatHint);
}

QPointF ImageProcPrivate::getIntersection(const QVector<QPointF> &points)
{
    QPointF intersection;
    QLineF lineTL2BR(points[0], points[2]);
    QLineF lineTR2BL(points[1], points[3]);
    if (lineTL2BR.intersect(lineTR2BL, &intersection) != QLineF::BoundedIntersection)
        return QPointF();

    return intersection;
};

QPointF ImageProcPrivate::getMassCenter(const QVector<QPointF> &points)
{
    QPointF massCenter;
    for (const QPointF &point : points)
        massCenter += point;
    massCenter /= static_cast<qreal>(points.size());

    return massCenter;
};

QSizeF ImageProcPrivate::getTargetSize(const QVector<QPointF> &points)
{
    const QLineF lineT(points[0], points[1]);
    const QLineF lineR(points[1], points[2]);
    const QLineF lineB(points[2], points[3]);
    const QLineF lineL(points[3], points[0]);
    const QPointF intersection = getIntersection(points);
    const QPointF massCenter = getMassCenter(points);

    QSizeF targetSize;
    targetSize.setWidth((lineT.length() + lineB.length()) / 2.0f + (massCenter.x() - intersection.x()) * 2.0f);
    targetSize.setHeight((lineL.length() + lineR.length()) / 2.0f + (massCenter.y() - intersection.y()) * 2.0f);
    const qreal targetMin = qMin(targetSize.width(), targetSize.height());
    const qreal targetMax = qMax(targetSize.width(), targetSize.height());
    if (targetMin / targetMax > 0.9f) {
        targetSize.setWidth(targetMax);
        targetSize.setHeight(targetMax);
    }

    return targetSize;
};

cv::Mat ImageProcPrivate::getTransform(const QVector<QPointF> &points, const QRectF &targetRect)
{
    cv::Point2f inputQuad[4];
    inputQuad[0] = cv::Point2f(points[0].x(), points[0].y());
    inputQuad[1] = cv::Point2f(points[1].x(), points[1].y());
    inputQuad[2] = cv::Point2f(points[2].x(), points[2].y());
    inputQuad[3] = cv::Point2f(points[3].x(), points[3].y());

    cv::Point2f outputQuad[4];
    outputQuad[0] = cv::Point2f(targetRect.left(), targetRect.top());
    outputQuad[1] = cv::Point2f(targetRect.right(), targetRect.top());
    outputQuad[2] = cv::Point2f(targetRect.right(), targetRect.bottom());
    outputQuad[3] = cv::Point2f(targetRect.left(), targetRect.bottom());

    return getPerspectiveTransform(inputQuad, outputQuad);
};

quint32 ImageProcPrivate::yuvToArgb(qint32 y, qint32 rv, qint32 guv, qint32 bu, qint32 a)
{
    qint32 yy = (y - 16) * 298;

    return (a << 24)
            | qBound(0, (yy + rv) >> 8, 255) << 16
                                                | qBound(0, (yy - guv) >> 8, 255) << 8
                                                | qBound(0, (yy + bu) >> 8, 255);
}

void ImageProcPrivate::planarYuv420ToArgb(const uchar *y, const uchar *u, const uchar *v,
                                          qint32 yStride, qint32 uStride, qint32 vStride,
                                          qint32 uvPixelStride, quint32 *rgb, qint32 width, qint32 height)
{
    quint32 *rgb0 = rgb;
    quint32 *rgb1 = rgb + width;

    for (qint32 j = 0; j < height; j += 2) {
        const uchar *lineY0 = y;
        const uchar *lineY1 = y + yStride;
        const uchar *lineU = u;
        const uchar *lineV = v;

        for (qint32 i = 0; i < width; i += 2) {
            const qint32 uu = *lineU - 128;
            const qint32 vv = *lineV - 128;
            const qint32 rv = 409 * vv + 128;
            const qint32 guv = 100 * uu + 208 * vv + 128;
            const qint32 bu = 516 * uu + 128;

            lineU += uvPixelStride;
            lineV += uvPixelStride;
            *rgb0++ = yuvToArgb(*lineY0++, rv, guv, bu);
            *rgb0++ = yuvToArgb(*lineY0++, rv, guv, bu);
            *rgb1++ = yuvToArgb(*lineY1++, rv, guv, bu);
            *rgb1++ = yuvToArgb(*lineY1++, rv, guv, bu);
        }

        y += yStride << 1;
        u += uStride;
        v += vStride;
        rgb0 += width;
        rgb1 += width;
    }
}

QImage ImageProcPrivate::convertYv12ToArgb(const QVideoFrame &frame)
{
    const uchar *plane1 = frame.bits(0);
    const uchar *plane2 = frame.bits(1);
    const uchar *plane3 = frame.bits(2);
    const qint32 plane1Stride = frame.bytesPerLine(0);
    const qint32 plane2Stride = frame.bytesPerLine(1);
    const qint32 plane3Stride = frame.bytesPerLine(2);
    const qint32 uvPixelStride = 1;

    QImage image(frame.width(), frame.height(), QImage::Format_ARGB32);

    planarYuv420ToArgb(plane1, plane3, plane2, plane1Stride, plane3Stride, plane2Stride,
                       uvPixelStride, reinterpret_cast<quint32 *>(image.bits()), frame.width(), frame.height());

    return image;
}

QImage ImageProcPrivate::convertNv12ToArgb(const QVideoFrame &frame)
{
    const uchar *plane1 = frame.bits(0);
    const uchar *plane2 = frame.bits(1);
    const qint32 plane1Stride = frame.bytesPerLine(0);
    const qint32 plane2Stride = frame.bytesPerLine(1);
    const qint32 uvPixelStride = 2;

    QImage image(frame.width(), frame.height(), QImage::Format_ARGB32);

    planarYuv420ToArgb(plane1, plane2, plane2 + 1, plane1Stride, plane2Stride, plane2Stride,
                       uvPixelStride, reinterpret_cast<quint32 *>(image.bits()), frame.width(), frame.height());

    return image;
}

QImage ImageProcPrivate::convertNv21ToArgb(const QVideoFrame &frame)
{
    const uchar *plane1 = frame.bits(0);
    const uchar *plane2 = frame.bits(1);
    const qint32 plane1Stride = frame.bytesPerLine(0);
    const qint32 plane2Stride = frame.bytesPerLine(1);
    const qint32 uvPixelStride = 2;

    QImage image(frame.width(), frame.height(), QImage::Format_ARGB32);

    planarYuv420ToArgb(plane1, plane2 + 1, plane2, plane1Stride, plane2Stride, plane2Stride,
                       uvPixelStride, reinterpret_cast<quint32 *>(image.bits()), frame.width(), frame.height());

    return image;
}

ImageProcPrivate ImageProc::m_pData = ImageProcPrivate();

QVector<QPointF> ImageProc::searchCornerPoints(const QImage &image, const QSize &maxSize)
{
    return m_pData.searchCornerPoints(image, maxSize);
}

QImage ImageProc::alignPerspective(const QImage &image, const QVector<QPointF> &points)
{
    return m_pData.alignPerspective(image, points);
}

QImage ImageProc::imageFromFrame(const QVideoFrame &frame)
{
    return m_pData.imageFromFrame(frame);
}
