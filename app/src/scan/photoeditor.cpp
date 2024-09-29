// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <QtGui/QOpenGLContext>
#include <QtGui/QOffscreenSurface>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>

#include "photoeditor.h"
#include "cornerpoint.h"
#include "holedoverlay.h"
#include "zoomview.h"

Qt::AspectRatioMode fillModeToAspectRatio(DSPhotoEditor::FillMode fillMode)
{
    switch (fillMode) {
    case DSPhotoEditor::Stretch:
        return Qt::IgnoreAspectRatio;
    case DSPhotoEditor::PreserveAspectFit:
        return Qt::KeepAspectRatio;
    case DSPhotoEditor::PreserveAspectCrop:
        return Qt::KeepAspectRatioByExpanding;
    }

    return Qt::IgnoreAspectRatio;
}

qint32 suitableDivisor(qint32 size, qint32 maxDivisor)
{
    if (size < maxDivisor)
        maxDivisor = size;

    if (maxDivisor <= 0)
        return size;

    qint32 curDivisor = maxDivisor;
    while (size % curDivisor != 0) {
        --curDivisor;
        continue;
    }

    return curDivisor;
};

qreal calcCrossProduct (const QPointF &a, const QPointF &b, const QPointF &c)
{
        qreal BAx = a.x() - b.x();
        qreal BAy = a.y() - b.y();
        qreal BCx = c.x() - b.x();
        qreal BCy = c.y() - b.y();
        return (BAx * BCy - BAy * BCx);
}

bool isConvex(const QList<QPointF> &points)
{

    bool neg = false;
    bool pos = false;
    qint32 n = points.size();

    for (qint32 i = 0; i < n; ++i) {
        qint32 ia = i;
        qint32 ib = (i + 1) % n;
        qint32 ic = (i + 2) % n;
        qreal crossProduct = calcCrossProduct(points.at(ia), points.at(ib), points.at(ic));

        if (crossProduct < 0.0f)
            neg = true;
        else if (crossProduct > 0.0f)
            pos = true;

        if (neg && pos)
            return false;
    }

    return true;
}

bool hasDistance(const QList<QPointF> &points, qreal minDistance)
{
    for (qint32 i = 0; i < points.size(); ++i) {
        for (qint32 j = 0; j < points.size(); ++j) {
            if (i == j)
                continue;

            if (QLineF(points.at(i), points.at(j)).length() < minDistance)
                return false;
        }
    }

    return true;
}

bool inImageRect(const QPointF &point, const QRectF &rect)
{
    return rect.contains(point);
}

void rotatePoints(QList<DSCornerPoint *> &cornerPoints, const QRectF &boundRect)
{
    if (cornerPoints.isEmpty())
        return;

    QList<QPointF> cornerPointCenters;
    for (DSCornerPoint *cornerPointItem : cornerPoints)
        cornerPointCenters.append(cornerPointItem->center());

    const QPointF tlAnchorPos(boundRect.topLeft());
    qreal minDistance = QLineF(tlAnchorPos, cornerPointCenters.at(0)).length();
    qint32 minDistanceIndex = 0;
    for (qint32 i = 1; i < cornerPointCenters.size(); ++i) {
        qreal curDistance = QLineF(tlAnchorPos, cornerPointCenters.at(i)).length();
        if (curDistance < minDistance) {
            minDistance = curDistance;
            minDistanceIndex = i;
        }
    }

    for (qint32 i = 0; i < minDistanceIndex; ++i)
        cornerPoints.append(cornerPoints.takeFirst());
}

qint32 DSPhotoEditor::m_maxTextureSize = -1;

DSPhotoEditor::DSPhotoEditor(QQuickItem *parent)
    : QQuickItem(parent)
{
    if (m_maxTextureSize == -1) {
        QOpenGLContext glContext;
        if (glContext.create()) {
            QOffscreenSurface surface;
            surface.setFormat(glContext.format());
            surface.create();
            glContext.makeCurrent(&surface);
            glEnable(GL_TEXTURE_2D);
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
        }
    }

    setFlags(ItemHasContents);

    m_holedOverlayItem = new DSHoledOverlay(this);
    m_holedOverlayItem->setVisible(false);
    connect(m_holedOverlayItem, &DSHoledOverlay::colorChanged,
            this, &DSPhotoEditor::overlayColorChanged);
}

void DSPhotoEditor::rotateLeft()
{
    _rotateFrame(-90.0f);
}

void DSPhotoEditor::rotateRight()
{
    _rotateFrame(90.0f);
}

DSPhotoEditor::FillMode DSPhotoEditor::fillMode() const
{
    return m_fillMode;
}

void DSPhotoEditor::setFillMode(FillMode fillMode)
{
    if (m_fillMode == fillMode)
        return;

    m_fillMode = fillMode;
    emit fillModeChanged(fillMode);

    _updateRects();
    update();
}

DSFrame DSPhotoEditor::frame() const
{
    return m_frame;
}

void DSPhotoEditor::setFrame(const DSFrame &frame)
{
    metaObject()->invokeMethod(this, "_updateFrame", Qt::QueuedConnection, Q_ARG(const DSFrame &, frame));
}

QColor DSPhotoEditor::overlayColor() const
{
    return m_holedOverlayItem->color();
}

void DSPhotoEditor::setOverlayColor(const QColor &overlayColor)
{
    m_holedOverlayItem->setColor(overlayColor);
}

QColor DSPhotoEditor::pointsColor() const
{
    return m_pointsColor;
}

void DSPhotoEditor::setPointsColor(const QColor &pointsColor)
{
    if (m_pointsColor == pointsColor)
        return;

    m_pointsColor = pointsColor;
    emit pointsColorChanged(pointsColor);

    for (const auto &cornerPointItem : m_cornerPointItemList)
        cornerPointItem->setFillColor(pointsColor);
}

QColor DSPhotoEditor::pointsHighlightedColor() const
{
    return m_pointsHighlightedColor;
}

void DSPhotoEditor::setPointsHighlightedColor(const QColor &pointsHighlightedColor)
{
    if (m_pointsHighlightedColor == pointsHighlightedColor)
        return;

    m_pointsHighlightedColor = pointsHighlightedColor;
    emit pointsHighlightedColorChanged(pointsHighlightedColor);
}

bool DSPhotoEditor::pointsVisible() const
{
    return m_pointsVisible;
}

void DSPhotoEditor::setPointsVisible(bool pointsVisible)
{
    if (m_pointsVisible == pointsVisible)
        return;

    m_pointsVisible = pointsVisible;
    emit pointsVisibleChanged(pointsVisible);

    for (const auto &cornerPointItem : m_cornerPointItemList)
        cornerPointItem->setVisible(pointsVisible);

    m_holedOverlayItem->setVisible(!m_holedOverlayItem->points().isEmpty() && m_pointsVisible);
}

bool DSPhotoEditor::pointsManualMoving() const
{
    return m_pointsManualMoving;
}

void DSPhotoEditor::setPointsManualMoving(bool pointsManualMoving)
{
    if (m_pointsManualMoving == pointsManualMoving)
        return;

    m_pointsManualMoving = pointsManualMoving;
    emit pointsManualMovingChanged(pointsManualMoving);

    for (const auto &cornerPointItem : m_cornerPointItemList)
        cornerPointItem->setCaptureRadiusVisible(pointsManualMoving);
}

bool DSPhotoEditor::pointIsCaptured() const
{
    return m_pointIsCaptured;
}

QRectF DSPhotoEditor::imageFilledRect() const
{
    return m_imageRectScaled;
}

qreal DSPhotoEditor::pointsRadius() const
{
    return m_pointsRadius;
}

void DSPhotoEditor::setPointsRadius(qreal pointsRadius)
{
    if(m_pointsRadius != pointsRadius)
        m_pointsRadius = pointsRadius;
    if(!m_cornerPointItemList.isEmpty())
    {
        for(auto& it: m_cornerPointItemList)
        {
            it->setCaptureRadius(m_pointsRadius);
        }
    }
    emit pointsRadiusChanged(pointsRadius);
}

void DSPhotoEditor::_rotateFrame(const qreal &angle)
{
    DSFrame frameRotated = m_frame;
    frameRotated.image = frameRotated.image.transformed(QTransform().rotate(angle));

    QRectF imageRect = m_frame.image.rect();
    QRectF imageRectRotated = frameRotated.image.rect();
    QPointF imageCenter = imageRect.center();
    QPointF imageCenterRotated = imageRectRotated.center();

    QTransform transform;
    transform.translate(imageCenterRotated.x(), imageCenterRotated.y());
    transform.rotate(angle);
    transform.translate(-imageCenter.x(), -imageCenter.y());

    for (auto &point : frameRotated.points)
        point = transform.map(point);

    _updateFrame(frameRotated);
}

void DSPhotoEditor::mapToScaledImage(QVector<QPointF> &points)
{
    for (auto &point : points) {
        point.rx() /= m_frame.image.rect().width() / m_imageRectScaled.width();
        point.rx() += m_imageRectScaled.left();
        point.ry() /= m_frame.image.rect().height() / m_imageRectScaled.height();
        point.ry() += m_imageRectScaled.top();
    }
}

void DSPhotoEditor::mapFromScaledImage(QVector<QPointF> &points)
{
    for (auto &point : points) {
        point.rx() -= m_imageRectScaled.left();
        point.rx() *= m_frame.image.rect().width() / m_imageRectScaled.width();
        point.ry() -= m_imageRectScaled.top();
        point.ry() *= m_frame.image.rect().height() / m_imageRectScaled.height();
    }
}
void DSPhotoEditor::updatePoints()
{
    rotatePoints(m_cornerPointItemList, m_rect);

    QVector<QPointF> cornerPointCenters;
    for (DSCornerPoint *cornerPointItem : m_cornerPointItemList)
        cornerPointCenters.append(cornerPointItem->center());

    m_holedOverlayItem->setPoints(cornerPointCenters);
    m_holedOverlayItem->setVisible(!cornerPointCenters.isEmpty() && m_pointsVisible);

    mapFromScaledImage(cornerPointCenters);
    m_frame.points = cornerPointCenters;
    emit frameChanged(m_frame);
}

void DSPhotoEditor::_updateFrame(const DSFrame &frame)
{
    if (m_frame == frame)
        return;

    m_frame = frame;
    emit frameChanged(frame);

    setImplicitSize(static_cast<qreal>(frame.image.width()), static_cast<qreal>(frame.image.height()));

    _updateRects();

    QVector<QPointF> centerPoints = m_frame.points;
    mapToScaledImage(centerPoints);

    while (m_cornerPointItemList.size() < centerPoints.size()) {
        DSCornerPoint *cornerPointItem = new DSCornerPoint(this);
        cornerPointItem->setFiltersChildMouseEvents(true);
        cornerPointItem->setCaptureRadius(m_pointsRadius);
        cornerPointItem->installEventFilter(this);
        cornerPointItem->setCaptureRadiusVisible(m_pointsManualMoving);
        cornerPointItem->setFillColor(m_pointsColor);
        cornerPointItem->setStrokeColor(m_pointsColor.darker());
        cornerPointItem->setVisible(m_pointsVisible);
        m_cornerPointItemList.append(cornerPointItem);
    }

    while (m_cornerPointItemList.size() > centerPoints.size()) {
        DSCornerPoint *cornerPointItem = m_cornerPointItemList.takeLast();
        cornerPointItem->deleteLater();
    }

    for (const auto &cornerPointItem : m_cornerPointItemList)
        cornerPointItem->disconnect();

    for (qint32 i = 0; i < centerPoints.size(); ++i)
        m_cornerPointItemList.at(i)->setCenter(centerPoints.at(i));

    updatePoints();
    update();

    for (const auto &cornerPointItem : m_cornerPointItemList)
        connect(cornerPointItem, &DSCornerPoint::centerChanged,
                this, &DSPhotoEditor::updatePoints);
}

void DSPhotoEditor::_updateRects()
{
    const QRectF boundsRect = boundingRect();
    const QSizeF imageSize = m_frame.image.size();
    const QSizeF imageSizeScaled = imageSize.scaled(boundsRect.size(), fillModeToAspectRatio(m_fillMode));
    QRectF nodeRect = boundsRect;
    QRectF imageRect = m_frame.image.rect();
    QRectF imageRectScaled(QPointF(0.0f, 0.0f), imageSizeScaled);
    imageRectScaled.moveCenter(boundingRect().center());

    if (m_fillMode == FillMode::PreserveAspectFit) {
        nodeRect.setSize(imageSizeScaled);
        nodeRect.moveCenter(boundsRect.center());
    } else if (m_fillMode == FillMode::PreserveAspectCrop) {
        const qreal coef = imageSize.width() / imageSizeScaled.width();

        imageRect.setSize(nodeRect.size() * coef);
        if (imageSizeScaled.width() > nodeRect.width())
            imageRect.moveLeft((imageSizeScaled.width() - nodeRect.width()) / 2.0f * coef);
        else
            imageRect.moveTop((imageSizeScaled.height() - nodeRect.height()) / 2.0f * coef);
    }

    if (m_rect != nodeRect || m_imageRect != imageRect || m_imageRectScaled != imageRectScaled) {
        m_rect = nodeRect;
        m_imageRect = imageRect;

        if (m_imageRectScaled != imageRectScaled) {
            m_imageRectScaled = imageRectScaled;
            emit imageFilledRectChanged(imageRectScaled);
        }

        m_holedOverlayItem->setSize(nodeRect.size());
        m_holedOverlayItem->setImageRect(imageRectScaled);
    }
}

bool DSPhotoEditor::eventFilter(QObject *target, QEvent *event)
{
    if (!m_pointsManualMoving)
        return QQuickItem::eventFilter(target, event);

    static QPointF currentPointDelta = QPointF();

    DSCornerPoint *cornerPointItem = static_cast<DSCornerPoint *>(target);
    if (!cornerPointItem || !m_cornerPointItemList.contains(cornerPointItem))
        return QQuickItem::eventFilter(target, event);

    if (event->type() != QEvent::TouchBegin
            && event->type() != QEvent::TouchUpdate
            && event->type() != QEvent::TouchCancel
            && event->type() != QEvent::TouchEnd)
        return QQuickItem::eventFilter(target, event);

    const QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
    if (!touchEvent)
        return QQuickItem::eventFilter(target, event);

    const QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
    if (touchPoints.size() != 1)
        return QQuickItem::eventFilter(target, event);

    const qint32 cornerPointIndex = m_cornerPointItemList.indexOf(cornerPointItem);
    QList<QPointF> cornerPointCenters;
    for (DSCornerPoint *cornerPointItem : m_cornerPointItemList)
        cornerPointCenters.append(cornerPointItem->center());

    const QTouchEvent::TouchPoint touchPoint = touchPoints.at(0);
    if (touchPoint.state() == Qt::TouchPointPressed) {
        m_pointIsCaptured = true;
        emit pointIsCapturedChanged(m_pointIsCaptured);
        if(!m_zoomViewCircle)
            m_zoomViewCircle = new ZoomView(this);
        m_zoomViewCircle->setWidth(m_zoomCircleWidth);
        m_zoomViewCircle->setHeight(m_zoomCircleHeight);
        m_zoomViewCircle->setCenter(cornerPointItem->center());
        currentPointDelta = cornerPointItem->center() - touchPoint.scenePos();
        cornerPointItem->setFillColor(m_pointsHighlightedColor);
        cornerPointItem->setStrokeColor(m_pointsHighlightedColor.darker());
        cornerPointItem->setCaptureRadiusVisible(false);
    } else if (touchPoint.state() == Qt::TouchPointReleased) {
        m_pointIsCaptured = false;
        emit pointIsCapturedChanged(m_pointIsCaptured);
        if(m_zoomViewCircle){
            delete m_zoomViewCircle;
            m_zoomViewCircle = nullptr;
        }
        currentPointDelta = QPointF();
        cornerPointItem->setFillColor(m_pointsColor);
        cornerPointItem->setStrokeColor(m_pointsColor.darker());
        cornerPointItem->setCaptureRadiusVisible(true);
    } else if (touchPoint.state() == Qt::TouchPointMoved) {
        qreal scaleTotal = 1.0f;
        QQuickItem *item = cornerPointItem;
        while (item) {
            scaleTotal *= item->scale();
            item = item->parentItem();
        }
        cornerPointCenters[cornerPointIndex] = (touchPoint.scenePos() + currentPointDelta) / scaleTotal;
    }

    const qreal minDinstance = cornerPointItem->pointRadius() * 2.0f;
    const qreal offset = cornerPointItem->pointRadius();
    const QSizeF imageSize = m_frame.image.size();
    const QSizeF imageSizeScaled = imageSize.scaled(boundingRect().size(), fillModeToAspectRatio(m_fillMode));
    QRectF imageRect(QPointF(0.0f, 0.0f), imageSizeScaled);
    imageRect.moveCenter(boundingRect().center());
    imageRect.adjust(offset, offset, -offset, -offset);

    if (isConvex(cornerPointCenters) && hasDistance(cornerPointCenters, minDinstance)
            && inImageRect(cornerPointCenters[cornerPointIndex], imageRect)){
        cornerPointItem->setCenter(cornerPointCenters[cornerPointIndex]);
        if(m_zoomViewCircle){
            m_zoomViewCircle->setCenter(cornerPointItem->center());
            m_zoomViewCircle->setImageTexture(imageToZoomTexture(cornerPointItem->center()));
        }
    }

    return true;
}

void DSPhotoEditor::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    _updateRects();
    update();

    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

QSGNode *DSPhotoEditor::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data)

    QSGNode *node = static_cast<QSGNode *>(oldNode);
    if (!node)
        node = new QSGNode();

    QImage image;
    if (m_frame.image.isNull()) {
        image = QImage(1, 1, QImage::Format_RGBA8888);
        image.fill(Qt::transparent);
    } else {
        image = m_frame.image;
    }

    const QRect srcRect = m_imageRect.toRect();
    const qint32 srcStepX = suitableDivisor(srcRect.width(), m_maxTextureSize);
    const qint32 srcStepY = suitableDivisor(srcRect.height(), m_maxTextureSize);

    const qint32 srcStepsXCount = srcRect.width() / srcStepX;
    const qint32 srcStepsYCount = srcRect.height() / srcStepY;

    const QRectF dstRect = m_rect;
    const qreal dstStepX = static_cast<qreal>(dstRect.width()) / srcStepsXCount;
    const qreal dstStepY = static_cast<qreal>(dstRect.height()) / srcStepsYCount;

    QRectF srcSubRect(srcRect.topLeft(), QSizeF(srcStepX, srcStepY));
    QRectF dstSubRect(dstRect.topLeft(), QSizeF(dstStepX, dstStepY));

    while (node->childCount()) {
        QSGNode *childNode = node->childAtIndex(0);
        node->removeChildNode(childNode);
        delete childNode;
    }

    for (qint32 i = 0; i < srcStepsXCount; ++i) {
        for (qint32 j = 0; j < srcStepsYCount; ++j) {
            srcSubRect.moveTopLeft(srcRect.topLeft() + QPointF(i * srcStepX, j * srcStepY));
            dstSubRect.moveTopLeft(dstRect.topLeft() + QPointF(i * dstStepX, j * dstStepY));
            const QImage subImage = image.copy(srcSubRect.toRect());
            QSGSimpleTextureNode *textureNode = new QSGSimpleTextureNode();
            textureNode->setOwnsTexture(true);
            textureNode->setTexture(window()->createTextureFromImage(subImage));
            textureNode->setSourceRect(subImage.rect());
            textureNode->setRect(dstSubRect);
            node->appendChildNode(textureNode);
        }
    }

    return node;
}

QImage DSPhotoEditor::imageToZoomTexture(const QPointF &corenerPointCenter)
{

    QPointF leftTop = corenerPointCenter - QPointF(m_zoomCircleWidth / 2, m_zoomCircleHeight / 2);
    float xNew = ((leftTop.x() - m_imageRectScaled.x()) / m_imageRectScaled.width()) * m_imageRect.width() + m_imageRect.x();
    float yNew = ((leftTop.y() - m_imageRectScaled.y()) / m_imageRectScaled.height()) * m_imageRect.height() + m_imageRect.y();
    float widthNew = (m_zoomCircleWidth * m_imageRect.width()) / m_imageRectScaled.width() ;
    float heightNew = (m_zoomCircleHeight * m_imageRect.height()) / m_imageRectScaled.height();
    QImage copyImage = m_frame.image.copy(xNew, yNew, widthNew, heightNew);
    return copyImage;
}

qreal DSPhotoEditor::zoomCircleWidth() const
{
    return m_zoomCircleWidth;
}

void DSPhotoEditor::setZoomCircleWidth(qreal zoomCircleWidth)
{
    if(zoomCircleWidth != m_zoomCircleWidth)
        m_zoomCircleWidth = zoomCircleWidth;
}

qreal DSPhotoEditor::zoomCircleHeight() const
{
    return m_zoomCircleHeight;
}

void DSPhotoEditor::setZoomCircleHeight(qreal zoomCircleHeight)
{
    if(zoomCircleHeight != m_zoomCircleHeight)
        m_zoomCircleHeight = zoomCircleHeight;
}
