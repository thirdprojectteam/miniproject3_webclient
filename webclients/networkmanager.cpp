#include "networkmanager.h"

#include <QNetworkRequest>

QNetworkReply* NetWorkManager::get(const QUrl &url)
{
    QNetworkRequest req(url);
    // 필요하다면 헤더 설정
    req.setHeader(QNetworkRequest::ContentTypeHeader,
                  "application/json");
    return accessManager.get(req);
}
