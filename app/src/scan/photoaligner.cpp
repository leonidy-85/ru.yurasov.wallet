// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <QtConcurrent/QtConcurrent>

#include "photoaligner.h"
#include "imageproc.h"

DSPhotoAligner::DSPhotoAligner(QObject *parent)
    : QObject(parent)
{
    connect(&m_processingThread, &QFutureWatcherBase::started, this, &DSPhotoAligner::onProcessingThreadStartedAction);
    connect(&m_processingThread, &QFutureWatcherBase::finished, this, &DSPhotoAligner::onProcessingThreadFinishedAction);
}

void DSPhotoAligner::onProcessingThreadStartedAction()
{
    emit processingChanged(true);
}

void DSPhotoAligner::onProcessingThreadFinishedAction()
{
    emit processingChanged(false);

    m_frame.alignedImage = m_processingThread.result();
    emit frameTypeChanged(m_frame.imageType);
    emit frameChanged(m_frame);

    if (m_frameCache != DSFrame()) {
        if (m_frameCache.image != m_frame.image && m_frameCache.points != m_frame.points)
            _startThread(m_frameCache);
        m_frameCache = DSFrame();
    }
}

DSPhotoAligner::~DSPhotoAligner()
{
    m_processingThread.waitForFinished();
}

bool DSPhotoAligner::processing() const
{
    return !m_processingThread.isFinished();
}

bool DSPhotoAligner::active() const
{
    return m_active;
}

void DSPhotoAligner::setActive(bool active)
{
    if (m_active == active)
        return;

    m_active = active;
    emit activeChanged(active);

    _startThread(m_frame);
}

DSFrame::ImageType DSPhotoAligner::frameType() const
{
    return m_frame.imageType;
}

DSFrame DSPhotoAligner::frame() const
{
    return m_frame;
}

void DSPhotoAligner::setFrame(const DSFrame &frame)
{
    if (m_frame == frame)
        return;

    m_frameCache = frame;
    m_frame = frame;
    emit frameTypeChanged(frame.imageType);
    emit frameChanged(frame);

    _startThread(frame);
}

void DSPhotoAligner::_startThread(const DSFrame &frame)
{
    if (!m_active || !m_processingThread.isFinished())
        return;

    const QImage &image = frame.image;
    const QVector<QPointF> &points = frame.points;
    m_processingThread.setFuture(QtConcurrent::run(&ImageProc::alignPerspective, image, points));
}
