#include "passinfo.h"

PassInfo::PassInfo(QObject *parent) :
    QObject(parent),
    m_id(),
    m_updated(),
    m_changes()
{

}

PassInfo::PassInfo(QString id, QDateTime updated, QStringList changes) :
    QObject(),
    m_id(id),
    m_updated(updated),
    m_changes(changes)
{

}
