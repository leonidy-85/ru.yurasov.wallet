// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <QtQuick/QSGGeometryNode>
#include <QtQuick/QSGFlatColorMaterial>

#include "holedoverlay.h"
#include "polygonproc.h"

DSHoledOverlay::DSHoledOverlay(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents);
}

QVector<QPointF> DSHoledOverlay::points() const
{
    return m_points;
}

void DSHoledOverlay::setPoints(const QVector<QPointF> &points)
{
    if (m_points == points)
        return;

    m_points = points;
    emit pointsChanged(points);

    update();
}

QRectF DSHoledOverlay::imageRect() const
{
    return m_imageRect;
}

void DSHoledOverlay::setImageRect(const QRectF &imageRect)
{
    if (m_imageRect == imageRect)
        return;

    m_imageRect = imageRect;
    emit imageRectChanged(imageRect);

    update();
}

QColor DSHoledOverlay::color() const
{
    return m_color;
}

void DSHoledOverlay::setColor(const QColor &color)
{
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);

    update();
}

void DSHoledOverlay::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (m_imageRect.isNull() && newGeometry.width() > 0.0f && newGeometry.height() > 0.0f) {
        m_imageRect = newGeometry;
        emit imageRectChanged(newGeometry);
    }

    update();

    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

QSGNode *DSHoledOverlay::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data)

    QSGGeometryNode *node = static_cast<QSGGeometryNode *>(oldNode);
    if (!node) {
        node = new QSGGeometryNode();
        node->setFlags(QSGNode::OwnsGeometry | QSGGeometryNode::OwnsMaterial);
    }

    QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
    material->setColor(m_color);

    QPainterPath pathOut;
    pathOut.addRect(m_imageRect);
    QVector<QPointF> points;

    if (m_points.size() > 3) {
        QPainterPath pathIn;
        pathIn.moveTo(m_points.at(0));
        for (qint32 i = 1; i < m_points.size(); ++i)
            pathIn.lineTo(m_points.at(i));
        pathIn.closeSubpath();

        points = PolygonProc::triangulate(pathOut.subtracted(pathIn));
    } else {
        points = PolygonProc::triangulate(pathOut);
    }

    QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), points.length());
    geometry->setDrawingMode(GL_TRIANGLES);
    for (qint32 i = 0; i < points.length(); ++i) {
        const QPointF &vertex = points.at(i);
        geometry->vertexDataAsPoint2D()[i].set(vertex.x(), vertex.y());
    }

    node->setMaterial(material);
    node->setGeometry(geometry);

    return node;
}
