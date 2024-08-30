// SPDX-FileCopyrightText: 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef QMLIMAGE_H
#define QMLIMAGE_H

#include <QPainter>
#include <QQuickPaintedItem>

class QmlImage : public QQuickPaintedItem
{
    Q_OBJECT
public:
    QmlImage(QQuickItem *parent = nullptr);

    Q_INVOKABLE void setImage(QImage image);

protected:
    virtual void paint(QPainter *painter) override;

private:
    QImage m_image;
};

#endif // QMLIMAGE_H
