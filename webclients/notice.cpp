#include "notice.h"
#include "clientsearch.h"
#include "networkreader.h"
#include "networkrequester.h"
#include "clickablelabel.h"

#include <QPushButton>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QLabel>
#include <QUrl>
#include <QTextEdit>
#include <QMessageBox>

Notice::Notice(NetWorkRequester *req, NetWorkReader *rea, QWidget* parent)
    :  reader(rea), requester(req),page(parent)
{
    detail    = page->findChild<QFrame*>("detail");
    create    = page->findChild<QFrame*>("createnotice");
    createBtn = page->findChild<QPushButton*>("createAlertBtn");
    returnBtn = page->findChild<QPushButton*>("detail_return");
    postBtn   = page->findChild<QPushButton*>("postBtn");

    for (const QHostAddress &addr : QNetworkInterface::allAddresses()) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol
            && addr != QHostAddress(QHostAddress::LocalHost)) {
            localIP = addr.toString();
            break;
        }
    }

    // title1~title4 라벨 클릭 연결
    for (int i = 1; i <= 4; ++i) {
        auto name = QStringLiteral("title%1").arg(i);
        auto titles  = page->findChild<ClickableLabel*>(name);
        if (!titles) continue;
        // 시각적으로 클릭 가능함을 표시
        titles->setCursor(Qt::PointingHandCursor);
        connect(titles, &ClickableLabel::clicked, this, [this, titles](){
            qDebug() << "clicked title : " << titles;
            emit titleSelected(titles->text());
        });
    }
    detail->setVisible(false);
    create->setVisible(false);
    returnBtn->setVisible(false);
    connect(reader, &NetWorkReader::jsonArrayAnnReady, this, &Notice::viewClientList);
    connect(this,&Notice::titleSelected, this, &Notice::onTitleSelected);
    connect(returnBtn, &QPushButton::clicked, this, &Notice::ReturnBoard);
    connect(createBtn, &QPushButton::clicked, this, &Notice::onCreatePage);
    connect(postBtn, &QPushButton::clicked, this, &Notice::PostBtn);
    connect(requester, &NetWorkRequester::endAnnLog, this, &Notice::refresh);
}

void Notice::init()
{
    qDebug() << "--- Notice init start ---";
    QUrl url(ANNDB_URL);
    requester->RequestGet(url);
}

void Notice::viewClientList(QJsonArray JsonArray)
{
    qDebug() << "--- Notice Receive Data ---";
    QLabel* noContentLabel = page->findChild<QLabel*>("noContent");
    noticeframe            = page->findChild<QFrame*>("noticeframe");
    board = JsonArray;
    if (board.isEmpty()) {
        // 데이터 없을 때
        noContentLabel->setVisible(true);
        noticeframe   ->setVisible(false);
    }
    else {
        noContentLabel->setVisible(false);
        noticeframe   ->setVisible(true);
        const int maxItems = 4;
        for (int i = 0; i < maxItems; ++i) {
            // 라벨 찾기
            QLabel* idLabel     = page->findChild<QLabel*>(QString("id%1").arg(i+1));
            QLabel* titleLabel  = page->findChild<QLabel*>(QString("title%1").arg(i+1));
            QLabel* writerLabel = page->findChild<QLabel*>(QString("writer%1").arg(i+1));
            QLabel* dateLabel   = page->findChild<QLabel*>(QString("date%1").arg(i+1));
            // 찾았다면
            if (!titleLabel || !writerLabel || !dateLabel)
                continue;
            if (i < board.size()) {
                // i번째 데이터가 있으면 표시
                QJsonObject obj = board.at(i).toObject();
                int id = obj.value("id").toInt();
                QString idStr = QString::number(id);
                idLabel->setText(idStr);
                titleLabel->setText(obj.value("title").toString());
                writerLabel->setText(obj.value("writer").toString());
                dateLabel-> setText(obj.value("times").toString());

                titleLabel->setVisible(true);
                writerLabel->setVisible(true);
                dateLabel->setVisible(true);
            } else {
                // 데이터가 없으면 숨김
                titleLabel->setVisible(false);
                writerLabel->setVisible(false);
                dateLabel-> setVisible(false);
            }
        }
    }
}

void Notice::onTitleSelected(const QString &title)
{
    noticeframe->setVisible(false);
    detail->setVisible(true);
    create->setVisible(false);
    createBtn->setVisible(false);
    returnBtn->setVisible(true);
    QJsonObject selected;
    for (const QJsonValue& v : board) {
        if (!v.isObject()) continue;
        QJsonObject obj = v.toObject();
        if (obj.value("title").toString() == title) {
            selected = obj;
            qDebug() << "Selected entry:" << obj;
            QLabel* titleLabel   = page->findChild<QLabel*>("detail_title");
            QLabel* writerLabel  = page->findChild<QLabel*>("detail_writer");
            QLabel* dateLabel    = page->findChild<QLabel*>("detail_date");
            QLabel* contentLabel = page->findChild<QLabel*>("detail_content");

            titleLabel->setText(obj.value("title").toString());
            writerLabel->setText(obj.value("writer").toString());
            dateLabel->setText(obj.value("times").toString());
            contentLabel->setText(obj.value("content").toString());
        }
    }
    postToLog(title,selected);
}

void Notice::ReturnBoard()
{
    noticeframe->setVisible(true);
    detail->setVisible(false);
    create->setVisible(false);
    createBtn->setVisible(true);
    returnBtn->setVisible(false);
}

void Notice::onCreatePage()
{
    noticeframe->setVisible(false);
    detail->setVisible(false);
    create->setVisible(true);
    createBtn->setVisible(false);
    returnBtn->setVisible(true);
}

void Notice::PostBtn()
{
    qDebug() << "--- Notice PostBtn start --- ";
    QLineEdit* titleLabel  = page->findChild<QLineEdit*>("create_title");
    QTextEdit* content  = page->findChild<QTextEdit*>("create_content");
    QUrl url(ANNDB_URL);
    QJsonObject obj;
    QJsonObject members;
    today = QDate::currentDate();
    members["writer"]  = localIP;
    members["title"]   = titleLabel->text();
    members["content"] = content->toPlainText();
    members["times"]   = today.toString(Qt::ISODate);
    obj["data"]        = members;
    QJsonDocument doc(obj);
    QByteArray postData = doc.toJson(QJsonDocument::Compact);
    requester->RequestPost(url,postData);
}

void Notice::refresh()
{
    QMessageBox::information(nullptr,tr("업데이트"), tr("업데이트 됐습니다"));
    ReturnBoard();
    init();
}

void Notice::postToLog(const QString& title,QJsonObject select)
{
    qDebug() << "--- Notice postToLog start --- ";
    QUrl url(ANNLOGDB_URL);
    QJsonObject obj;
    QJsonObject members;
    today = QDate::currentDate();
    int announce_id = select.value("id").toInt();
    members["announce_id"]  = announce_id;
    members["checker"]      = localIP;
    members["check_time"]   = today.toString(Qt::ISODate);
    obj["data"]             = members;
    QJsonDocument doc(obj);
    QByteArray postData = doc.toJson(QJsonDocument::Compact);
    requester->RequestPost(url,postData);
}
