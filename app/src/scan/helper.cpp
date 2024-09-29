// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include "helper.h"
#include "page.h"
#include "document.h"

DSHelper::DSHelper(QObject *parent)
    : QObject(parent)
{
}

DSPageSize DSHelper::pageSize(DSPageSize::Id sizeId)
{
    return DSPageSize(sizeId);
}

DSPageSize DSHelper::pageSize(const QSize &sizeMm)
{
    return DSPageSize(sizeMm);
}

DSPage *DSHelper::createPage()
{
    return new DSPage(nullptr);
}

DSDocument *DSHelper::createDocument()
{
    return new DSDocument(nullptr);
}

QSizeF DSHelper::scale(const QSizeF &size, const QSizeF &maxSize)
{
    return size.scaled(maxSize, Qt::KeepAspectRatio);
}
