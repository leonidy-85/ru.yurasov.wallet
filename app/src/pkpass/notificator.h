#ifndef NOTIFICATOR_H
#define NOTIFICATOR_H

#include <QObject>
#include <QList>
#include <QVariantList>
#include <QDBusConnection>
#include <nemonotifications-qt5/notification.h>

class Notificator : public QObject
{
    Q_OBJECT
public:
    explicit Notificator(QObject *parent = 0);
    ~Notificator();

    Q_INVOKABLE void addNotification(QString origin, QString summary, QString body);
    Q_INVOKABLE void removeNotification(QString origin);
    Q_INVOKABLE void bannerNotification(QString summary, QString body);

signals:

public slots:

private:
    QList<Notification*> m_notifications;
};

#endif // NOTIFICATOR_H
