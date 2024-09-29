// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DSPHOTOEDITOR_H
#define DSPHOTOEDITOR_H

#include <QtQuick/QQuickItem>

#include "types.h"

class QSGSimpleTextureNode;
class DSHoledOverlay;
class DSCornerPoint;
class ZoomView;

class DSPhotoEditor : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(DSFrame frame READ frame WRITE setFrame NOTIFY frameChanged)
    Q_PROPERTY(QColor overlayColor READ overlayColor WRITE setOverlayColor NOTIFY overlayColorChanged)
    Q_PROPERTY(QColor pointsColor READ pointsColor WRITE setPointsColor NOTIFY pointsColorChanged)
    Q_PROPERTY(QColor pointsHighlightedColor READ pointsHighlightedColor WRITE setPointsHighlightedColor NOTIFY pointsHighlightedColorChanged)
    Q_PROPERTY(bool pointsVisible READ pointsVisible WRITE setPointsVisible NOTIFY pointsVisibleChanged)
    Q_PROPERTY(bool pointsManualMoving READ pointsManualMoving WRITE setPointsManualMoving NOTIFY pointsManualMovingChanged)
    Q_PROPERTY(bool pointIsCaptured READ pointIsCaptured NOTIFY pointIsCapturedChanged)
    Q_PROPERTY(qreal pointsRadius READ pointsRadius WRITE setPointsRadius NOTIFY pointsRadiusChanged)
    Q_PROPERTY(qreal zoomCircleWidth READ zoomCircleWidth WRITE setZoomCircleWidth NOTIFY zoomCircleWidthChanged)
    Q_PROPERTY(qreal zoomCircleHeight READ zoomCircleHeight WRITE setZoomCircleHeight NOTIFY zoomCircleHeightChanged)
    Q_PROPERTY(QRectF imageFilledRect READ imageFilledRect NOTIFY imageFilledRectChanged)

public:
    enum FillMode : qint32 {
        Stretch,
        PreserveAspectFit,
        PreserveAspectCrop,
    };
    Q_ENUM(FillMode)

public:
    explicit DSPhotoEditor(QQuickItem *parent = nullptr);

    Q_INVOKABLE void rotateLeft();
    Q_INVOKABLE void rotateRight();

    FillMode fillMode() const;
    void setFillMode(FillMode fillMode);

    DSFrame frame() const;
    void setFrame(const DSFrame &frame);

    QColor overlayColor() const;
    void setOverlayColor(const QColor &overlayColor);

    QColor pointsColor() const;
    void setPointsColor(const QColor &pointsColor);

    QColor pointsHighlightedColor() const;
    void setPointsHighlightedColor(const QColor &pointsHighlightedColor);

    bool pointsVisible() const;
    void setPointsVisible(bool pointsVisible);

    bool pointsManualMoving() const;
    void setPointsManualMoving(bool pointsManualMoving);

    bool pointIsCaptured() const;
    QRectF imageFilledRect() const;

    qreal pointsRadius() const;
    void setPointsRadius(qreal pointsRadius);

    qreal zoomCircleWidth() const;
    void setZoomCircleWidth(qreal zoomCircleWidth);

    qreal zoomCircleHeight() const;
    void setZoomCircleHeight(qreal zoomCircleHeight);

signals:
    void fillModeChanged(FillMode fillMode);
    void frameChanged(const DSFrame &frame);
    void overlayColorChanged(const QColor &overlayColor);
    void pointsColorChanged(const QColor &pointsColor);
    void pointsHighlightedColorChanged(const QColor &pointsHighlightedColor);
    void pointsVisibleChanged(bool pointsVisible);
    void pointsManualMovingChanged(bool pointsManualMoving);
    void pointIsCapturedChanged(bool pointIsCaptured);
    void imageFilledRectChanged(const QRectF &imageFilledRect);
    void pointsRadiusChanged(qreal pointsRadius);
    void zoomCircleWidthChanged();
    void zoomCircleHeightChanged();

private slots:
    void _rotateFrame(const qreal &angle);
    void _updateFrame(const DSFrame &frame);
    void _updateRects();

protected:
    bool eventFilter(QObject *target, QEvent *event) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;
private:
    QImage imageToZoomTexture(const QPointF& corenerPointCenter);
    static qint32 m_maxTextureSize;
    FillMode m_fillMode { FillMode::PreserveAspectFit };
    DSFrame m_frame {  };
    QRectF m_rect {  };
    QRectF m_imageRect {  };
    QRectF m_imageRectScaled {  };
    QColor m_pointsColor { QColor(255, 255, 255) };
    QColor m_pointsHighlightedColor { QColor(200, 200, 100) };
    bool m_pointsVisible { false };
    bool m_pointsManualMoving { false };
    bool m_pointIsCaptured { false };
    qreal m_pointsRadius { 50.f };
    qreal m_zoomCircleHeight { 50.f };
    qreal m_zoomCircleWidth { 50.f };
    ZoomView *m_zoomViewCircle{ nullptr };
    DSHoledOverlay *m_holedOverlayItem { nullptr };
    QList<DSCornerPoint *> m_cornerPointItemList {  };
    void mapToScaledImage(QVector<QPointF> &points);
    void mapFromScaledImage(QVector<QPointF> &points);
    void updatePoints();
};

Q_DECLARE_METATYPE(DSPhotoEditor *)

#endif // DSPHOTOEDITOR_H
