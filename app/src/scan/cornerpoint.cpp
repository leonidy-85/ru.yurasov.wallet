// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <QtQuick/QSGGeometryNode>
#include <QtQuick/QSGFlatColorMaterial>
#include <QtMath>

#include "cornerpoint.h"

enum CircleType : qint32 {
    Holed,
    Filled,
};

void updateNodeGeometry(QSGGeometryNode *node, const QPointF &center, const qreal &radius,
                        const qreal &width = 0.0f, CircleType type = Filled)
{
    const qint32 vertexCount = type == Filled ? 360 : (360 * 2);
    QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vertexCount);

    if (type == Filled) {
        QSGGeometry::Point2D *vertexes = geometry->vertexDataAsPoint2D();
        for (qint32 i = 0; i < vertexCount; ++i) {
            const qreal step = i * 2.0f * M_PI / vertexCount;
            vertexes[i].set(center.x() + radius * qCos(step), center.y() - radius * qSin(step));
        }

        geometry->setDrawingMode(GL_TRIANGLE_FAN);
    } else if (type == Holed) {
        const qreal radiusOut = radius;
        const qreal radiusIn = radiusOut - width;
        const qreal step = 2.0f * M_PI / (vertexCount / 2);
        const qreal stepHalf = step / 2.0f;
        QSGGeometry::Point2D *vertexes = geometry->vertexDataAsPoint2D();

        for (qint32 i = 0; i < geometry->vertexCount(); i += 2) {
            const qreal stepOut = step * i;
            const qreal stepIn = stepOut + stepHalf;
            vertexes[i].set(center.x() + radiusOut * qCos(stepOut), center.y() - radiusOut * qSin(stepOut));
            vertexes[i + 1].set(center.x() + radiusIn * qCos(stepIn), center.y() - radiusIn * qSin(stepIn));
        }

        geometry->setDrawingMode(GL_TRIANGLE_STRIP);
    } else {
        // Nothing
    }

    node->setFlags(QSGNode::OwnsGeometry);
    node->setGeometry(geometry);
}

void updateNodeMaterial(QSGGeometryNode *node, const QColor &color)
{
    QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
    material->setColor(color);

    node->setFlags(QSGGeometryNode::OwnsMaterial);
    node->setMaterial(material);
}

DSCornerPoint::DSCornerPoint(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlags(ItemHasContents | ItemAcceptsInputMethod);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setKeepTouchGrab(true);
    setKeepMouseGrab(true);

    _updateSize();
}

QPointF DSCornerPoint::center() const
{
    return position() + QPointF(width(), height()) / 2.0f;
}

void DSCornerPoint::setCenter(const QPointF &center)
{
    if (this->center() == center)
        return;

    setPosition(center - QPointF(width(), height()) / 2.0f);
    emit centerChanged(center);
}

QColor DSCornerPoint::strokeColor() const
{
    return m_strokeColor;
}

void DSCornerPoint::setStrokeColor(const QColor &strokeColor)
{
    if (m_strokeColor == strokeColor)
        return;

    m_strokeColor = strokeColor;
    emit strokeColorChanged(strokeColor);

    update();
}

QColor DSCornerPoint::fillColor() const
{
    return m_fillColor;
}

void DSCornerPoint::setFillColor(const QColor &fillColor)
{
    if (m_fillColor == fillColor)
        return;

    m_fillColor = fillColor;
    emit fillColorChanged(fillColor);

    update();
}

qreal DSCornerPoint::pointRadius() const
{
    return m_pointRadius;
}

void DSCornerPoint::setPointRadius(const qreal &pointRadius)
{
    if (m_pointRadius == pointRadius)
        return;

    m_pointRadius = pointRadius;
    emit pointRadiusChanged(pointRadius);

    _updateSize();
    update();
}

qreal DSCornerPoint::captureRadius() const
{
    return m_captureRadius;
}

void DSCornerPoint::setCaptureRadius(const qreal &captureRadius)
{
    if (m_captureRadius == captureRadius)
        return;

    m_captureRadius = captureRadius;
    emit captureRadiusChanged(captureRadius);

    _updateSize();
    update();
}

bool DSCornerPoint::captureRadiusVisible() const
{
    return m_captureRadiusVisible;
}

void DSCornerPoint::setCaptureRadiusVisible(bool captureRadiusVisible)
{
    if (m_captureRadiusVisible == captureRadiusVisible)
        return;

    m_captureRadiusVisible = captureRadiusVisible;
    emit captureRadiusVisibleChanged(captureRadiusVisible);

    _updateSize();
    update();
}

void DSCornerPoint::_updateSize()
{
    QPointF center = this->center();

    if (m_captureRadiusVisible)
        setImplicitSize(m_captureRadius * 2.0f, m_captureRadius * 2.0f);
    else
        setImplicitSize(m_pointRadius * 2.0f, m_pointRadius * 2.0f);

    setSize(QSizeF(implicitWidth(), implicitHeight()));

    setCenter(center);
    update();
}

void DSCornerPoint::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    update();

    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

QSGNode *DSCornerPoint::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data)

    QSGNode *node = static_cast<QSGNode *>(oldNode);
    if (!node)
        node = new QSGNode();

    while (node->childCount()) {
        QSGNode *childNode = node->childAtIndex(0);
        node->removeChildNode(childNode);
        delete childNode;
    }

    const QPointF center = boundingRect().center();
    const qreal radiusIn = m_pointRadius;
    const qreal radiusOut = m_captureRadius;
    const qreal outlineWidth = 4.0f;
    const qreal strokeWidth = 1.0f;

    QSGGeometryNode *node0 = new QSGGeometryNode;
    updateNodeGeometry(node0, center, radiusIn);
    updateNodeMaterial(node0, m_fillColor);

    QSGGeometryNode *node1 = new QSGGeometryNode;
    updateNodeGeometry(node1, center, radiusIn, strokeWidth, Holed);
    updateNodeMaterial(node1, m_strokeColor);

    node->appendChildNode(node0);
    node->appendChildNode(node1);

    if (m_captureRadiusVisible) {
        QSGGeometryNode *node0 = new QSGGeometryNode;
        updateNodeGeometry(node0, center, radiusOut, outlineWidth, Holed);
        updateNodeMaterial(node0, m_fillColor);

        QSGGeometryNode *node1 = new QSGGeometryNode;
        updateNodeGeometry(node1, center, radiusOut, strokeWidth, Holed);
        updateNodeMaterial(node1, m_strokeColor);

        QSGGeometryNode *node2 = new QSGGeometryNode;
        updateNodeGeometry(node2, center, radiusOut - outlineWidth + strokeWidth, strokeWidth, Holed);
        updateNodeMaterial(node2, m_strokeColor);

        node->appendChildNode(node0);
        node->appendChildNode(node1);
        node->appendChildNode(node2);
    }

    return node;
}
