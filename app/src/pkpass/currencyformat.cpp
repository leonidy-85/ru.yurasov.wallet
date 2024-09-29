#include "currencyformat.h"

CurrencyFormat::CurrencyFormat(QObject *parent) :
    QObject(parent),
    m_currencies(),
    m_decimalPoint(),
    m_symbolPrecedes(true)
{
    // get the local monetary decimal point
    setlocale(LC_ALL, "");
    lconv* lc = localeconv();
    m_decimalPoint = QString(lc->mon_decimal_point);
    if (m_decimalPoint == "")
        m_decimalPoint = QString(lc->decimal_point);  // fallback
    // currency symbol after the amount?
    if (!lc->p_cs_precedes)
        m_symbolPrecedes = false;
    // get the currency list
    QFile currencyFile(Aurora::Application::pathTo("qml/components/currencies.json").path());
    if (currencyFile.open(QFile::ReadOnly | QFile::Text)) {
        m_currencies = QJsonDocument::fromJson(currencyFile.readAll());
    }
}

QString CurrencyFormat::format(double value, QString isoName) {
    // search the currency in the currency list
    QJsonObject currency = m_currencies.object().value(isoName).toObject();
    QString symbol(isoName);
    int fractional = 2;
    QString mark(m_decimalPoint);
    if (!currency.isEmpty()) {
        symbol = currency.value("symbol").toString();
        fractional = currency.value("fractional").toInt();
        if (currency.contains("mark"))
            mark = currency.value("mark").toString();
    }
    // format the value
    double integral;
    double decimal = modf(value, &integral);
    QString formatted;
    if (m_symbolPrecedes)
        formatted = symbol + " ";
    formatted += QString::number(integral, 'f', 0);
    if (decimal != 0.0) {
        decimal = floor(fabs(decimal) * pow(10, fractional));
        formatted += mark + QString::number(decimal, 'f', 0);
    }
    if (!m_symbolPrecedes)
        formatted += " " + symbol;
    return formatted;
}
