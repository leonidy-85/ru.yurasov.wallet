#ifndef DATETIMEFORMAT_H
#define DATETIMEFORMAT_H

#include <QObject>
#include <QDateTime>
#include <QMap>
#include <QByteArray>
#include <locale.h>
#include <langinfo.h>
#include <time.h>

class DateTimeFormat : public QObject
{
    Q_OBJECT
public:
    explicit DateTimeFormat(QObject *parent = 0);

    Q_INVOKABLE QString format(QString dateTime, QString dateFormat, QString timeFormat, bool ignoreTimeZone);

signals:

public slots:

private:
    QMap<QString, QString> m_dateFormats;
    QMap<QString, QString> m_timeFormats;

    QString m_expandCodes(QString orig);
    QString m_clearCodes(QString orig, QString allowed);
};

#endif // DATETIMEFORMAT_H
