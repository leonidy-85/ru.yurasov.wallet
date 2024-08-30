// ImageProcessor.h
#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QImage>
#include <QObject>
#include <QBuffer>
#include <QByteArray>
#include <QImageReader>
//#include <QDebug>

class ImageProcessor : public QObject {
    Q_OBJECT
public:
    explicit ImageProcessor(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE QString scaleImage(const QString &base64Image, int width, int height) {
//        QImage originalImage(imagePath);
//        if (originalImage.isNull()) {
//            return QString(); // Возвращаем пустую строку, если изображение не удалось загрузить
//        }

         QByteArray imageData = QByteArray::fromBase64(base64Image.toUtf8());
         QImage image;

          image.loadFromData(imageData, "PNG");

//        qDebug() << "width" << width;
//        qDebug() << "height" << height;

        // Масштабируем изображение
        QImage scaledImage = image.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Конвертируем в Base64
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        scaledImage.save(&buffer, "PNG"); // Сохраняем изображение в буфер в формате PNG

        return QString(byteArray.toBase64()); // Возвращаем Base64 строку
    }
};


#endif // IMAGEPROCESSOR_H
