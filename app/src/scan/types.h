// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DSTYPES_H
#define DSTYPES_H

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtCore/QVector>
#include <QtGui/QImage>

class DSPageSize final
{
    Q_GADGET

    Q_PROPERTY(Id sizeId READ sizeId WRITE setSizeId)
    Q_PROPERTY(QSize sizeMm READ sizeMm WRITE setSizeMm)

public:
    enum Id {
        A0, // 841 x 1189 mm
        A1, // 594 x 841 mm
        A2, // 420 x 594 mm
        A3, // 297 x 420 mm
        A4, // 210 x 297 mm
        A5, // 148 x 210 mm
        A6, // 105 x 148 mm
        Custom, // custom size in mm
    };
    Q_ENUM(Id)

public:
    DSPageSize();
    DSPageSize(Id sizeId);
    DSPageSize(const QSize &sizeMm);

    Q_INVOKABLE void setSizeId(Id sizeId);
    Q_INVOKABLE Id sizeId() const;

    Q_INVOKABLE void setSizeMm(const QSize &sizeMm);
    Q_INVOKABLE QSize sizeMm() const;

    friend bool operator==(const DSPageSize &left, const DSPageSize &right);
    friend bool operator!=(const DSPageSize &left, const DSPageSize &right);

private:
    Id m_sizeId { Custom };
    QSize m_sizeMm {  };
};

Q_DECLARE_METATYPE(DSPageSize)
Q_DECLARE_METATYPE(DSPageSize::Id)

class DSFrame final
{
    Q_GADGET

    Q_PROPERTY(ImageType imageType MEMBER imageType)
    Q_PROPERTY(QImage image MEMBER image)
    Q_PROPERTY(QImage alignedImage MEMBER alignedImage)
    Q_PROPERTY(QVector<QPointF> points MEMBER points)

public:
    enum ImageType : qint32 {
        Video,
        Photo,
        Unknown,
    };
    Q_ENUM(ImageType)

public:
    inline bool operator==(const DSFrame &rhs) const {
        return imageType == rhs.imageType
               && image == rhs.image
               && alignedImage == rhs.alignedImage
               && points == rhs.points;
    }
    inline bool operator!=(const DSFrame &rhs) const {
        return !(*this == rhs);
    }

public:
    ImageType imageType { ImageType::Unknown };
    QImage image {  };
    QImage alignedImage {  };
    QVector<QPointF> points {  };
};

Q_DECLARE_METATYPE(DSFrame)

#endif // DSTYPES_H
