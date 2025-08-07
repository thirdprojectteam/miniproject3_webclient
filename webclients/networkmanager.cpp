#include "networkmanager.h"

#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>

QNetworkReply* NetWorkManager::get(const QUrl &url)
{
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return accessManager.get(req);
}

QNetworkReply *NetWorkManager::post(const QUrl &url, const QByteArray &data)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return accessManager.post(request,data);
}
