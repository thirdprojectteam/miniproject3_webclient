#ifndef CLIENTSEARCH_H
#define CLIENTSEARCH_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidget>

class NetWorkReader;
class NetWorkRequester;
class QJsonArray;

class ClientSearch : public QWidget
{
    Q_OBJECT
public:
    explicit ClientSearch(NetWorkRequester *req, NetWorkReader *rea,QWidget* parent = nullptr);

    void init();
signals:

public slots:
    void viewClientList(QJsonArray JsonArray);
    void searchClientID(const QJsonObject& info);

private:
    NetWorkReader    *reader;
    NetWorkRequester *requester;
    QWidget          *page;
    QJsonArray       client;
    QTableWidget     *table;

    const QString CLIENTDB_URL = "http://127.0.0.1:8081/client/clientdb";
    const QString ACCOUNT_URL  = "http://127.0.0.1:8081/client/accountdb";

    int findClientIdByResident(const QJsonArray& client, const QString& targetResident);

private slots:
    void on_searchBtn1_clicked();
    void on_resetBtn1_clicked();
    void postToAccount();
    void refresh();
};

#endif // CLIENTSEARCH_H
