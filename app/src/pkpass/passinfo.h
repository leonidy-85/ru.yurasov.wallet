#ifndef PASSINFO_H
#define PASSINFO_H

#include <QObject>
#include <QStringList>
#include <QDateTime>

class PassInfo : public QObject
{
    Q_OBJECT
public:
    explicit PassInfo(QObject *parent = 0);
    explicit PassInfo(QString id, QDateTime updated, QStringList changes);

    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QDateTime updated READ updated WRITE setUpdated NOTIFY updatedChanged)
    Q_PROPERTY(QStringList changes READ changes WRITE setChanges NOTIFY changesChanged)

    QString id() { return m_id; }
    void setId(QString value) { m_id = value; emit idChanged(m_id); }
    QDateTime updated() { return m_updated; }
    void setUpdated(QDateTime value) { m_updated = value; emit updatedChanged(m_updated); }
    QStringList changes() { return m_changes; }
    void setChanges(QStringList value) { m_changes = value; emit changesChanged(m_changes); }

signals:
    void idChanged(QString id);
    void updatedChanged(QDateTime updated);
    void changesChanged(QStringList changes);

public slots:

private:
    QString m_id;
    QDateTime m_updated;
    QStringList m_changes;
};

#endif // PASSINFO_H
