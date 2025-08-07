#ifndef NOTICE_H
#define NOTICE_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QFrame>
#include <QPushButton>
#include <QDate>
#include <QNetworkInterface>
#include <QHostAddress>
class NetWorkReader;
class NetWorkRequester;
class QJsonArray;

class Notice : public QWidget
{
    Q_OBJECT
public:
    explicit Notice(NetWorkRequester *req, NetWorkReader *rea,QWidget* parent = nullptr);
    void init();

signals:
    // 클릭된 제목만 외부로 방출
    void titleSelected(const QString& title);

public slots:
    void viewClientList(QJsonArray JsonArray);
    void onTitleSelected(const QString& title);
    void ReturnBoard();
    void onCreatePage();
    void PostBtn();
    void refresh();
private:
    NetWorkReader    *reader;
    NetWorkRequester *requester;
    QWidget          *page;
    QJsonArray       board;
    QFrame           *detail;
    QFrame           *create;
    QPushButton      *createBtn;
    QPushButton      *returnBtn;
    QPushButton      *postBtn;
    QFrame           *noticeframe;
    QDate            today;
    QString          localIP;

    const QString ANNDB_URL    = "http://127.0.0.1:8081/client/announcedb";
    const QString ANNLOGDB_URL = "http://127.0.0.1:8081/client/announcelogdb";

    void postToLog(const QString& title, QJsonObject select);
};
#endif // NOTICE_H
