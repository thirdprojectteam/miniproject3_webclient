#include "networkrequester.h"
#include "networkmanager.h"

#include <QUrl>

NetWorkRequester::NetWorkRequester(QObject *parent)
    : QObject{parent}
{}

void NetWorkRequester::RequestGet(const QUrl &url)
{
    // 1) 매니저에 GET 요청
    QNetworkReply* reply = NetWorkManager::instance().get(url);

    // 2) 응답 완료 시점에 finished() 호출되도록 연결
    connect(reply,&QNetworkReply::finished,this,&NetWorkRequester::finished);
}

void NetWorkRequester::RequestPost(const QUrl &url, const QByteArray &data)
{
    qDebug() << "--- RequestPost start --- ";
    QNetworkReply* reply = NetWorkManager::instance().post(url,data);
    if(url.toString().contains("accountdb"))
        connect(reply,&QNetworkReply::finished,this,&NetWorkRequester::popupFinishPost);
    else if(url.toString().contains("announcedb"))
        connect(reply,&QNetworkReply::finished,this,&NetWorkRequester::popupAnnLogFinishPost);
}

void NetWorkRequester::finished()
{
    qDebug() << "--- NetWorkRequester finished ---";
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        qDebug() << "--- NetWorkRequester emit replyReady ---";
        emit replyReady(reply);
    }
}

void NetWorkRequester::popupFinishPost()
{
    emit endPost();
}

void NetWorkRequester::popupAnnLogFinishPost()
{
    emit endAnnLog();
}
