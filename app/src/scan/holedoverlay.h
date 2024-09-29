// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DSHOLEDOVERLAY_H
#define DSHOLEDOVERLAY_H

#include <QtQuick/QQuickItem>

class DSHoledOverlay : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QVector<QPointF> points READ points WRITE setPoints NOTIFY pointsChanged)
    Q_PROPERTY(QRectF imageRect READ imageRect WRITE setImageRect NOTIFY imageRectChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    explicit DSHoledOverlay(QQuickItem *parent = nullptr);

    QVector<QPointF> points() const;
    void setPoints(const QVector<QPointF> &points);

    QRectF imageRect() const;
    void setImageRect(const QRectF &imageRect);

    QColor color() const;
    void setColor(const QColor &color);

signals:
    void pointsChanged(const QVector<QPointF> &points);
    void imageRectChanged(const QRectF &imageRect);
    void colorChanged(const QColor &color);

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;

private:
    QVector<QPointF> m_points {  };
    QRectF m_imageRect {  };
    QColor m_color { QColor(0, 0, 0, 100) };
};

Q_DECLARE_METATYPE(DSHoledOverlay *)

#endif // DSHOLEDOVERLAY_H
