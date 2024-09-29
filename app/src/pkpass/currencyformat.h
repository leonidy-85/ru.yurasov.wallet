#ifndef CURRENCYFORMAT_H
#define CURRENCYFORMAT_H

#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <auroraapp.h>
#include <locale.h>
#include <math.h>

class CurrencyFormat : public QObject
{
    Q_OBJECT
public:
    explicit CurrencyFormat(QObject *parent = 0);

    Q_INVOKABLE QString format(double value, QString isoName);

signals:

public slots:

private:
    QJsonDocument m_currencies;
    QString m_decimalPoint;
    bool m_symbolPrecedes;
};

#endif // CURRENCYFORMAT_H
