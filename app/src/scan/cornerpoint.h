// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DSCORNERPOINT_H
#define DSCORNERPOINT_H

#include <QtQuick/QQuickItem>

class DSCornerPoint : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor NOTIFY fillColorChanged)
    Q_PROPERTY(QColor strokeColor READ strokeColor WRITE setStrokeColor NOTIFY strokeColorChanged)
    Q_PROPERTY(qreal pointRadius READ pointRadius WRITE setPointRadius NOTIFY pointRadiusChanged)
    Q_PROPERTY(qreal captureRadius READ captureRadius WRITE setCaptureRadius NOTIFY captureRadiusChanged)

public:
    explicit DSCornerPoint(QQuickItem *parent = nullptr);

    QPointF center() const;
    void setCenter(const QPointF &center);

    QColor strokeColor() const;
    void setStrokeColor(const QColor &strokeColor);

    QColor fillColor() const;
    void setFillColor(const QColor &fillColor);

    qreal pointRadius() const;
    void setPointRadius(const qreal &pointRadius);

    qreal captureRadius() const;
    void setCaptureRadius(const qreal &captureRadius);

    bool captureRadiusVisible() const;
    void setCaptureRadiusVisible(bool captureRadiusVisible);

signals:
    void centerChanged(const QPointF &center);
    void strokeColorChanged(const QColor &strokeColor);
    void fillColorChanged(const QColor &fillColor);
    void pointRadiusChanged(const qreal &pointRadius);
    void captureRadiusChanged(const qreal &captureRadius);
    void captureRadiusVisibleChanged(bool captureRadiusVisible);

private slots:
    void _updateSize();

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;

private:
    QColor m_strokeColor { QColor(50, 50, 50) };
    QColor m_fillColor { QColor(255, 255, 255) };
    qreal m_pointRadius { 8.0f };
    qreal m_captureRadius { 0.0f };
    bool m_captureRadiusVisible { true };
};

Q_DECLARE_METATYPE(DSCornerPoint *)

#endif // DSCORNERPOINT_H
