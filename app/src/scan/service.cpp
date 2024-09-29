// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtGui/QGuiApplication>
#include <QtGui/QDesktopServices>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>

#include "service.h"

const QString dbusServiceStr = QStringLiteral(DBUS_SERVICE);
const QString dbusPathStr = QStringLiteral(DBUS_PATH);
const QString dbusIfaceStr = QStringLiteral(DBUS_INTERFACE);
const QString dbusMethodStr = QStringLiteral("openFile");

DSService::DSService(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    setAutoRelaySignals(true);

    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject(dbusPathStr, this, QDBusConnection::ExportAllSlots);
    if (!isRegistered()) {
        bool success = dbus.registerService(dbusServiceStr);
        if (!success)
            qApp->quit();
    }
}

DSService::~DSService()
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.unregisterObject(dbusPathStr);
    dbus.unregisterService(dbusServiceStr);
}

bool DSService::isRegistered()
{
    return QDBusConnection::sessionBus().interface()->isServiceRegistered(dbusServiceStr);
}

QString DSService::dbusService()
{
    return dbusServiceStr;
}

QString DSService::dbusPath()
{
    return dbusPathStr;
}

QString DSService::dbusIface()
{
    return dbusIfaceStr;
}

QString DSService::dbusMethod()
{
    return dbusMethodStr;
}

void DSService::processFilePath(const QString &filePath)
{
    QDBusMessage message = QDBusMessage::createMethodCall(dbusServiceStr, dbusPathStr, dbusIfaceStr, dbusMethodStr);
    message.setArguments(QList<QVariant>() << filePath);
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
}

void DSService::openFile(const QString &filePath)
{
    const QUrl url = QUrl::fromLocalFile(filePath);
    if (QFile::exists(url.toLocalFile()))
        QDesktopServices::openUrl(url);

    emit fileOpened();
}
