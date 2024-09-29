#ifndef HOMEWATCHER_H
#define HOMEWATCHER_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QList>
#include <QStringList>
#include <QVariantList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#include "homescanner.h"
#include "passdb.h"

class HomeWatcher : public QFileSystemWatcher
{
    Q_OBJECT
public:
    explicit HomeWatcher(QObject *parent = 0);
    ~HomeWatcher();

    Q_INVOKABLE void updatePaths(const QStringList &paths);

signals:
    void workerScanHome(bool update);
    void passesFound(QVariantList list, bool update);

public slots:
    void scanHome(bool update = false);
    void passesFoundByWorker(QVariantList list, QStringList paths, bool update);

private:
    QThread m_worker;
};

#endif // HOMEWATCHER_H
