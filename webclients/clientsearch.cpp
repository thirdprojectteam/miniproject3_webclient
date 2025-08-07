#include "clientsearch.h"
#include "networkreader.h"
#include "networkrequester.h"

#include <QPushButton>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTableWidget>
#include <QHash>
#include <QLineEdit>
#include <QDate>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QUrl>
#include <QMessageBox>

ClientSearch::ClientSearch(NetWorkRequester *req, NetWorkReader *rea, QWidget* parent)
    :  reader(rea), requester(req),page(parent)
{
    QPushButton* searchBtn = page->findChild<QPushButton*>("searchBtn1");
    QPushButton* resetBtn  = page->findChild<QPushButton*>("resetBtn1");
    QPushButton* createBtn = page->findChild<QPushButton*>("createAccount");
    connect(reader, &NetWorkReader::jsonArrayReady, this, &ClientSearch::viewClientList);
    connect(searchBtn, &QPushButton::clicked, this, &ClientSearch::on_searchBtn1_clicked);
    connect(resetBtn, &QPushButton::clicked, this, &ClientSearch::on_resetBtn1_clicked);
    connect(createBtn, &QPushButton::clicked, this, &ClientSearch::postToAccount);
    connect(requester, &NetWorkRequester::endPost, this, &ClientSearch::refresh);
}

void ClientSearch::init()
{
    qDebug() << "--- ClientSearch init start ---";
    QUrl url(CLIENTDB_URL);
    requester->RequestGet(url);
}

void ClientSearch::viewClientList(QJsonArray JsonArray)
{
    qDebug() << "--- ClientSearch Receive Data ---";
    table = page->findChild<QTableWidget*>("tableWidget");
    client = JsonArray;
    //행 개수 설정
    int rowCount = client.size();
    table->setRowCount(rowCount);
    table->clearContents();
    // 실제로 채울 키와, 그 키가 매핑된 컬럼 인덱스 정의
    //  (예: 0열=RFID_name, 1열=hasAccount, 2열=client_resident)
    QHash<QString,int> keyToCol;
    keyToCol["RFID_name"]       = 0;
    keyToCol["hasAccount"]      = 1;
    keyToCol["client_resident"] = 2;

    for (int row = 0; row < rowCount; ++row) {
        QJsonObject obj = client.at(row).toObject();

        for (auto it = keyToCol.constBegin(); it != keyToCol.constEnd(); ++it) {
            const QString& key = it.key();
            int col = it.value();
            QString text;
            if (key == "hasAccount") {
                int flag = obj.value(key).toInt();
                text = (flag == 0 ? QStringLiteral("X")
                                  : QStringLiteral("O"));
            }
            else {
                text = obj.value(key).toString();
            }
            // 아이템 생성 또는 업데이트
            QTableWidgetItem* item = table->item(row, col);
            if (!item) {
                item = new QTableWidgetItem(text);
                table->setItem(row, col, item);
            } else {
                item->setText(text);
            }
            item->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void ClientSearch::searchClientID(const QJsonObject &info)
{

}

void ClientSearch::on_searchBtn1_clicked()
{
    qDebug() << "고객 검색 버튼 눌렸다";

    int rows    = table->rowCount();
    int columns = table->columnCount();
    QLineEdit *searchText  = page->findChild<QLineEdit*>("lineEdit");
    for (int row = 0; row < rows; ++row) {
        bool match = searchText->text().isEmpty();

        if (!match) {
            // 모든 컬럼을 돌면서 검색어 포함 여부 확인
            for (int col = 0; col < columns; ++col) {
                auto item = table->item(row, col);
                if (item && item->text().contains(searchText->text(), Qt::CaseInsensitive)) {
                    match = true;
                    break;
                }
            }
        }
        table->setRowHidden(row, !match);
    }

}

void ClientSearch::on_resetBtn1_clicked()
{
    // 1) 검색창 비우기
    if (auto edit = page->findChild<QLineEdit*>("lineEdit")) {
        edit->clear();
    }
    // 2) 모든 행 보이기
    if (auto table = page->findChild<QTableWidget*>("tableWidget")) {
        for (int row = 0; row < table->rowCount(); ++row) {
            table->setRowHidden(row, false);
        }
    }
}

int ClientSearch::findClientIdByResident(const QJsonArray& client, const QString& targetResident) {
    for (const QJsonValue& val : client) {
        qDebug() << val;
        if (!val.isObject())
            continue;
        const QJsonObject obj = val.toObject();
        if (obj.value("client_resident").toString() == targetResident) {
            return obj.value("id").toInt(-1);
        }
    }
    return -1;
}

void ClientSearch::postToAccount()
{
    QLineEdit *account = page->findChild<QLineEdit*>("accountNum");
    QLineEdit *resident = page->findChild<QLineEdit*>("resident");

    int id = findClientIdByResident(client, resident->text());
    QDate today = QDate::currentDate();
    QString localIP;
    for (const QHostAddress &addr : QNetworkInterface::allAddresses()) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol
            && addr != QHostAddress(QHostAddress::LocalHost)) {
            localIP = addr.toString();
            break;
        }
    }

    QUrl url(ACCOUNT_URL);
    QJsonObject obj;
    QJsonObject members;

    members["RFID_UUID"]      = account->text();
    members["client_id"]      = id;
    members["balance"]        = 1000000;
    members["account_pass"]   = "7456";
    members["account_start"]  = today.toString(Qt::ISODate);
    members["account_expire"] = today.addYears(5).toString(Qt::ISODate);
    members["account_man"]    = localIP;

    obj["data"] = members;
    QJsonDocument doc(obj);
    QByteArray postData = doc.toJson(QJsonDocument::Compact);
    requester->RequestPost(url,postData);
}

void ClientSearch::refresh()
{
    QMessageBox::information(nullptr,tr("개설 완료"), tr("계좌 개설 완료 됐습니다"));
    init();
}
