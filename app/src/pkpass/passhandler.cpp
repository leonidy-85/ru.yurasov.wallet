#include "passhandler.h"

PassHandler::PassHandler(QObject *parent) :
    QObject(parent),
    m_network(),
    m_replies()
{
    QObject::connect(&m_network, &QNetworkAccessManager::finished, this, &PassHandler::replyFinished);
}

QString PassHandler::getCanonicalPath(QString path) {
    QFileInfo pass(path);
    if (pass.isFile())
        return pass.canonicalFilePath();
    return path;
}

void PassHandler::removePass(QString path) {
    QFile(path).remove();
}

void PassHandler::updatePass(QString path) {
    // get the infos needed for the update
    ZipFile passFile(path);
    if (!passFile.isValid()) {
        emit updateFinished("not updateable");
        return;
    }
    QJsonDocument pass(QJsonDocument::fromJson(passFile.getTextFile("pass.json").toUtf8()));
    QString baseURL(pass.object().value("webServiceURL").toString());
    QString passID(pass.object().value("passTypeIdentifier").toString());
    QString serial(pass.object().value("serialNumber").toString());
    QString auth(pass.object().value("authenticationToken").toString());
    if (baseURL == "" || passID == "" || serial == "" || auth == "") {
        emit updateFinished("not updateable");
        return;
    }
    // build the HTTP request
    QNetworkRequest request(QUrl(baseURL + "/v1/passes/" + passID + "/" + serial));
    request.setRawHeader("Authorization", QByteArray("ApplePass ") + auth.toUtf8());
    PassDB db;
    PassInfo* info = db.getPassInfo(passID + "/" + serial);
    if (!info->updated().isNull())
        request.setRawHeader("If-Modified-Since", m_rfc2616(info->updated()));
    delete info;
    // send the request
    QNetworkReply* reply = m_network.get(request);
    m_replies.insert(reply, path);
}

void PassHandler::replyFinished(QNetworkReply *reply) {
    // check what this is an answer to
    QString path(m_replies.value(reply));
    if (path == "")
        return;
    m_replies.remove(reply);
    reply->deleteLater();
    // Code 304 -> not modified since last update
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 304) {
        emit updateFinished("no new version");
        return;
    }
    // anything else is a network error
    if (reply->error() != QNetworkReply::NoError) {
        emit updateFinished("update failed");
        return;
    }
    // load the reply into a temporary file
    QTemporaryFile tmp;
    QString id;
    if (tmp.open()) {
        m_copyFile(tmp, *reply);
        tmp.seek(0);
        ZipFile zip(tmp.fileName());
        if (!zip.isValid())
            tmp.close();  // invalid reply
        // get the ID for the pass DB
        QJsonDocument json(QJsonDocument::fromJson(zip.getTextFile("pass.json").toUtf8()));
        id = json.object().value("passTypeIdentifier").toString() + "/" + json.object().value("serialNumber").toString();
    }
    if (tmp.isOpen()) {
        // get changes to former version
        QFile passFile(path);
        QStringList changes;
        if (passFile.exists())
            changes = getChanges(path, tmp.fileName());
        tmp.seek(0);
        // overwrite former version
        if (passFile.open(QFile::WriteOnly)) {
            m_copyFile(passFile, tmp);
            passFile.close();
            // update pass DB
            PassDB db;
            PassInfo* info = new PassInfo(id, QDateTime::currentDateTime(), changes);
            db.setPassInfo(info);
            delete info;
            emit updateFinished("ok");
        }
        else {
            emit updateFinished("update failed");
        }
        tmp.close();
    }
    else {
        emit updateFinished("update failed");
    }
}

QMap<QString, QVariant> PassHandler::getFields(QString filename) {
    // get all header, primary, secondary and auxiliary fields from a pass
    QMap<QString, QVariant> fields;
    ZipFile zip(filename);
    if (!zip.isValid())
        return fields;
    QJsonDocument pass(QJsonDocument::fromJson(zip.getTextFile("pass.json").toUtf8()));
    QStringList styles({"boardingPass", "coupon", "eventTicket", "storeCard", "generic"});
    QStringList types({"headerFields", "primaryFields", "secondaryFields", "auxiliaryFields"});
    for (auto style = styles.cbegin(); style != styles.cend(); ++style) {
        for (auto type = types.cbegin(); type != types.cend(); ++type) {
            QJsonArray thisFields(pass.object().value(*style).toObject().value(*type).toArray());
            for (int entry = 0; entry < thisFields.size(); entry++) {
                QJsonObject field(thisFields.at(entry).toObject());
                if (field.contains("key") && field.contains("value"))
                    fields.insert(field.value("key").toString(), field.value("value").toVariant());
            }
        }
    }
    return fields;
}

QStringList PassHandler::getChanges(QString oldfile, QString newfile) {
    // compare the fields from two passes
    QStringList changed;
    QMap<QString, QVariant> oldfields(getFields(oldfile));
    QMap<QString, QVariant> newfields(getFields(newfile));
    for (auto newfield = newfields.cbegin(); newfield != newfields.cend(); ++newfield) {
        if (!oldfields.contains(newfield.key()) || oldfields.value(newfield.key()) != newfield.value())
            changed.append(newfield.key());
    }
    return changed;
}

void PassHandler::createCalendarEntry(QString subject, QString isoDateTime) {
    // get the datetime
    QDateTime dateTime(QDateTime::fromString(isoDateTime, Qt::ISODate));
    if (!dateTime.isValid()) {
        emit calendarEntryFinished("format");
        return;
    }
    // create a temporary iCal file
    QString icaldir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    if (!QDir(icaldir).exists())
        QDir().mkpath(icaldir);
    QFile ical(icaldir + "/passbook.ics");
    if (ical.open(QFile::WriteOnly)) {
        QString w_subject(subject);
        ical.write("BEGIN:VCALENDAR\r\n");
        ical.write("VERSION:2.0\r\n");
        ical.write("PRODID:-//p2501.ch//Pass Viewer 0.10//EN\r\n");
        ical.write("BEGIN:VEVENT\r\n");
        ical.write("UID:" + w_subject.replace(' ', '_').toUtf8() + "/" + isoDateTime.toUtf8() + "/harbour-passviewer\r\n");
        ical.write("DTSTAMP:" + QDateTime::currentDateTimeUtc().toString("yyyyMMddTHHmmssZ").toUtf8() + "\r\n");
        ical.write("DTSTART:" + dateTime.toUTC().toString("yyyyMMddTHHmmssZ").toUtf8() + "\r\n");
        ical.write("SUMMARY:" + subject.toUtf8() + "\r\n");
        ical.write("TRANSP:TRANSPARENT\r\n");
        ical.write("END:VEVENT\r\n");
        ical.write("END:VCALENDAR\r\n");
        ical.close();
        QDesktopServices::openUrl(QUrl("file:/" + ical.fileName()));
    }
}

void PassHandler::m_copyFile(QIODevice &to, QIODevice &from) {
    for (QByteArray data(from.read(4096)); data.size() > 0; data = from.read(4096))
        to.write(data);
}

QByteArray PassHandler::m_rfc2616(QDateTime datetime) {
    // RFC2616 is similar to RFC2822, but requires the weekday, requires UTC, and the timezone must be written as "GMT"
    QString dateString(datetime.toUTC().toString(Qt::RFC2822Date));
    QStringList weekdays({"NULL", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"});
    dateString.prepend(weekdays.at(datetime.date().dayOfWeek()) + ", ");
    dateString.replace("+0000", "GMT");
    return dateString.toUtf8();
}
