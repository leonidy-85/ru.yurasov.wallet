#ifndef IMAGECONVERTER_H
#define IMAGECONVERTER_H

#include <QObject>
#include <QImage>
#include <QBuffer>
#include <QByteArray>
#include <QDebug>
#include <iostream>
#include <QString>


class ImageConverter : public QObject {
    Q_OBJECT
public:
    explicit ImageConverter(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    QString imageToBase64(const QString &filePath) {


        QString path = filePath;

        qWarning() << path;

        QImage image(path);
        if (image.isNull()) {
            return QString("error");
        }

        qWarning() << "44444=";

        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG");
        return byteArray.toBase64();
    }



    // Новый метод для проверки разрешения изображения
    QString checkImageResolution(const QString &filePath) {
        QImage image(filePath);
        if (image.isNull()) {
            return "Invalid image path";
        }

        int width = image.width();
        int height = image.height();

        // Вернем разрешение в виде строки
        return QString("%1x%2").arg(width).arg(height);
    }
};

#endif
