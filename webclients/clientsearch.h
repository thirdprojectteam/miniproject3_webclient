#ifndef CLIENTSEARCH_H
#define CLIENTSEARCH_H

#include <QObject>

#include "networkrequester.h"
#include <QJsonArray>
#include <QJsonObject>

class ClientSearch : public QObject
{
    Q_OBJECT
public:
    explicit ClientSearch(QWidget *page,QObject *parent = nullptr);

signals:

private slots:
    void on_search_btn_clicked();
    void viewClientList(QJsonArray JsonArray);

private:
    QWidget            *page_;
    NetWorkRequester   *req_;
    const QString CLIENTDB_URL = "http://127.0.0.1:8081/client/clientdb";
};

#endif // CLIENTSEARCH_H
