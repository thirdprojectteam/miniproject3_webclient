#include <QUrl>

#include "clientsearch.h"
#include "networkreader.h"
#include "networkrequester.h"
ClientSearch::ClientSearch(QWidget *page,QObject *parent)
    : QObject{parent}, page_(page), req_(new NetWorkRequester(this))
{
    NetWorkReader* reader = new NetWorkReader(this);
    connect(reader,&NetWorkReader::jsonArrayReady,this,&ClientSearch::viewClientList);
}

void ClientSearch::on_search_btn_clicked()
{
    // 요청할 URL 생성 (웹서버의 /api/users 경로)
    qDebug() << "눌림";
    QUrl url(CLIENTDB_URL);
    NetWorkRequester* requester = new NetWorkRequester(this);
    requester->RequestGet(url);
}

void ClientSearch::viewClientList(QJsonArray JsonArray)
{
    qDebug() <<JsonArray.toVariantList();
}
