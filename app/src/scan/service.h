// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DSSERVICE_H
#define DSSERVICE_H

#include <QtDBus/QDBusAbstractAdaptor>

class DSService : public QDBusAbstractAdaptor
{
    Q_OBJECT

    Q_CLASSINFO("D-Bus Interface", DBUS_INTERFACE)

public:
    explicit DSService(QObject *parent = nullptr);
    ~DSService() override;

    static bool isRegistered();

    static QString dbusService();
    static QString dbusPath();
    static QString dbusIface();
    static QString dbusMethod();

    static void processFilePath(const QString &filePath);

signals:
    void fileOpened();

public slots:
    void openFile(const QString &filePath);
};

#endif // DSSERVICE_H
