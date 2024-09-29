// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef ZOOMVIEW_H
#define ZOOMVIEW_H

#include <QQuickItem>
#include <QImage>

class ZoomView : public QQuickItem
{
    Q_OBJECT
public:
    ZoomView(QQuickItem *parent = nullptr);

    QPointF center() const;
    void setCenter(const QPointF &center);

    qreal shift() const;
    void setShift(qreal shift);

    void setImageTexture(const QImage &imageTexture);

    const QColor &circleColor() const;
    void setCircleColor(const QColor &circleColor);

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;

private:
    void updateSize();
    void updateCenter();

private:
    qreal m_shift {10};
    const int m_pointLineWidth = 3;
    const int m_circleLineWidth = 3;
    const qreal m_pointRadius = 5.f;
    QPointF m_center;
    QImage m_imageTexture;
    QColor m_circleColor{255,255,255};
};

#endif // ZOOMVIEW_H
