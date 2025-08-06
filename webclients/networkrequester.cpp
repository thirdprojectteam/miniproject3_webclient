#include "networkrequester.h"
#include "networkmanager.h"

#include <QUrl>
NetWorkManager::~NetWorkManager() = default;
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

void NetWorkRequester::finished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        emit replyReady(reply);
    }
}
