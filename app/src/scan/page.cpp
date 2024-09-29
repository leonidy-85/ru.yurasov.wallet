// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <QtConcurrent/QtConcurrent>

#include "page.h"
#include "document.h"
#include "storage.h"

DSPage::DSPage(QObject *parent)
    : DSPage(QUuid::createUuid(), parent)
{
}

DSPage::DSPage(const QUuid &id, QObject *parent)
    : QObject(parent), m_id(id),
      //m_imagePath(DSStorage::dataDir().absoluteFilePath(QStringLiteral("page_%1_image.png").arg(m_id.toString()))),
     //m_alignedImagePath(DSStorage::dataDir().absoluteFilePath(QStringLiteral("page_%1_alignedImage.png").arg(m_id.toString())))
      m_alignedImagePath(DSStorage::dataDir().absoluteFilePath(QStringLiteral("page_alignedImage.png")))
   //   m_pointsPath(DSStorage::dataDir().absoluteFilePath(QStringLiteral("page_%1_points").arg(m_id.toString())))
{
    updatePaths(false, false);

    connect(&m_saveThread, &QFutureWatcherBase::started, this, &DSPage::onSaveThreadStartedAction);
    connect(&m_saveThread, &QFutureWatcherBase::finished, this, &DSPage::onSaveThreadFinishedAction);
    connect(&m_loadThread, &QFutureWatcherBase::started, this, &DSPage::onloadThreadStartedAction);
    connect(&m_loadThread, &QFutureWatcherBase::finished, this, &DSPage::onloadThreadFinishedAction);
}

void DSPage::onSaveThreadStartedAction() {
    updatePaths(true, true);
    emit savingActiveChanged(true);
}

void DSPage::onSaveThreadFinishedAction() {
    m_frameSave = DSFrame();
    updatePaths(true, false);

    emit savingActiveChanged(false);

    _tryProcess();
}

void DSPage::onloadThreadStartedAction() {
    emit loadingActiveChanged(true);
}

void DSPage::onloadThreadFinishedAction() {
    m_loadRequest = false;
    if (m_cache && !m_cacheUpdated) {
        m_cacheUpdated = true;
        DSFrame frame = m_loadThread.result();
        if (m_frameCache != frame) {
            m_frameCache = frame;
            emit frameChanged(frame);
        }
    }

    emit loadingActiveChanged(false);

    _tryProcess();
}

void DSPage::updatePaths(bool castSignals, bool clear) {
    if (clear) {
       // m_imagePathCur.clear();
        m_alignedImagePathCur.clear();
    //    m_previewImagePathCur.clear();
    } else {
//        if (QFile::exists(m_imagePath))
//            m_imagePathCur = m_imagePath;
        if (QFile::exists(m_alignedImagePath))
            m_alignedImagePathCur = m_alignedImagePath;

        if (!m_alignedImagePathCur.isEmpty())
            m_previewImagePathCur = m_alignedImagePathCur;
        else if (!m_alignedImagePath.isEmpty())
            m_previewImagePathCur = m_alignedImagePath;
    }

    if (castSignals) {
        //emit imagePathChanged(m_imagePathCur);
        emit alignedImagePathChanged(m_alignedImagePathCur);
        emit previewImagePathChanged(m_previewImagePathCur);
    }
}

DSPage::~DSPage()
{
    const DSDocument * const document = dynamic_cast<DSDocument *>(parent());
    if (!document)
        remove();

    m_loadThread.waitForFinished();
    m_saveThread.waitForFinished();
}

QUuid DSPage::id() const
{
    return m_id;
}

QString DSPage::imagePath() const
{
    return m_imagePathCur;
}

QString DSPage::alignedImagePath() const
{
    return m_alignedImagePathCur;
}

QString DSPage::previewImagePath() const
{
    return m_previewImagePathCur;
}

DSFrame DSPage::frame() const
{
    return m_frameCache;
}

void DSPage::setFrame(const DSFrame &frame)
{
    if (m_removeRequest)
        return;

    if (m_cache) {
        if (m_frameCache != frame) {
            if (m_frameCache != DSFrame())
                m_frameSave = frame;

            m_frameCache = frame;
            m_cacheUpdated = true;
            emit frameChanged(frame);
        }
    } else {
        m_frameSave = frame;

        if (m_frameCache != DSFrame()) {
            m_frameCache = DSFrame();
            emit frameChanged(DSFrame());
        }
    }

    _tryProcess();
}

bool DSPage::cache() const
{
    return m_cache;
}

void DSPage::setCache(bool cache)
{
    if (m_removeRequest)
        return;

    if (m_cache == cache)
        return;

    m_cache = cache;
    emit cacheChanged(cache);

    if (cache) {
        m_cacheUpdated = false;
        m_loadRequest = true;
    } else {
        if (m_frameCache != DSFrame()) {
            m_frameCache = DSFrame();
            emit frameChanged(DSFrame());
        }
    }

    _tryProcess();
}

bool DSPage::savingActive() const
{
    return !m_saveThread.isFinished();
}

bool DSPage::loadingActive() const
{
    return !m_loadThread.isFinished();
}

void DSPage::remove()
{
    if (m_removeRequest)
        return;

    DSDocument *document = dynamic_cast<DSDocument *>(parent());
    if (document)
        document->takePage(document->indexOfPage(this));

    m_removeRequest = true;
    _tryProcess();
}

void DSPage::_tryProcess()
{
    if (m_removeRequest) {
        QFile::remove(m_imagePath);
        QFile::remove(m_alignedImagePath);
        QFile::remove(m_pointsPath);
        deleteLater();
        return;
    }

    if (!m_saveThread.isFinished() || !m_loadThread.isFinished())
        return;

    if (m_loadRequest) {
        m_loadThread.setFuture(QtConcurrent::run<DSFrame>(this, &DSPage::_processLoad));
        return;
    }

    if (m_frameSave != DSFrame())
        m_saveThread.setFuture(QtConcurrent::run<void>(this, &DSPage::_processSave, m_frameSave));
}

void DSPage::_processSave(const DSFrame &frame) const
{
   // frame.image.save(m_imagePath);
    frame.alignedImage.save(m_alignedImagePath);
    QFile file(m_pointsPath);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QDataStream stream(&file);
        stream << frame.points;
        file.close();
    }
}

DSFrame DSPage::_processLoad() const
{
    DSFrame frame;
    frame.image.load(m_imagePath);
    frame.alignedImage.load(m_alignedImagePath);
    QFile file(m_pointsPath);
    if (file.open(QFile::ReadOnly)) {
        QDataStream stream(&file);
        stream >> frame.points;
        file.close();
    }

    return frame;
}
