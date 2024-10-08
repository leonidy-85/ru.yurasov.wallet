#ifndef IMAGECONVERTER_H
#define IMAGECONVERTER_H

#include <QObject>
#include <QImage>
#include <QBuffer>
#include <QByteArray>

class ImageConverter : public QObject {
    Q_OBJECT
public:
    explicit ImageConverter(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    QString imageToBase64(const QString &filePath) {
        QImage image(filePath);
        if (image.isNull()) {
            return QString();
        }

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

#endif // IMAGECONVERTER_H


//#ifndef IMAGECONVERTER_H
//#define IMAGECONVERTER_H

//#include <QObject>
//#include <QImage>
//#include <QBuffer>
//#include <QByteArray>
//#include <QDebug>

//class ImageConverter : public QObject {
//    Q_OBJECT
//public:
//    explicit ImageConverter(QObject *parent = nullptr) : QObject(parent) {}

//public slots:
//    QString imageToBase64(const QString &filePath) {
//        QImage image(filePath);
//            if (image.isNull()) {
//                qDebug() << "Failed to load image from path:" << filePath;
//                return QString();
//            }
//            QByteArray byteArray;
//            QBuffer buffer(&byteArray);
//            buffer.open(QIODevice::WriteOnly);
//             image.save(&buffer, "PNG");
//              qDebug() << "Received value from QML:" << filePath;
//         return byteArray.toBase64();
//    }


//   // Q_INVOKABLE QString imageToBase64(const QString &filePath);
//};

//#endif // IMAGECONVERTER_H
