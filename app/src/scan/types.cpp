// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <QtCore/QHash>

#include "types.h"

const QHash<DSPageSize::Id, QSize> pageSizes = {
    { DSPageSize::A0, QSize(841, 1189) },
    { DSPageSize::A1, QSize(594, 841) },
    { DSPageSize::A2, QSize(420, 594) },
    { DSPageSize::A3, QSize(297, 420) },
    { DSPageSize::A4, QSize(210, 297) },
    { DSPageSize::A5, QSize(148, 210) },
    { DSPageSize::A6, QSize(105, 148) },
};

DSPageSize::DSPageSize()
    : m_sizeId(Custom), m_sizeMm(QSize(1, 1))
{
}

DSPageSize::DSPageSize(Id sizeId)
    : m_sizeId(sizeId), m_sizeMm(pageSizes.value(sizeId, QSize(1, 1)))
{
}

DSPageSize::DSPageSize(const QSize &sizeMm)
    : m_sizeId(pageSizes.key(sizeMm, Custom)), m_sizeMm(sizeMm)
{
}

void DSPageSize::setSizeId(Id sizeId)
{
    m_sizeId = sizeId;
    m_sizeMm = pageSizes.value(sizeId, m_sizeMm);
}

DSPageSize::Id DSPageSize::sizeId() const
{
    return m_sizeId;
}

void DSPageSize::setSizeMm(const QSize &sizeMm)
{
    m_sizeId = pageSizes.key(sizeMm, Custom);
    m_sizeMm = sizeMm;
}

QSize DSPageSize::sizeMm() const
{
    return m_sizeMm;
}

bool operator==(const DSPageSize &left, const DSPageSize &right)
{
    return left.m_sizeId == right.m_sizeId && left.m_sizeMm == right.m_sizeMm;
}

bool operator!=(const DSPageSize &left, const DSPageSize &right)
{
    return !(left == right);
}
