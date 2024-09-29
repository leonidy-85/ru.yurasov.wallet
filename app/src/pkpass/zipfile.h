#ifndef ZIPFILE_H
#define ZIPFILE_H

/* This class reads ZIP files using Deflate, BZip2 and/or LZMA compression,
 * which should cover almost anything in the wild. Please note, however, that
 * decompression is done in memory, and the ZIP64 extension is only partially
 * supported. Thus, it is not suitable for large files. */

#include <QObject>
#include <QByteArray>
#include <QMap>
#include <QList>
#include <QFile>
#include <QtEndian>
#include <QTextCodec>

#include <zlib.h>
#include <bzlib.h>
//#include <lzma.h>

class ZipFile : public QObject
{
    Q_OBJECT
public:
    explicit ZipFile(QObject *parent = 0);
    explicit ZipFile(QString filename);

    Q_INVOKABLE bool isValid() { return m_valid; }
    Q_INVOKABLE QByteArray getFile(QString filename);
    Q_INVOKABLE QString getTextFile(QString filename);

signals:

public slots:

private:
    bool m_valid;
    QFile m_file;
    QMap<QString, QList<qint64> > m_entries;

    qint64 m_findMark(QFile &file, QByteArray mark);
    void m_interpretExtras(QFile &file, qint64 markSize, qint64 &size, qint64 &compressedSize, qint64 &offset);
};

#endif // ZIPFILE_H
