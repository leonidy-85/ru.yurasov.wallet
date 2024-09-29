// SPDX-FileCopyrightText: 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include "qmlimage.h"

QmlImage::QmlImage(QQuickItem *parent) : QQuickPaintedItem(parent) { }

void QmlImage::setImage(QImage image)
{
    m_image = image;
    update();
}

void QmlImage::paint(QPainter *painter)
{
    painter->drawImage(QRectF{ 0, 0, width(), height() }, m_image);
}
