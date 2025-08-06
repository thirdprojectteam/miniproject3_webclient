#include "networkreader.h"

#include <QJsonParseError>
#include <QJsonDocument>

NetWorkReader::NetWorkReader(QObject *parent)
    : QObject{parent}
{}

void NetWorkReader::replyread(QNetworkReply *reply)
{
    // 에러 체크, readAll(), JSON 파싱
    QByteArray raw = reply->readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(raw, &err);
    if (err.error == QJsonParseError::NoError) {
        if (doc.isObject())   emit jsonObjectReady(doc.object());
        else if (doc.isArray()) emit jsonArrayReady(doc.array());
        //else                  emit parseError("Unknown JSON type");
    } else {
        //emit parseError(err.errorString());
    }
    reply->deleteLater();
}
