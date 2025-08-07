#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkReply>

class NetWorkManager : public QObject
{
    Q_OBJECT
public:
    static NetWorkManager& instance(){
        static NetWorkManager inst;
        return inst;
    }
    QNetworkReply* get(const QUrl &url);
    QNetworkReply* post(const QUrl &url, const QByteArray &data);

signals:

private:
    NetWorkManager() = default;
    ~NetWorkManager() = default;
    NetWorkManager(const NetWorkManager&) = delete;
    NetWorkManager& operator=(const NetWorkManager&) = delete;
    QNetworkAccessManager accessManager;
};

#endif // NETWORKMANAGER_H
