// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include "polygonproc.h"
#include <limits>

struct Node
{
    Node(quint32 index, qreal x_, qreal y_) : i(index), x(x_), y(y_) {  }
    Node(const Node &node) = delete;
    Node &operator=(const Node &node) = delete;
    Node(Node &&node) = delete;
    Node &operator=(Node &&node) = delete;

    const quint32 i { 0 };
    const qreal x { 0.0f };
    const qreal y { 0.0f };
    qint32 z { 0 };
    bool steiner { false };

    Node *prev { nullptr };
    Node *next { nullptr };
    Node *prevZ { nullptr };
    Node *nextZ { nullptr };
};

class PolygonProcPrivate
{
public:
    QVector<QPointF> triangulate(const QPainterPath &path);
    QVector<QPointF> triangulate(const QVector<QPolygonF> &polygons);

private:
    Node *linkedList(const QPolygonF& polygon, const bool clockwise);
    Node *filterPoints(Node *start, Node *end = nullptr);
    void earcutLinked(Node *ear, qint32 pass = 0);
    bool isEar(Node *ear);
    bool isEarHashed(Node *ear);
    Node *cureLocalIntersections(Node *start);
    void splitEarcut(Node *start);
    Node *eliminateHoles(const QVector<QPolygonF> &polygon, Node *outerNode);
    Node *eliminateHole(Node *hole, Node *outerNode);
    Node *findHoleBridge(Node *hole, Node *outerNode);
    bool sectorContainsSector(const Node *m, const Node *p);
    void indexCurve(Node *start);
    Node *sortLinked(Node *list);
    qint32 zOrder(const qreal fx, const qreal fy);
    Node *getLeftmost(Node *start);
    bool pointInTriangle(qreal ax, qreal ay, qreal bx, qreal by, qreal cx, qreal cy, qreal px, qreal py) const;
    bool isValidDiagonal(Node *a, Node *b);
    qreal area(const Node *p, const Node *q, const Node *r) const;
    bool equals(const Node *p1, const Node *p2);
    bool intersects(const Node *p1, const Node *q1, const Node *p2, const Node *q2);
    bool onSegment(const Node *p, const Node *q, const Node *r);
    qint32 sign(qreal val);
    bool intersectsPolygon(const Node *a, const Node *b);
    bool locallyInside(const Node *a, const Node *b);
    bool middleInside(const Node *a, const Node *b);
    Node *splitPolygon(Node *a, Node *b);
    Node *insertNode(qint32 i, const QPointF &pt, Node *last);
    void removeNode(Node *p);

private:
    QList<Node *> m_nodes {  };
    QVector<quint32> m_indices {  };
    qint32 m_vertices { 0 };

    qreal m_minX { 0.0f };
    qreal m_maxX { 0.0f };
    qreal m_minY { 0.0f };
    qreal m_maxY { 0.0f };
    qreal m_invSize { 0.0f };
    bool m_hashing { false };
};

QVector<QPointF> PolygonProcPrivate::triangulate(const QPainterPath &path)
{
    QVector<QPolygonF> polygons = path.toSubpathPolygons().toVector();
    std::reverse(polygons.begin(), polygons.end());

    return triangulate(polygons);
}

QVector<QPointF> PolygonProcPrivate::triangulate(const QVector<QPolygonF> &polygons)
{
    qDeleteAll(m_nodes);
    m_nodes.clear();
    m_indices.clear();
    m_vertices = 0;

    if (polygons.empty())
        return QVector<QPointF>();

    qreal x = 0.0f;
    qreal y = 0.0f;
    qint32 threshold = 80;
    qint32 len = 0;

    for (qint32 i = 0; threshold >= 0 && i < polygons.size(); i++) {
        threshold -= polygons[i].size();
        len += polygons[i].size();
    }

    m_indices.reserve(len + polygons[0].size());

    Node *outerNode = linkedList(polygons.at(0), true);
    if (!outerNode || outerNode->prev == outerNode->next)
        return QVector<QPointF>();

    if (polygons.size() > 1)
        outerNode = eliminateHoles(polygons, outerNode);

    m_hashing = threshold < 0;
    if (m_hashing) {
        Node *p = outerNode->next;
        m_minX = m_maxX = outerNode->x;
        m_minY = m_maxY = outerNode->y;

        do {
            x = p->x;
            y = p->y;
            m_minX = qMin(m_minX, x);
            m_minY = qMin(m_minY, y);
            m_maxX = qMax(m_maxX, x);
            m_maxY = qMax(m_maxY, y);
            p = p->next;
        } while (p != outerNode);

        m_invSize = qMax(m_maxX - m_minX, m_maxY - m_minY);
        m_invSize = !qFuzzyIsNull(m_invSize) ? (1.0f / m_invSize) : 0.0f;
    }

    earcutLinked(outerNode);

    QVector<QPointF> points;
    for (const auto &polygon : polygons)
        for (const auto &point : polygon)
            points.append(point);

    QVector<QPointF> result;
    for (const auto &index : m_indices)
        result.append(points.at(index));

    qDeleteAll(m_nodes);
    m_nodes.clear();

    return result;
}

Node *PolygonProcPrivate::linkedList(const QPolygonF& polygon, const bool clockwise)
{
    const qint32 len = polygon.size();
    Node *last = nullptr;
    qreal sum = 0.0f;

    for (qint32 i = 0, j = len > 0 ? len - 1 : 0; i < len; j = i++) {
        const QPointF &p1 = polygon.at(i);
        const QPointF &p2 = polygon.at(j);
        sum += (p2.x() - p1.x()) * (p1.y() + p2.y());
    }

    if (clockwise == (sum > 0)) {
        for (qint32 i = 0; i < len; i++)
            last = insertNode(m_vertices + i, polygon.at(i), last);
    } else {
        for (qint32 i = len; i-- > 0;)
            last = insertNode(m_vertices + i, polygon.at(i), last);
    }

    if (last && equals(last, last->next)) {
        removeNode(last);
        last = last->next;
    }

    m_vertices += len;

    return last;
}

Node *PolygonProcPrivate::filterPoints(Node *start, Node *end)
{
    if (!end)
        end = start;

    Node *p = start;
    bool again = false;

    do {
        again = false;

        if (!p->steiner && (equals(p, p->next) || qFuzzyIsNull(area(p->prev, p, p->next)))) {
            removeNode(p);
            p = end = p->prev;

            if (p == p->next)
                break;

            again = true;
        } else {
            p = p->next;
        }
    } while (again || p != end);

    return end;
}

void PolygonProcPrivate::earcutLinked(Node *ear, qint32 pass)
{
    if (!ear)
        return;

    if (pass == 0 && m_hashing)
        indexCurve(ear);

    Node *stop = ear;
    Node *prev = nullptr;
    Node *next = nullptr;
    qint32 iterations = 0;

    while (ear->prev != ear->next) {
        prev = ear->prev;
        next = ear->next;
        iterations++;

        if (m_hashing ? isEarHashed(ear) : isEar(ear)) {
            m_indices.append(prev->i);
            m_indices.append(ear->i);
            m_indices.append(next->i);

            removeNode(ear);

            ear = next->next;
            stop = next->next;

            continue;
        }

        ear = next;

        if (ear == stop) {
            if (pass == 0) {
                earcutLinked(filterPoints(ear), 1);
            } else if (pass == 1) {
                ear = cureLocalIntersections(filterPoints(ear));
                earcutLinked(ear, 2);
            } else if (pass == 2) {
                splitEarcut(ear);
            }

            break;
        }
    }
}

bool PolygonProcPrivate::isEar(Node *ear)
{
    const Node *a = ear->prev;
    const Node *b = ear;
    const Node *c = ear->next;

    if (area(a, b, c) >= 0.0f)
        return false;

    Node *p = ear->next->next;

    while (p != ear->prev) {
        if (pointInTriangle(a->x, a->y, b->x, b->y, c->x, c->y, p->x, p->y)
            && area(p->prev, p, p->next) >= 0.0f)
            return false;
        p = p->next;
    }

    return true;
}

bool PolygonProcPrivate::isEarHashed(Node *ear)
{
    const Node *a = ear->prev;
    const Node *b = ear;
    const Node *c = ear->next;

    if (area(a, b, c) >= 0.0f)
        return false;

    const qreal minTX = qMin(a->x, qMin(b->x, c->x));
    const qreal minTY = qMin(a->y, qMin(b->y, c->y));
    const qreal maxTX = qMax(a->x, qMax(b->x, c->x));
    const qreal maxTY = qMax(a->y, qMax(b->y, c->y));
    const qint32 minZ = zOrder(minTX, minTY);
    const qint32 maxZ = zOrder(maxTX, maxTY);

    Node *p = ear->nextZ;

    while (p && p->z <= maxZ) {
        if (p != ear->prev && p != ear->next
            && pointInTriangle(a->x, a->y, b->x, b->y, c->x, c->y, p->x, p->y)
            && area(p->prev, p, p->next) >= 0.0f)
            return false;
        p = p->nextZ;
    }

    p = ear->prevZ;

    while (p && p->z >= minZ) {
        if (p != ear->prev && p != ear->next
            && pointInTriangle(a->x, a->y, b->x, b->y, c->x, c->y, p->x, p->y)
            && area(p->prev, p, p->next) >= 0.0f)
            return false;
        p = p->prevZ;
    }

    return true;
}

Node *PolygonProcPrivate::cureLocalIntersections(Node *start)
{
    Node *p = start;

    do {
        Node *a = p->prev;
        Node *b = p->next->next;

        if (!equals(a, b) && intersects(a, p, p->next, b) && locallyInside(a, b) && locallyInside(b, a)) {
            m_indices.append(a->i);
            m_indices.append(p->i);
            m_indices.append(b->i);

            removeNode(p);
            removeNode(p->next);

            p = start = b;
        }
        p = p->next;
    } while (p != start);

    return filterPoints(p);
}

void PolygonProcPrivate::splitEarcut(Node *start)
{
    Node *a = start;

    do {
        Node *b = a->next->next;
        while (b != a->prev) {
            if (a->i != b->i && isValidDiagonal(a, b)) {
                Node *c = splitPolygon(a, b);

                a = filterPoints(a, a->next);
                c = filterPoints(c, c->next);

                earcutLinked(a);
                earcutLinked(c);

                return;
            }
            b = b->next;
        }
        a = a->next;
    } while (a != start);
}

Node *PolygonProcPrivate::eliminateHoles(const QVector<QPolygonF> &polygon, Node *outerNode)
{
    QVector<Node *> queue;
    for (qint32 i = 1; i < polygon.size(); i++) {
        Node *list = linkedList(polygon[i], false);
        if (list) {
            if (list == list->next)
                list->steiner = true;
            queue.append(getLeftmost(list));
        }
    }

    std::sort(queue.begin(), queue.end(), [](const Node *a, const Node *b) {
        return a->x < b->x;
    });

    for (qint32 i = 0; i < queue.size(); i++) {
        outerNode = eliminateHole(queue[i], outerNode);
        outerNode = filterPoints(outerNode, outerNode->next);
    }

    return outerNode;
}

Node *PolygonProcPrivate::eliminateHole(Node *hole, Node *outerNode)
{
    Node *bridge = findHoleBridge(hole, outerNode);
    if (!bridge)
        return outerNode;

    Node *bridgeReverse = splitPolygon(bridge, hole);
    Node *filteredBridge = filterPoints(bridge, bridge->next);
    filterPoints(bridgeReverse, bridgeReverse->next);

    return outerNode == bridge ? filteredBridge : outerNode;
}

Node *PolygonProcPrivate::findHoleBridge(Node *hole, Node *outerNode)
{
    qreal hx = hole->x;
    qreal hy = hole->y;
    qreal qx = -std::numeric_limits<qreal>::infinity();
    Node *p = outerNode;
    Node *m = nullptr;

    do {
        if (hy <= p->y && hy >= p->next->y && p->next->y != p->y) {
            qreal x = p->x + (hy - p->y) * (p->next->x - p->x) / (p->next->y - p->y);
            if (x <= hx && x > qx) {
                qx = x;
                if (x == hx) {
                    if (hy == p->y)
                        return p;

                    if (hy == p->next->y)
                        return p->next;
                }
                m = p->x < p->next->x ? p : p->next;
            }
        }
        p = p->next;
    } while (p != outerNode);

    if (!m)
        return nullptr;

    if (hx == qx)
        return m;

    const Node *stop = m;
    qreal tanMin = std::numeric_limits<qreal>::infinity();
    qreal tanCur = 0.0f;

    p = m;
    qreal mx = m->x;
    qreal my = m->y;

    do {
        if (hx >= p->x && p->x >= mx && hx != p->x
            && pointInTriangle(hy < my ? hx : qx, hy, mx, my, hy < my ? qx : hx, hy, p->x, p->y)) {

            tanCur = std::abs(hy - p->y) / (hx - p->x);

            if (locallyInside(p, hole)
                && (tanCur < tanMin || (tanCur == tanMin && (p->x > m->x || sectorContainsSector(m, p))))) {
                m = p;
                tanMin = tanCur;
            }
        }

        p = p->next;
    } while (p != stop);

    return m;
}

bool PolygonProcPrivate::sectorContainsSector(const Node *m, const Node *p)
{
    return area(m->prev, m, p->prev) < 0.0f && area(p->next, m, m->next) < 0.0f;
}

void PolygonProcPrivate::indexCurve(Node *start)
{
    Node *p = start;

    do {
        p->z = p->z ? p->z : zOrder(p->x, p->y);
        p->prevZ = p->prev;
        p->nextZ = p->next;
        p = p->next;
    } while (p != start);

    p->prevZ->nextZ = nullptr;
    p->prevZ = nullptr;

    sortLinked(p);
}

Node *PolygonProcPrivate::sortLinked(Node *list)
{
    Node *p = nullptr;
    Node *q = nullptr;
    Node *e = nullptr;
    Node *tail = nullptr;
    qint32 numMerges = 0;
    qint32 pSize = 0;
    qint32 qSize = 0;
    qint32 inSize = 1;

    for (;;) {
        p = list;
        list = nullptr;
        tail = nullptr;
        numMerges = 0;

        while (p) {
            numMerges++;
            q = p;
            pSize = 0;

            for (qint32 i = 0; i < inSize; i++) {
                pSize++;
                q = q->nextZ;

                if (!q)
                    break;
            }

            qSize = inSize;

            while (pSize > 0 || (qSize > 0 && q)) {
                if (pSize == 0) {
                    e = q;
                    q = q->nextZ;
                    qSize--;
                } else if (qSize == 0 || !q) {
                    e = p;
                    p = p->nextZ;
                    pSize--;
                } else if (p->z <= q->z) {
                    e = p;
                    p = p->nextZ;
                    pSize--;
                } else {
                    e = q;
                    q = q->nextZ;
                    qSize--;
                }

                if (tail)
                    tail->nextZ = e;
                else
                    list = e;

                e->prevZ = tail;
                tail = e;
            }

            p = q;
        }

        tail->nextZ = nullptr;

        if (numMerges <= 1)
            return list;

        inSize *= 2;
    }
}

qint32 PolygonProcPrivate::zOrder(const qreal fx, const qreal fy)
{
    qint32 x = static_cast<qint32>(32767.0f * (fx - m_minX) * m_invSize);
    qint32 y = static_cast<qint32>(32767.0f * (fy - m_minY) * m_invSize);

    x = (x | (x << 8)) & 0x00FF00FF;
    x = (x | (x << 4)) & 0x0F0F0F0F;
    x = (x | (x << 2)) & 0x33333333;
    x = (x | (x << 1)) & 0x55555555;

    y = (y | (y << 8)) & 0x00FF00FF;
    y = (y | (y << 4)) & 0x0F0F0F0F;
    y = (y | (y << 2)) & 0x33333333;
    y = (y | (y << 1)) & 0x55555555;

    return x | (y << 1);
}

Node *PolygonProcPrivate::getLeftmost(Node *start)
{
    Node *p = start;
    Node *leftmost = start;

    do {
        if (p->x < leftmost->x || (p->x == leftmost->x && p->y < leftmost->y))
            leftmost = p;
        p = p->next;
    } while (p != start);

    return leftmost;
}

bool PolygonProcPrivate::pointInTriangle(qreal ax, qreal ay, qreal bx, qreal by, qreal cx, qreal cy, qreal px, qreal py) const
{
    return (cx - px) * (ay - py) - (ax - px) * (cy - py) >= 0.0f
           && (ax - px) * (by - py) - (bx - px) * (ay - py) >= 0.0f
           && (bx - px) * (cy - py) - (cx - px) * (by - py) >= 0.0f;
}

bool PolygonProcPrivate::isValidDiagonal(Node *a, Node *b)
{
    return a->next->i != b->i && a->prev->i != b->i && !intersectsPolygon(a, b)
           && ((locallyInside(a, b) && locallyInside(b, a) && middleInside(a, b)
                && (!qFuzzyIsNull(area(a->prev, a, b->prev)) || !qFuzzyIsNull(area(a, b->prev, b))))
               || (equals(a, b) && area(a->prev, a, a->next) > 0.0f && area(b->prev, b, b->next) > 0.0f));
}

qreal PolygonProcPrivate::area(const Node *p, const Node *q, const Node *r) const
{
    if (!p || !q || !r)
        return 0.0f;

    return (q->y - p->y) * (r->x - q->x) - (q->x - p->x) * (r->y - q->y);
}

bool PolygonProcPrivate::equals(const Node *p1, const Node *p2)
{
    if (!p1 || !p2)
        return false;

    return p1->x == p2->x && p1->y == p2->y;
}

bool PolygonProcPrivate::intersects(const Node *p1, const Node *q1, const Node *p2, const Node *q2)
{
    qint32 o1 = sign(area(p1, q1, p2));
    qint32 o2 = sign(area(p1, q1, q2));
    qint32 o3 = sign(area(p2, q2, p1));
    qint32 o4 = sign(area(p2, q2, q1));

    if (o1 != o2 && o3 != o4)
        return true;

    if (o1 == 0 && onSegment(p1, p2, q1))
        return true;

    if (o2 == 0 && onSegment(p1, q2, q1))
        return true;

    if (o3 == 0 && onSegment(p2, p1, q2))
        return true;

    if (o4 == 0 && onSegment(p2, q1, q2))
        return true;

    return false;
}

bool PolygonProcPrivate::onSegment(const Node *p, const Node *q, const Node *r)
{
    return q->x <= qMax(p->x, r->x)
           && q->x >= qMin(p->x, r->x)
           && q->y <= qMax(p->y, r->y)
           && q->y >= qMin(p->y, r->y);
}

qint32 PolygonProcPrivate::sign(qreal val)
{
    return (0.0f < val) - (val < 0.0f);
}

bool PolygonProcPrivate::intersectsPolygon(const Node *a, const Node *b)
{
    const Node *p = a;

    do {
        if (p->i != a->i && p->next->i != a->i && p->i != b->i && p->next->i != b->i
            && intersects(p, p->next, a, b))
            return true;
        p = p->next;
    } while (p != a);

    return false;
}

bool PolygonProcPrivate::locallyInside(const Node *a, const Node *b)
{
    return area(a->prev, a, a->next) < 0.0f
           ? area(a, b, a->next) >= 0.0f && area(a, a->prev, b) >= 0.0f
           : area(a, b, a->prev) < 0.0f || area(a, a->next, b) < 0.0f;
}

bool PolygonProcPrivate::middleInside(const Node *a, const Node *b)
{
    const Node *p = a;
    const qreal px = (a->x + b->x) / 2.0f;
    const qreal py = (a->y + b->y) / 2.0f;
    bool inside = false;

    do {
        if (((p->y > py) != (p->next->y > py)) && p->next->y != p->y
            && (px < (p->next->x - p->x) * (py - p->y) / (p->next->y - p->y) + p->x))
            inside = !inside;
        p = p->next;
    } while (p != a);

    return inside;
}

Node *PolygonProcPrivate::splitPolygon(Node *a, Node *b)
{
    Node *a2 = new Node(a->i, a->x, a->y);
    Node *b2 = new Node(b->i, b->x, b->y);
    Node *an = a->next;
    Node *bp = b->prev;

    m_nodes.append(a2);
    m_nodes.append(b2);

    a->next = b;
    b->prev = a;

    a2->next = an;
    an->prev = a2;

    b2->next = a2;
    a2->prev = b2;

    bp->next = b2;
    b2->prev = bp;

    return b2;
}

Node *PolygonProcPrivate::insertNode(qint32 i, const QPointF &pt, Node *last)
{
    Node *p = new Node(i, pt.x(), pt.y());
    m_nodes.append(p);

    if (!last) {
        p->prev = p;
        p->next = p;
    } else {
        p->next = last->next;
        p->prev = last;
        last->next->prev = p;
        last->next = p;
    }

    return p;
}

void PolygonProcPrivate::removeNode(Node *p)
{
    if (!p)
        return;

    p->next->prev = p->prev;
    p->prev->next = p->next;

    if (p->prevZ)
        p->prevZ->nextZ = p->nextZ;

    if (p->nextZ)
        p->nextZ->prevZ = p->prevZ;
}

PolygonProcPrivate PolygonProc::m_pData = PolygonProcPrivate();

QVector<QPointF> PolygonProc::triangulate(const QPainterPath &path)
{
    return m_pData.triangulate(path);
}

QVector<QPointF> PolygonProc::triangulate(const QVector<QPolygonF> &polygons)
{
    return m_pData.triangulate(polygons);
}
