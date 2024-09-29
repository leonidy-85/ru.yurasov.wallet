#include "zipfile.h"

#pragma pack(1)

struct Eod {
    char signature[4];
    quint16 disks;
    quint16 startDisk;
    quint16 directoriesDisk;
    quint16 directories;
    quint32 directorySize;
    quint32 directoryPos;
};

struct Eodl {
    char signature[4];
    quint32 startDisk;
    quint64 eodPos;
    quint32 disks;
};

struct Eod64 {
    char signature[4];
    quint64 size;
    quint16 version;
    quint16 versionNeeded;
    quint32 disks;
    quint32 startDisk;
    quint64 directoriesDisk;
    quint64 directories;
    quint64 directorySize;
    quint64 directoryPos;
};

struct DirEntry {
    char signature[4];
    quint16 version;
    quint16 versionNeeded;
    quint16 gpbf;
    quint16 compression;
    quint16 mtime;
    quint16 mdate;
    quint32 crc32;
    quint32 compressedSize;
    quint32 size;
    quint16 nameLength;
    quint16 extraLength;
    quint16 commentLength;
    quint16 firstDisk;
    quint16 internalAttributes;
    quint32 externalAttributes;
    quint32 headerPos;
};

struct FileHeader {
    char signature[4];
    quint16 versionNeeded;
    quint16 gpgf;
    quint16 compression;
    quint16 mtime;
    quint16 mdate;
    quint32 crc32;
    quint32 compressedSize;
    quint32 size;
    quint16 nameLength;
    quint16 extraLength;
};

struct ExtraHeader {
    quint16 type;
    quint16 size;
};

#pragma pack()

ZipFile::ZipFile(QObject *parent) :
    QObject(parent),
    m_valid(false),
    m_file(NULL),
    m_entries()
{

}

ZipFile::ZipFile(QString filename) :
    QObject(NULL),
    m_valid(false),
    m_file(filename),
    m_entries()
{
    if (!m_file.open(QFile::ReadOnly))
        return;
    // search the "end of directory" entry
    qint64 pos_eod = m_findMark(m_file, "PK\x05\x06");
    if (pos_eod == -1)
        return;
    // read the "end of directory" entry (with the directory position)
    m_file.seek(pos_eod);
    QByteArray eodString(m_file.read(sizeof(Eod)));
    if (eodString.size() != sizeof(Eod))
        return;
    Eod* eod = (Eod*)eodString.data();
    // read the directory
    quint64 dirPos = qFromLittleEndian<quint32>(eod->directoryPos);
    if (dirPos == 0xffffffff) {
        // ZIP64
        // first get the "end of directory locater"
        pos_eod = m_findMark(m_file, "PK\x06\x07");
        if (pos_eod == -1)
            return;
        m_file.seek(pos_eod);
        QByteArray eodlString(m_file.read(sizeof(Eodl)));
        if (eodlString.size() != sizeof(Eodl))
            return;
        Eodl* eodl = (Eodl*)eodlString.data();
        // now the Zip64 "end of directory"
        pos_eod = qFromLittleEndian<quint64>(eodl->eodPos);
        m_file.seek(pos_eod);
        QByteArray eod64String(m_file.read(sizeof(Eod64)));
        if (eod64String.size() != sizeof(Eod64))
            return;
        Eod64* eod64 = (Eod64*)eod64String.data();
        dirPos = qFromLittleEndian<quint64>(eod64->directoryPos);
    }
    DirEntry* entry = NULL;
    QTextCodec* cp437 = QTextCodec::codecForName("cp437");
    while(dirPos + sizeof(DirEntry) <= m_file.size()) {
        // read the directory entry
        m_file.seek(dirPos);
        QByteArray entryString(m_file.read(sizeof(DirEntry)));
        if (entryString.size() != sizeof(DirEntry))
            break;
        entry = (DirEntry*)entryString.data();
        if (QByteArray(entry->signature, 4) != "PK\x01\x02")
            break;
        dirPos += sizeof(DirEntry) + qFromLittleEndian<quint16>(entry->nameLength) + qFromLittleEndian<quint16>(entry->extraLength) + qFromLittleEndian<quint16>(entry->commentLength);
        // ignore entries with unsupported compression methods
        quint16 compression = qFromLittleEndian<quint16>(entry->compression);
        if (compression == 0 || compression == 8 || compression == 12 || compression == 14) {
            // get the info
            QByteArray rawname(m_file.read(qFromLittleEndian<quint16>(entry->nameLength)));
            QString name;
            if (cp437 != NULL && (qFromLittleEndian<quint16>(entry->gpbf) & 0x0800) == 0)
                name = cp437->toUnicode(rawname);
            else
                name = QString(rawname); // UTF-8
            QList<qint64> entryData;
            entryData.append(compression);
            entryData.append(qFromLittleEndian<quint32>(entry->headerPos));
            entryData.append(qFromLittleEndian<quint32>(entry->compressedSize));
            entryData.append(qFromLittleEndian<quint32>(entry->size));
            m_interpretExtras(m_file, qFromLittleEndian<quint16>(entry->extraLength), entryData[3], entryData[2], entryData[1]);
            m_entries.insert(name, entryData);
        }
    }
    if (m_entries.size() > 0)
        m_valid = true;
}

QByteArray ZipFile::getFile(QString filename) {
    if (!m_entries.contains(filename))
        return QByteArray();
    if (m_entries.value(filename).at(2) == 0)  // empty file
        return QByteArray();
    // get the ZIP file header
    m_file.seek(m_entries.value(filename).at(1));
    QByteArray headerString(m_file.read(sizeof(FileHeader)));
    if (headerString.size() != sizeof(FileHeader))
        return QByteArray();
    FileHeader* header = (FileHeader*)headerString.data();
    if (QByteArray(header->signature, 4) != "PK\x03\x04")
        return QByteArray();
    // read the actual, compressed file
    m_file.seek(m_entries.value(filename).at(1) + sizeof(FileHeader) + qFromLittleEndian<quint16>(header->nameLength) + qFromLittleEndian<quint16>(header->extraLength));
    QByteArray compressed(m_file.read(m_entries.value(filename).at(2)));
    if (compressed.size() != m_entries.value(filename).at(2))
        return QByteArray();
    // decompress (if it's compressed)
    switch (m_entries.value(filename).at(0)) {
    case 0:  // uncompressed
        return compressed;
        break;
    case 8:  // deflate
        {
            compressed.prepend("\x78\x9c");  // ZLib Header
            QByteArray uncompressed(m_entries.value(filename).at(3), '\0');
            uLongf insize = compressed.size();
            uLongf outsize = uncompressed.size();
            uncompress((uchar*)uncompressed.data(), &outsize, (uchar*)compressed.data(), insize);
            return uncompressed;
        }
        break;
    case 12:  // bzip2
        {
            QByteArray uncompressed(m_entries.value(filename).at(3), '\0');
            uint insize = compressed.size();
            uint outsize = uncompressed.size();
            BZ2_bzBuffToBuffDecompress(uncompressed.data(), &outsize, compressed.data(), insize, 0, 0);
            return uncompressed;
        }
        break;
    case 14:  // lzma
        {
//            lzma_stream lzma = LZMA_STREAM_INIT;
//            if (lzma_alone_decoder(&lzma, UINT64_MAX) != LZMA_OK)
//                return QByteArray();
//            QByteArray uncompressed(m_entries.value(filename).at(3), '\0');
//            // to convert from ZIP to LZMA header, remove the first 4 bytes, then insert a little endian quint64 with the uncompressed length at position 5
//            compressed.remove(0, 4);
//            quint64 uncompressedSize = qToLittleEndian<quint64>(uncompressed.size());
//            compressed.insert(5, (char*)&uncompressedSize, 8);
//            lzma.next_in = (uint8_t*)compressed.data();
//            lzma.avail_in = compressed.size();
//            lzma.next_out = (uint8_t*)uncompressed.data();
//            lzma.avail_out = uncompressed.size();
//            lzma_code(&lzma, LZMA_RUN);
//            lzma_end(&lzma);
            //return uncompressed;
            return NULL;
        }
    }
    return QByteArray();
}

QString ZipFile::getTextFile(QString filename) {
    // convenience method for unicode-encoded text files
    QByteArray bytes(getFile(filename));
    QTextCodec* codec = QTextCodec::codecForUtfText(bytes, QTextCodec::codecForName("UTF-8"));
    if (codec != NULL)
        return codec->toUnicode(bytes);
    else
        return QString(bytes);
}

qint64 ZipFile::m_findMark(QFile &file, QByteArray mark) {
    qint64 pos = file.size();
    while (pos > 0) {
        pos = qMax<qint64>(0, pos - (1024 - mark.size()));
        file.seek(pos);
        QByteArray block(file.read(1024));
        qint64 off = block.lastIndexOf(mark);
        if (off >= 0)
            return pos + off;
    }
    return -1;
}

void ZipFile::m_interpretExtras(QFile &file, qint64 markSize, qint64 &size, qint64 &compressedSize, qint64 &offset) {
    qint64 read = 0;
    while (read < markSize) {
        QByteArray extraHeaderString(file.read(sizeof(ExtraHeader)));
        if (extraHeaderString.size() != sizeof(ExtraHeader))
            return;
        ExtraHeader* extraHeader = (ExtraHeader*)extraHeaderString.data();
        qint64 fieldSize = qFromLittleEndian<quint16>(extraHeader->size);
        read += fieldSize + 4;
        if (qFromLittleEndian<quint16>(extraHeader->type) == 1) {
            // ZIP64
            while (fieldSize >= 8) {
                QByteArray zip64val(file.read(8));
                if (zip64val.size() < 8)
                    return;
                qint64 val = qFromLittleEndian<qint64>(*(qint64*)zip64val.data());
                if (size == 0xffffffff)
                    size = val;
                else if (compressedSize == 0xffffffff)
                    compressedSize = val;
                else if (offset == 0xffffffff)
                    offset = val;
            }
        }
        QByteArray dummy(file.read(fieldSize));
    }
}
