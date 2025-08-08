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
    void jsonArrayReady(QJsonArray jsonArray);      //clientdb
    void jsonObjectReady(QJsonObject jsonObject);   //clientdb
    void jsonArrayAnnReady(QJsonArray jsonArray);   //announcedb
    void latestObjectReady(QJsonObject jsonObject); //atmlogdb/latest
    void atmArrayReady(QJsonArray jsonArray);       //atmlogdb
    void annlogArrayReady(QJsonArray jsonArray);    //announcelogdb

public slots:
    void replyread(QNetworkReply*);
};

#endif // NETWORKREADER_H
