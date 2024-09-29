#ifndef ZIPFILEIMAGEPROVIDER_H
#define ZIPFILEIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QPixmap>
#include <QImage>
#include <QByteArray>

#include "zipfile.h"

class ZipFileImageProvider : public QQuickImageProvider
{
public:
    explicit ZipFileImageProvider();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

signals:

public slots:
};

#endif // ZIPFILEIMAGEPROVIDER_H
