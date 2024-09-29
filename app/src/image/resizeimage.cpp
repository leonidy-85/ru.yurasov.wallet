#include "resizeimage.h"
#include <QImage>
#include <QByteArray>
#include <QPixmap>

//QString resizeImage::imageResize(const QString &base64Image, int x, int y, int width, int height) {
//    // Декодируем изображение из base64
//    QByteArray imageData = QByteArray::fromBase64(base64Image.toUtf8());
//    QImage image;
//    image.loadFromData(imageData, "PNG");

  QString resizeImage::imageResize(QImage image, int x, int y, int width, int height) {
    // Декодируем изображение из base64
//    QByteArray imageData = QByteArray::fromBase64(base64Image.toUtf8());
//    QImage image;
//    image.loadFromData(imageData, "PNG");

    // Обрезаем изображение
    QImage croppedImage = image.copy(x, y, width, height);

    // Возвращаем обрезанное изображение в формате base64
    return imageToBase64(croppedImage);
}

QString resizeImage::imageToBase64(const QImage &image) {
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    return QString::fromUtf8(byteArray.toBase64());
}
