#ifndef NETWORKREADER_H
#define NETWORKREADER_H

#include <QObject>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
class NetWorkReader : public QObject
{
    Q_OBJECT
public:
    explicit NetWorkReader(QObject *parent = nullptr);

signals:
    void jsonArrayReady(QJsonArray jsonArray);
    void jsonObjectReady(QJsonObject jsonObject);
    void jsonArrayAnnReady(QJsonArray jsonArray);

public slots:
    void replyread(QNetworkReply*);
};

#endif // NETWORKREADER_H
