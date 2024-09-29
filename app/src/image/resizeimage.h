#ifndef RESIZEIMAGE_H
#define RESIZEIMAGE_H

#include <QObject>
#include <QImage>
#include <QBuffer>
#include <QPixmap>


class resizeImage : public QObject {
    Q_OBJECT
public:
    explicit resizeImage(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE QString imageResize(QImage image, int x, int y, int width, int height);

private:
    QString imageToBase64(const QImage &image);
};


#endif // RESIZEIMAGE_H
