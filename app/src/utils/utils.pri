# SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
# SPDX-License-Identifier: BSD-3-Clause

QT += core gui multimedia

INCLUDEPATH += $$PWD

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/imageproc.h \
    $$PWD/polygonproc.h \

SOURCES += \
    $$PWD/imageproc.cpp \
    $$PWD/polygonproc.cpp \
