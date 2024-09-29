#include "passdb.h"

PassDB::PassDB(QObject *parent) :
    QObject(parent),
    m_db(QSqlDatabase::database())
{
    if (m_db.databaseName().endsWith("/passes.db"))
        return;
    // if we don't have a connection yet, create it
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbdir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    if (!QDir(dbdir).exists())
        QDir().mkpath(dbdir);
    m_db.setDatabaseName(dbdir + "/passes.db");
    m_db.open();
    QSqlQuery("PRAGMA foreign_keys = ON", m_db);
    // create the tables, if they don't exist yet
    QSqlQuery checkTable(m_db);
    checkTable.prepare("select count(*) from sqlite_master where type='table' and name = :table");
    QStringList tables({"passes", "changes"});
    for (auto table = tables.cbegin(); table != tables.cend(); ++table) {
        checkTable.bindValue(":table", *table);
        checkTable.exec();
        checkTable.next();
        if (checkTable.value(0).toInt() == 0) {
            if (*table == "passes")
                QSqlQuery("create table passes (pass_id text primary key, updated integer)", m_db);
            else
                QSqlQuery("create table changes ("
                               "change_id integer primary key, "
                               "pass_id text not null references passes (pass_id) on delete cascade, "
                               "field text not null)", m_db);
        }
        checkTable.finish();
    }
}

QStringList PassDB::getPassIDs() {
    QStringList passes;
    QSqlQuery passQuery("select pass_id from passes", m_db);
    while (passQuery.next())
        passes.append(passQuery.value(0).toString());
    return passes;
}

PassInfo* PassDB::getPassInfo(QString id) {
    QDateTime updated;
    QStringList changes;
    QSqlQuery updatedQuery(m_db);
    updatedQuery.prepare("select updated from passes where pass_id = :id");
    updatedQuery.bindValue(":id", id);
    updatedQuery.exec();
    updatedQuery.next();
    int updatedTime_t = updatedQuery.value(0).toInt();
    if (updatedTime_t > 0)
        updated = QDateTime::fromTime_t(updatedTime_t);
    QSqlQuery changesQuery(m_db);
    changesQuery.prepare("select field from changes where pass_id = :id");
    changesQuery.bindValue(":id", id);
    changesQuery.exec();
    while (changesQuery.next())
        changes.append(changesQuery.value(0).toString());
    return new PassInfo(id, updated, changes);
}

QStringList PassDB::getPassChanges(QString id) {
    // get just the changes, and remove them from the DB
    PassInfo* info = getPassInfo(id);
    QStringList changes = info->changes();
    info->setChanges(QStringList());
    setPassInfo(info);
    delete info;
    return changes;
}

void PassDB::setPassInfo(PassInfo *info) {
    QSqlQuery deleteQuery(m_db);
    deleteQuery.prepare("delete from passes where pass_id = :id");
    deleteQuery.bindValue(":id", info->id());
    deleteQuery.exec();
    if (info->changes().size() > 0 || !info->updated().isNull()) {
        QSqlQuery passQuery(m_db);
        passQuery.prepare("insert into passes values (:id, :updated)");
        passQuery.bindValue(":id", info->id());
        passQuery.bindValue(":updated", info->updated().toTime_t());
        passQuery.exec();
        QSqlQuery changesQuery(m_db);
        changesQuery.prepare("insert into changes (pass_id, field) values (:id, :field)");
        for (auto change = info->changes().cbegin(); change != info->changes().cend(); ++change) {
            changesQuery.bindValue(":id", info->id());
            changesQuery.bindValue(":field", *change);
            changesQuery.exec();
            changesQuery.finish();
        }
    }
}

void PassDB::deletePassInfo(QString id) {
    QSqlQuery deleteQuery(m_db);
    deleteQuery.prepare("delete from passes where pass_id = :id");
    deleteQuery.bindValue(":id", id);
    deleteQuery.exec();
}
