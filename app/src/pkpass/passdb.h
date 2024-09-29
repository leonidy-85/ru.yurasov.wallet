#ifndef PASSDB_H
#define PASSDB_H

#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "passinfo.h"

class PassDB : public QObject
{
    Q_OBJECT
public:
    explicit PassDB(QObject *parent = 0);

    Q_INVOKABLE QStringList getPassIDs();
    Q_INVOKABLE PassInfo* getPassInfo(QString id);
    Q_INVOKABLE QStringList getPassChanges(QString id);
    Q_INVOKABLE void setPassInfo(PassInfo* info);
    Q_INVOKABLE void deletePassInfo(QString id);

signals:

public slots:

private:
    QSqlDatabase m_db;
};

#endif // PASSDB_H
