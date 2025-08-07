#include "networkreader.h"
#include <QJsonParseError>
#include <QJsonDocument>

NetWorkReader::NetWorkReader(QObject *parent)
    : QObject{parent}
{
}

void NetWorkReader::replyread(QNetworkReply *reply)
{
    // 에러 체크, readAll(), JSON 파싱
    QUrl url = reply->request().url(); //응답 구분
    QByteArray raw = reply->readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(raw, &err);
    if (err.error == QJsonParseError::NoError) {
        qDebug() << "--- NetWorkReader Parsing Success ---";
        if (doc.isObject()){
            if(url.toString().contains("clientdb"))
                emit jsonObjectReady(doc.object());
        }
        else if (doc.isArray()) {
            if(url.toString().contains("clientdb"))
                emit jsonArrayReady(doc.array());
            else if(url.toString().contains("announcedb"))
                emit jsonArrayAnnReady(doc.array());
        }

        //else                  emit parseError("Unknown JSON type");
    } else {
        qDebug() << "--- NetWorkReader Parsing FAIL!! ---";
        //emit parseError(err.errorString());
    }
    reply->deleteLater();
}
