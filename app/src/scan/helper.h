// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DSHELPER_H
#define DSHELPER_H

#include <QtCore/QObject>

#include "types.h"

class DSPage;
class DSDocument;

class DSHelper : public QObject
{
    Q_OBJECT

public:
    explicit DSHelper(QObject *parent = nullptr);

    Q_INVOKABLE static QSizeF scale(const QSizeF &size, const QSizeF &maxSize);

    Q_INVOKABLE static DSPageSize pageSize(DSPageSize::Id sizeId);
    Q_INVOKABLE static DSPageSize pageSize(const QSize &sizeMm);

    Q_INVOKABLE static DSPage *createPage();
    Q_INVOKABLE static DSDocument *createDocument();
};

Q_DECLARE_METATYPE(DSHelper *)

#endif // DSHELPER_H
