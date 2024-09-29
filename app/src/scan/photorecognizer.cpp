// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <QtConcurrent/QtConcurrent>
#include <QtMath>

#include "photorecognizer.h"
#include "imageproc.h"

const QSize c_videoFrameMaxSize = QSize(512, 512);
const QSize c_photoFrameMaxSize = QSize(2048, 2048);

qreal coordToLine(const QPointF &point0, const QPointF &point1)
{
    return qSqrt(qFabs(point0.x() - point1.x()) * qFabs(point0.x() - point1.x())
                 + qFabs(point0.y() - point1.y()) * qFabs(point0.y() - point1.y()));
}

qreal geronFunc(qreal a, qreal b, qreal c)
{
    const qreal p = (a + b + c) / 2.0;
    return qSqrt(p * (p - a) * (p - b) * (p - c));
}

qreal shapeArea(const QVector<QPointF> &points)
{
    if (points.size() != 4)
        return 0.0f;

    const qreal ab = coordToLine(points.at(0), points.at(1));
    const qreal bc = coordToLine(points.at(1), points.at(2));
    const qreal cd = coordToLine(points.at(2), points.at(3));
    const qreal da = coordToLine(points.at(3), points.at(0));
    const qreal bd = coordToLine(points.at(1), points.at(3));

    return geronFunc(ab, bd, da) + geronFunc(bc, cd, bd);
}

QVector<QPointF> defaultPoints(const QSize &size)
{
    const qreal offestH = static_cast<qreal>(size.width()) / 8.0f;
    const qreal offestV = static_cast<qreal>(size.height()) / 8.0f;
    const qreal offest = qMin(offestH, offestV);
    const QRectF rect = QRectF(QPointF(0.0f, 0.0f), size).adjusted(offest, offest, -offest, -offest);

    QVector<QPointF> points;
    points.append(rect.topLeft());
    points.append(rect.topRight());
    points.append(rect.bottomRight());
    points.append(rect.bottomLeft());

    return points;
}

DSPhotoRecognizer::DSPhotoRecognizer(QObject *parent)
    : QObject(parent)
{
    connect(&m_processingThread, &QFutureWatcherBase::started, this, &DSPhotoRecognizer::onProcessingThreadStartedAction);
    connect(&m_processingThread, &QFutureWatcherBase::finished, this, &DSPhotoRecognizer::onProcessingThreadFinishedAction);
}

void DSPhotoRecognizer::onProcessingThreadStartedAction()
{
    emit processingChanged(true);
}

void DSPhotoRecognizer::onProcessingThreadFinishedAction()
{
    emit processingChanged(false);

    QVector<QPointF> points = m_processingThread.result();

    qreal shapeAreaNew = 0.0f;
    if (m_frame.imageType == DSFrame::Photo) {
        const qreal scale = static_cast<qreal>(m_imageSize.width()) / static_cast<qreal>(m_frame.image.width());
        QVector<QPointF> checkPoints = points;
        QTransform transform = QTransform().scale(scale, scale);
        for (auto &checkPoint : checkPoints)
            checkPoint = transform.map(checkPoint);

        shapeAreaNew = shapeArea(checkPoints);
        if (qFuzzyIsNull(shapeAreaNew)
            || (!qFuzzyIsNull(shapeAreaNew) && (shapeAreaNew < m_shapeArea * 0.7 || shapeAreaNew > m_shapeArea * 1.3)))
            m_frame.points = defaultPoints(m_frame.image.size());
        else
            m_frame.points = points;
    } else {
        shapeAreaNew = shapeArea(points);
        m_frame.points = points;
    }

    m_shapeArea = shapeAreaNew;
    m_imageSize = m_frame.image.size();

    emit frameTypeChanged(m_frame.imageType);
    emit frameChanged(m_frame);

    if (m_frameCache != DSFrame()) {
        if (m_frameCache.image != m_frame.image)
            _startThread(m_frameCache);
        m_frameCache = DSFrame();
    }
}

DSPhotoRecognizer::~DSPhotoRecognizer()
{
    m_processingThread.waitForFinished();
}

bool DSPhotoRecognizer::processing() const
{
    return !m_processingThread.isFinished();
}

bool DSPhotoRecognizer::active() const
{
    return m_active;
}

void DSPhotoRecognizer::setActive(bool active)
{
    if (m_active == active)
        return;

    m_active = active;
    emit activeChanged(active);

    _startThread(m_frame);
}

DSFrame::ImageType DSPhotoRecognizer::frameType() const
{
    return m_frame.imageType;
}

DSFrame DSPhotoRecognizer::frame() const
{
    return m_frame;
}

void DSPhotoRecognizer::setFrame(const DSFrame &frame)
{
    if (m_frame == frame)
        return;

    QVector<QPointF> points = m_frame.points;

    m_frameCache = frame;
    m_frame = frame;
    m_frame.points = points;
    emit frameTypeChanged(frame.imageType);
    emit frameChanged(frame);

    _startThread(frame);
}

void DSPhotoRecognizer::_startThread(const DSFrame &frame)
{
    if (!m_active || !m_processingThread.isFinished())
        return;

    const QImage &image = frame.image;
    const QSize &maxSize = frame.imageType == DSFrame::Video ? c_videoFrameMaxSize : c_photoFrameMaxSize;
    m_processingThread.setFuture(QtConcurrent::run(&ImageProc::searchCornerPoints, image, maxSize));
}
