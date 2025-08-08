#ifndef MANAGEPAGE_H
#define MANAGEPAGE_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QtCharts>
#include <QTimer>
#include <QNetworkReply>
#include <QWebSocket>
#include <QDateTime>
#include <QVBoxLayout>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QWebSocket>
#include <QPlainTextEdit>

class NetWorkReader;
class NetWorkRequester;
class QJsonArray;

class ManagePage : public QWidget
{
    Q_OBJECT
public:
    explicit ManagePage(NetWorkRequester *req, NetWorkReader *rea, QWidget* parent = nullptr);
    void init();

signals:
    void finishInit();

private slots:
    void updateLineChart(QJsonObject obj);
    void onLineChartReply(QJsonArray JsonArray);
    void onBarChartReply(QJsonArray JsonArray);
    void startLineChart();

    //채팅
    void onConnected();
    void onTextMessageReceived(const QString &message);
    void sendMesg();
    void sendMessage();
    void sendEnterMesg();

public slots:
    void setActive(bool on);

private:
    NetWorkReader    *reader;
    NetWorkRequester *requester;
    QWidget          *page;
    QJsonArray       client;

    // BarChart
    QChartView       *barChartView;
    QBarSeries       *barSeries;
    QVBoxLayout      *barBox;
    QBarCategoryAxis *axisXBar;
    QValueAxis       *axisYBar;
    QChart           *barChart;
    void fetchBarChart();

    // LineChart
    QChartView    *lineChartView;
    QLineSeries   *lineSeries;
    QDateTime     startTime;
    QVBoxLayout   *lineBox;
    QDateTimeAxis *axisXLine;
    QValueAxis    *axisYLine;
    QChart        *lineChart;
    QTimer        *lineTimer;
    bool          isActivePage = false; //페이지가 활성화 되어 있는지
    bool          timerArmed = false;  // 타이머가 이미 start 되었는지
    void fetchLineChart();
    void fetchRealLine();

    // 채팅
    QWebSocket      *socket;
    QString         nickname;
    QPlainTextEdit  *textEdit;
    QLineEdit       *lineEdit;
    QPushButton     *sendBtn;

    const QString ANNLOGDB_URL = "http://127.0.0.1:8081/client/announcelogdb";
    const QString ATMLOGDB_URL = "http://127.0.0.1:8081/client/atmlogdb";
    const QString ATMREAL_URL  = "http://127.0.0.1:8081/client/atmlogdb/latest";

    const int intervalSec = 5;
    const int wantCount   = 10;
    const int YrangeM;
    const int maxPoints   = 10;

};

#endif // MANAGEPAGE_H
