#include "managepage.h"
#include "networkrequester.h"
#include "networkreader.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QHostAddress>
#include <QNetworkInterface>

ManagePage::ManagePage(NetWorkRequester *req, NetWorkReader *rea, QWidget* parent)
    :  reader(rea), requester(req),page(parent),socket(new QWebSocket()),YrangeM(20)
{
    barBox  = page->findChild<QVBoxLayout*>("barlayout");
    lineBox = page->findChild<QVBoxLayout*>("linelayout");
    lineTimer = new QTimer(this);
    lineTimer->setSingleShot(false);
    lineTimer->setInterval(intervalSec * 1000);

    // ----- BarChart -----
    barSeries = new QBarSeries;
    QStringList categories = { "게시판1", "게시판2", "게시판3", "게시판4" };

    axisXBar = new QBarCategoryAxis;
    axisXBar->append(categories);

    axisYBar = new QValueAxis;
    axisYBar->setRange(0, YrangeM);     // 조회수 범위
    axisYBar->setLabelFormat("%.0f");   // ✅ 정수로 표시 (소수점 제거)
    axisYBar->setVisible(true);         // ✅ Y축 보이게
    // ✅ Bar 색상 검정
    QBarSet *barSet = new QBarSet("조회수");
    barSet->setColor(Qt::black);
    barSeries->append(barSet);
    barSeries->setBarWidth(0.1);

    // ✅ Grid 숨기기
    axisXBar->setGridLineVisible(false);
    axisYBar->setGridLineVisible(false);

    barChart = new QChart;
    barChart->addSeries(barSeries);
    barChart->addAxis(axisXBar, Qt::AlignBottom);
    barChart->addAxis(axisYBar, Qt::AlignLeft);
    barSeries->attachAxis(axisXBar);
    barSeries->attachAxis(axisYBar);
    barChart->legend()->setVisible(true);

    barChartView = new QChartView;
    barChartView->setChart(barChart);
    barBox->addWidget(barChartView);

    // ----- LineChart -----
    lineSeries = new QLineSeries;

    axisXLine = new QDateTimeAxis();
    axisXLine->setFormat("HH:mm:ss");
    axisXLine->setTickCount(6);
    axisXLine->setGridLineVisible(false);
    axisXLine->setMinorGridLineVisible(false);
    axisXLine->setLineVisible(false);   // 라벨만 보이게

    axisYLine = new QValueAxis();
    axisYLine->setRange(0, 1);
    axisYLine->setVisible(false);       // ✅ Y축 안 보이게

    lineChart = new QChart();
    lineChart->addSeries(lineSeries);
    lineChart->addAxis(axisXLine, Qt::AlignBottom);
    lineChart->addAxis(axisYLine, Qt::AlignLeft);
    lineSeries->attachAxis(axisXLine);
    lineSeries->attachAxis(axisYLine);

    // 배경/그리드 최소화(선 다 지우고 깔끔하게)
    lineChart->legend()->hide();
    lineChart->setBackgroundVisible(false);
    lineChart->setPlotAreaBackgroundVisible(false);
    lineChart->setDropShadowEnabled(false);
    lineChart->setMargins(QMargins(0,0,0,0));

    lineChartView = new QChartView(lineChart);
    lineBox->addWidget(lineChartView);

    //채팅
    textEdit = page->findChild<QPlainTextEdit*>("chatbox");
    lineEdit = page->findChild<QLineEdit*>("chatline");
    sendBtn  = page->findChild<QPushButton*>("chatSendBtn");
    textEdit->setReadOnly(true);
    for (const QHostAddress &addr : QNetworkInterface::allAddresses()) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol
            && addr != QHostAddress(QHostAddress::LocalHost)) {
            nickname = addr.toString();
            break;
        }
    }
    //채팅 시그널 슬롯
    socket->open(QUrl("ws://192.168.2.18:9000"));  // 서버 주소/포트 설정
    connect(sendBtn,&QPushButton::clicked, this, &ManagePage::sendMesg);
    connect(socket, &QWebSocket::connected, this, &ManagePage::onConnected);
    connect(socket, &QWebSocket::textMessageReceived, this, &ManagePage::onTextMessageReceived);
    connect(lineEdit, &QLineEdit::returnPressed, this, &ManagePage::sendEnterMesg);
    //채팅 아닌거 슬롯
    connect(reader, &NetWorkReader::atmArrayReady, this, &ManagePage::onLineChartReply, Qt::UniqueConnection);
    connect(reader, &NetWorkReader::annlogArrayReady, this, &ManagePage::onBarChartReply, Qt::UniqueConnection);
    connect(reader, &NetWorkReader::latestObjectReady, this, &ManagePage::updateLineChart, Qt::UniqueConnection);
    connect(this, &ManagePage::finishInit, this, &ManagePage::startLineChart, Qt::UniqueConnection);
    connect(lineTimer, &QTimer::timeout, this, &ManagePage::fetchRealLine, Qt::UniqueConnection);
}

void ManagePage::init()
{
    fetchBarChart();
    fetchLineChart();
}

void ManagePage::setActive(bool on) {
    isActivePage = on;
    if (!lineTimer) return;
    if (on) {
        if (!timerArmed) {
            // 활성화되면 타이머 시작
            lineTimer->stop();
            lineTimer->setInterval(intervalSec * 1000);
            lineTimer->start();
            timerArmed = true;
            qDebug() << "[ManagePage] timer START (active)";
        }
    } else {
        if (lineTimer->isActive()) lineTimer->stop();
        timerArmed = false;
        qDebug() << "[ManagePage] timer STOP (inactive)";
    }
}

void ManagePage::updateLineChart(QJsonObject obj)
{
    qDebug() << "--- updateLineChart start --- ";
    if (!isActivePage) return;
    // 값 파싱 (키 이름 맞춰 수정)
    auto v = obj.value("value").toInt();
    // 시간: 현재 시각을 5초 그리드로 맞추고 싶으면 아래처럼 반올림
    QDateTime now = QDateTime::currentDateTime();
    qint64 ms = now.toMSecsSinceEpoch();
    qint64 step = intervalSec * 1000;
    ms = (ms / step) * step;            // 그리드 정렬
    now = QDateTime::fromMSecsSinceEpoch(ms);

    // 10개 유지
    if (lineSeries->count() >= maxPoints)
        lineSeries->removePoints(0, 1);

    // 점 추가
    lineSeries->append(now.toMSecsSinceEpoch(), v);
    lineSeries->setPointsVisible(true);

    // X축: 현재를 오른쪽 끝, 5초*9 만큼 왼쪽으로
    const QDateTime end   = now;
    const QDateTime start = end.addSecs(-(maxPoints - 1) * intervalSec);
    axisXLine->setMin(start);
    axisXLine->setMax(end);

    // Y축 자동 스케일(원하면 유지/삭제)
    double minV = v, maxV = v;
    for (const QPointF& p : lineSeries->pointsVector()) {
        minV = std::min(minV, p.y());
        maxV = std::max(maxV, p.y());
    }
    if (minV == maxV) { minV -= 1.0; maxV += 1.0; }
    const double pad = (maxV - minV) * 0.1;
    axisYLine->setRange(minV - pad, maxV + pad);
    axisYLine->setVisible(false);
}

void ManagePage::onLineChartReply(QJsonArray JsonArray)
{
    qDebug() << "--- onLineChartReply start --- ";
    // 최신 시간(지금)을 우측 끝으로
    const QDateTime end = QDateTime::currentDateTime();
    const int n = qMin(wantCount, JsonArray.size());

    // n개만 그릴 수 있도록 시작 시간 계산
    const QDateTime start = end.addSecs(-(n - 1) * intervalSec);

    // 축 범위 업데이트
    axisXLine->setMin(start);
    axisXLine->setMax(end);

    // 시리즈 리셋
    lineSeries->clear();
    lineSeries->setPointsVisible(true);   // 점 표시

    // 서버에서 온 데이터가 id DESC(최신 먼저)라면, 오래된 것부터 사용하도록 뒤집어서 매핑
    // 값 추출 키 이름은 실제 JSON 키에 맞게 수정하세요(e.g., "value")
    QVector<double> values;
    values.reserve(n);
    for (int i = JsonArray.size() - 1; i >= JsonArray.size() - n; --i) {
        const QJsonObject obj = JsonArray[i].toObject();
        const double v = obj.value("value").toDouble();  // <-- 키 이름 확인!
        values.push_back(v);
    }

    // 시간축에 10초(또는 5초) 간격으로 찍기
    for (int i = 0; i < n; ++i) {
        const QDateTime t = start.addSecs(i * intervalSec);
        lineSeries->append(t.toMSecsSinceEpoch(), values[i]);
    }

    // 필요하면 Y축을 데이터에 맞춰 자동 스케일
    // (현재는 0~1로 고정이니 쓰지 않으려면 아래 블록 제거)
    {
        auto [minIt, maxIt] = std::minmax_element(values.begin(), values.end());
        double minV = *minIt, maxV = *maxIt;
        if (minV == maxV) { minV -= 1.0; maxV += 1.0; }
        const double pad = (maxV - minV) * 0.1;
        axisYLine->setRange(minV - pad, maxV + pad);
        axisYLine->setVisible(false); // 계속 숨길 거면 유지
    }
    emit finishInit();
}

void ManagePage::onBarChartReply(QJsonArray JsonArray)
{
    qDebug() << "--- onBarChartReply start --- ";
    // 기존 데이터만 삭제 (스타일/폭 유지)
    for (QBarSet* set : barSeries->barSets()) {
        set->remove(0, set->count()); // 모든 값 제거
    }
    axisXBar->clear();
    if (JsonArray.isEmpty()) {
        barChart->setTitle("No data");
        axisYBar->setRange(0, 1);
        return;
    }
    // 데이터 채우기
    QStringList categories;
    int maxVal = 0;
    QBarSet* set = nullptr;
    // 기존 BarSet 재사용
    set = barSeries->barSets().first();
    for (const QJsonValue& v : JsonArray) {
        if (!v.isObject()) continue;
        const auto o = v.toObject();
        QString title = o.value("title").toString();
        int count = o.value("record_count").toInt();

        *set << count;
        categories << title;
        maxVal = std::max(maxVal, count);
    }
    axisXBar->append(categories);
    axisYBar->setRange(0, std::max(1, maxVal + 1));
    barChart->setTitle("공지사항 조회수");
}

void ManagePage::startLineChart()
{
    qDebug() << "--- ManagePage TIMER start ---";
    if (!isActivePage) {
        qDebug() << "[ManagePage] startLineChart skipped (inactive)";
        return;
    }
    if (!lineTimer) return;
    if (timerArmed) {
        qDebug() << "[ManagePage] startLineChart skipped (already armed)";
        return;
    }

    qDebug() << "--- ManagePage TIMER start ---";
    lineTimer->stop();
    lineTimer->setInterval(intervalSec * 1000);
    lineTimer->start();
    timerArmed = true;
}

void ManagePage::fetchBarChart()
{
    qDebug() << "--- ManagePage fetch Bar Chart start ---";
    QUrl url(ANNLOGDB_URL);
    requester->RequestGet(url);
}

void ManagePage::fetchLineChart()
{
    qDebug() << "--- ManagePage fetch Line Chart start ---";
    QUrl url(ATMLOGDB_URL);
    requester->RequestGet(url);
}

void ManagePage::fetchRealLine()
{
    if (!isActivePage) {
        qDebug() << "[ManagePage] ignore onLineChartReply (inactive)";
        return;
    }
    qDebug() << "--- ManagePage REAL TIME fetch Line Chart start ---";
    QUrl url(ATMREAL_URL);
    requester->RequestGet(url);
    if (!timerArmed)
        emit finishInit();
}

void ManagePage::onConnected()
{
    // 연결되면 닉네임 먼저 전송
    socket->sendTextMessage(nickname);
    textEdit->appendPlainText("[System] 서버에 연결되었습니다.");
}

void ManagePage::sendMesg()
{
    sendMessage();
}

void ManagePage::sendEnterMesg()
{
    sendMessage();
}

void ManagePage::sendMessage()
{
    QString msg = lineEdit->text().trimmed();
    if (!msg.isEmpty()) {
        socket->sendTextMessage(msg);
        lineEdit->clear();
    }
}

void ManagePage::onTextMessageReceived(const QString &message)
{
    textEdit->appendPlainText(message);
}
