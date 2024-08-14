#include "barcodeimageprovider.h"

BarcodeImageProvider::BarcodeImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap) // Указываем тип предоставляемого изображения
{
}

QPixmap BarcodeImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{

    QString type;
    QTextCodec* codec;
    QByteArray content;
    QPixmap empty;

    int firstslash = id.indexOf('/');
    int secondslash = id.indexOf('/', firstslash + 1);
    if (firstslash == -1 || secondslash == -1)
        return empty;
    type = id.left(firstslash);
    codec = QTextCodec::codecForName(id.mid(firstslash + 1, secondslash - firstslash - 1).toUtf8());
    if (!codec)
        return empty;
    // decode the (Base64 encoded UTF-8) barcode content
    content = QByteArray::fromBase64(id.mid(secondslash + 1).toLatin1());
    // Code128 requires UTF-8. For everything else: convert content to the desired encoding.
    if (type != "code128")
        content = codec->fromUnicode(QString::fromUtf8(content.constData(), content.size()));
    // Создаем объекты для генерации штрих-кодов
    zint_symbol *symbol = ZBarcode_Create();

    if (!symbol)
        return empty;
    // Настраиваем параметры штрих-кода
    if (type == "20") {
        symbol->input_mode = UNICODE_MODE;
    }

    if (type == "" || type == NULL) {
        ZBarcode_Delete(symbol);
        return empty;
    }
     symbol->show_hrt = 0;
     symbol->symbology = type.toInt();

    // Устанавливаем данные для кодирования
    QByteArray data = id.mid(secondslash + 1).toUtf8();

    if (ZBarcode_Encode_and_Buffer(symbol, (unsigned char*)data.constData(), data.size(), 0) != 0) {
        ZBarcode_Delete(symbol);
        return empty;
    }

    // Получаем изображение из zint
    QImage barcode(symbol->bitmap_width, symbol->bitmap_height, QImage::Format_RGB32);

  //  QPixmap pixmap = QPixmap::fromImage(img);
    size->setWidth(symbol->bitmap_width);
    size->setHeight(symbol->bitmap_height);
    int bitmapPos = 0;
    for (int row = 0; row < symbol->bitmap_height; row++) {
        QRgb* line = (QRgb*)barcode.scanLine(row);
        for (int col = 0; col < symbol->bitmap_width; col++) {
            line[col] = qRgb((uchar)symbol->bitmap[bitmapPos], (uchar)symbol->bitmap[bitmapPos + 1], (uchar)symbol->bitmap[bitmapPos + 2]);
            bitmapPos += 3;
        }
    }
    // Освобождаем ресурсы
    ZBarcode_Delete(symbol);

    return QPixmap::fromImage(barcode);

}


