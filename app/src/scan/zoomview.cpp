// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <QSGTexture>
#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <QSGGeometry>
#include <QSGNode>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QtMath>

#include "zoomview.h"

ZoomView::ZoomView(QQuickItem *parent)
    :QQuickItem(parent)
{
    setFlags(ItemHasContents);
    updateSize();
}

QSGNode *ZoomView::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
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

    QPointF topLeft(m_center.x() - width() / 2.f,
                    m_center.y() - height() / 2.f);
    QSizeF textureSize(1.f / (width()),
                       1.f / (height()));

    QSGGeometryNode *picture = new QSGGeometryNode();
    QSGGeometryNode *circle = new QSGGeometryNode();
    QSGGeometryNode *point = new QSGGeometryNode();

    QSGGeometry *geometryPoint = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 6);
    geometryPoint->setLineWidth(m_pointLineWidth);
    geometryPoint->setDrawingMode(GL_LINE_LOOP);
    point->setGeometry(geometryPoint);
    point->setFlag(QSGNode::OwnsGeometry);
    circle->setFlag(QSGNode::OwnsMaterial);

    QSGGeometry *geometryCircle = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 80);
    geometryCircle->setDrawingMode(GL_LINE_STRIP);
    geometryCircle->setLineWidth(m_circleLineWidth);
    circle->setGeometry(geometryCircle);
    circle->setFlag(QSGNode::OwnsGeometry);
    circle->setFlag(QSGNode::OwnsMaterial);

    QSGGeometry *geometryPicture = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 80);
    geometryPicture->setDrawingMode(GL_TRIANGLE_FAN);
    picture->setGeometry(geometryPicture);
    picture->setFlag(QSGNode::OwnsGeometry);
    picture->setFlag(QSGNode::OwnsOpaqueMaterial);

    auto materialPicure = new QSGOpaqueTextureMaterial;
    materialPicure->setTexture(window()->createTextureFromImage(m_imageTexture));
    materialPicure->setFiltering(QSGTexture::Linear);
    materialPicure->setMipmapFiltering(QSGTexture::Linear);
    picture->setMaterial(materialPicure);
    picture->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

    auto materialCircle = new QSGFlatColorMaterial;
    materialCircle->setColor(m_circleColor);
    circle->setMaterial(materialCircle);
    circle->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

    auto materialPoint = new QSGFlatColorMaterial;
    materialPoint->setColor(m_circleColor);
    point->setMaterial(materialPoint);
    point->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

    auto vertexesTexture = picture->geometry()->vertexDataAsTexturedPoint2D();
    auto vertexesPoint = point->geometry()->vertexDataAsPoint2D();
    auto vertexesCircle = circle->geometry()->vertexDataAsPoint2D();

    qreal xCircle = 0;
    qreal yCircle = 0;
    qreal xTexture = 0;
    qreal yTexture = 0;
    qreal angle = 0;

    vertexesPoint[0].set(m_center.x() + m_pointRadius, m_center.y());
    vertexesPoint[1].set(m_center.x() - m_pointRadius, m_center.y());
    vertexesPoint[2].set(m_center.x(), m_center.y());
    vertexesPoint[3].set(m_center.x(), m_center.y() + m_pointRadius);
    vertexesPoint[4].set(m_center.x(), m_center.y() - m_pointRadius);
    vertexesPoint[5].set(m_center.x(), m_center.y());

    for(int i = 0; i < circle->geometry()->vertexCount() - 1; ++i)
    {
        angle = i * 2.f * M_PI/circle->geometry()->vertexCount();
        xCircle = m_center.x() + (width() / 2) * cos(angle);
        yCircle = m_center.y() + (height() / 2) * sin(angle);
        xTexture = xCircle * textureSize.width()  -  topLeft.x() * textureSize.width();
        yTexture = yCircle * textureSize.height() - topLeft.y() * textureSize.height();
        vertexesCircle[i].set(xCircle, yCircle);
        vertexesTexture[i].set(xCircle, yCircle, xTexture, yTexture);
    }
    vertexesTexture[picture->geometry()->vertexCount() - 1].set(m_center.x() + (width() / 2) * cos(0),
                                                                m_center.y() + (height() / 2) * sin(0),
                                                                1, 0.5);
    vertexesCircle[circle->geometry()->vertexCount() - 1].set(m_center.x() + (width() / 2) * cos(0),
                                                              m_center.y() + (height() / 2) * sin(0));

    node->appendChildNode(picture);
    node->appendChildNode(point);
    node->appendChildNode(circle);

    return node;
}

void ZoomView::updateSize()
{
    setImplicitSize(width(), height());
    setSize(QSizeF(implicitWidth(), implicitHeight()));
    update();
}

void ZoomView::updateCenter()
{
    qreal moveX = width() / 2 + m_shift;
    qreal moveY = height() / 2 + m_shift;
    if(parent()){
        QQuickItem* parentItem = qobject_cast<QQuickItem*>(parent());
        if(m_center.x() > parentItem->boundingRect().center().x()){
            moveX = m_center.x() - moveX;
        }else{
            moveX += m_center.x();
        }
        if(m_center.y() > parentItem->boundingRect().center().y()){
            moveY = m_center.y() - moveY;
        }else{
            moveY += m_center.y();
        }
    }else{
        moveX = m_center.x() - moveX;
        moveY = m_center.y() - moveY;
    }
    m_center = QPointF(moveX, moveY);
    update();
}

QPointF ZoomView::center() const
{
    return m_center;
}

const QColor &ZoomView::circleColor() const
{
    return m_circleColor;
}

void ZoomView::setCircleColor(const QColor &circleColor)
{
    m_circleColor = circleColor;
}

void ZoomView::setImageTexture(const QImage &imageTexture)
{
    m_imageTexture = imageTexture;
    update();
}

qreal ZoomView::shift() const
{
    return m_shift;
}

void ZoomView::setShift(qreal shift)
{
    if(m_shift != shift)
        m_shift = shift;
}

void ZoomView::setCenter(const QPointF &center)
{
    if(m_center != center){
        m_center = center;
        updateCenter();
    }
}



