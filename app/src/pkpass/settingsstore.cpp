#include <QCoreApplication>
#include <QSettings>
#include <QStandardPaths>
#include "settingsstore.h"

SettingsStore::SettingsStore(QObject *parent) :
    QObject(parent),
    m_settings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/" + QCoreApplication::applicationName() + ".conf",
             QSettings::NativeFormat)
{
}

bool SettingsStore::checkTime() {
    return m_settings.value("time/check", true).toBool();
}

void SettingsStore::setCheckTime(bool value) {
    bool oldValue = checkTime();
    m_settings.setValue("time/check", value);
    if (value != oldValue)
        emit checkTimeChanged();
}

int SettingsStore::hoursBefore() {
    return m_settings.value("time/hours_before", 4).toInt();
}

void SettingsStore::setHoursBefore(int value) {
    int oldValue = hoursBefore();
    m_settings.setValue("time/hours_before", value);
    if (value != oldValue)
        emit hoursBeforeChanged();
}

int SettingsStore::hoursAfter() {
    return m_settings.value("time/hours_after", 4).toInt();
}

void SettingsStore::setHoursAfter(int value) {
    int oldValue = hoursAfter();
    m_settings.setValue("time/hours_after", value);
    if (value != oldValue)
        emit hoursAfterChanged();
}

bool SettingsStore::checkDistance() {
    return m_settings.value("distance/check", false).toBool();
}

void SettingsStore::setCheckDistance(bool value) {
    bool oldValue = checkDistance();
    m_settings.setValue("distance/check", value);
    if (value != oldValue)
        emit checkDistanceChanged();
}

bool SettingsStore::useHere() {
    return m_settings.value("distance/use_here", false).toBool();
}

void SettingsStore::setUseHere(bool value) {
    bool oldValue = useHere();
    m_settings.setValue("distance/use_here", value);
    if (value != oldValue)
        emit useHereChanged();
}

int SettingsStore::maxDistance() {
    return m_settings.value("distance/max", 1000).toInt();
}

void SettingsStore::setMaxDistance(int value) {
    int oldValue = maxDistance();
    m_settings.setValue("distance/max", value);
    if (value != oldValue)
        emit maxDistanceChanged();
}

bool SettingsStore::overrideDistance() {
    return m_settings.value("distance/override", true).toBool();
}

void SettingsStore::setOverrideDistance(bool value) {
    bool oldValue = overrideDistance();
    m_settings.setValue("distance/override", value);
    if (value != oldValue)
        emit overrideDistanceChanged();
}

bool SettingsStore::barcodeTap() {
    return m_settings.value("barcode/tap", false).toBool();
}

void SettingsStore::setBarcodeTap(bool value) {
    bool oldValue = barcodeTap();
    m_settings.setValue("barcode/tap", value);
    if (value != oldValue)
        emit barcodeTapChanged();
}
