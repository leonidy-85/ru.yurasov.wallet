#include "zipfileimageprovider.h"

ZipFileImageProvider::ZipFileImageProvider() :
    QQuickImageProvider(QQuickImageProvider::Pixmap)
{

}

QPixmap ZipFileImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) {
    // get the archive and file from the ID
    QPixmap empty;
    QString zipPath(id.left(id.lastIndexOf('/')));
    QString imgFile(id.right(id.size() - id.lastIndexOf('/') - 1));
    // try to read the image from the archive
    size->setWidth(0);
    size->setHeight(0);
    zipPath.prepend('/');
    ZipFile zip(zipPath);
    if (!zip.isValid())
        return empty;
    QByteArray data;
    // prefer the HD-image if available
    QString hdFile(imgFile.left(imgFile.lastIndexOf('.')) + "@2x" + imgFile.mid(imgFile.lastIndexOf('.')));
    data = zip.getFile(hdFile);
    if (data.size() == 0)
        data = zip.getFile(imgFile);  // fallback to normal image
    if (data.size() == 0)
        return empty;
    // decode the image file
    QImage img(QImage::fromData(data));
    if (img.isNull())
        return empty;
    size->setWidth(img.width());
    size->setHeight(img.height());
    return QPixmap::fromImage(img);
}
