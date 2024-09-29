// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef POLYGONPROC_H
#define POLYGONPROC_H

#include <QtCore/QVector>
#include <QtGui/QPolygonF>
#include <QtGui/QPainterPath>

class PolygonProcPrivate;
class PolygonProc
{
public:
    static QVector<QPointF> triangulate(const QPainterPath &path);
    static QVector<QPointF> triangulate(const QVector<QPolygonF> &polygons);

private:
    static PolygonProcPrivate m_pData;
};

#endif // POLYGONPROC_H
