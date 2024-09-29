#ifndef SETTINGSSTORE_H
#define SETTINGSSTORE_H

#include <QObject>
#include <QSettings>

class SettingsStore : public QObject
{
    Q_OBJECT
public:
    explicit SettingsStore(QObject *parent = 0);

    Q_PROPERTY(bool checkTime READ checkTime WRITE setCheckTime NOTIFY checkTimeChanged)
    Q_PROPERTY(int hoursBefore READ hoursBefore WRITE setHoursBefore NOTIFY hoursBeforeChanged)
    Q_PROPERTY(int hoursAfter READ hoursAfter WRITE setHoursAfter NOTIFY hoursAfterChanged)
    Q_PROPERTY(bool checkDistance READ checkDistance WRITE setCheckDistance NOTIFY checkDistanceChanged)
    Q_PROPERTY(bool useHere READ useHere WRITE setUseHere NOTIFY useHereChanged)
    Q_PROPERTY(int maxDistance READ maxDistance WRITE setMaxDistance NOTIFY maxDistanceChanged)
    Q_PROPERTY(bool overrideDistance READ overrideDistance WRITE setOverrideDistance NOTIFY overrideDistanceChanged)
    Q_PROPERTY(bool barcodeTap READ barcodeTap WRITE setBarcodeTap NOTIFY barcodeTapChanged)

    bool checkTime();
    void setCheckTime(bool value);
    int hoursBefore();
    void setHoursBefore(int value);
    int hoursAfter();
    void setHoursAfter(int value);
    bool checkDistance();
    void setCheckDistance(bool value);
    bool useHere();
    void setUseHere(bool value);
    int maxDistance();
    void setMaxDistance(int value);
    bool overrideDistance();
    void setOverrideDistance(bool value);
    bool barcodeTap();
    void setBarcodeTap(bool value);

signals:
    void checkTimeChanged();
    void hoursBeforeChanged();
    void hoursAfterChanged();
    void checkDistanceChanged();
    void useHereChanged();
    void maxDistanceChanged();
    void overrideDistanceChanged();
    void barcodeTapChanged();

public slots:

private:
    QSettings m_settings;

};

#endif // SETTINGSSTORE_H
