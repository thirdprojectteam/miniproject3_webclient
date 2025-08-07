#include "mainwindow.h"
#include "ui_mini3_browser.h"
#include "networkrequester.h"
#include "networkreader.h"
#include "clientsearch.h"
#include "clickablelabel.h"
#include "notice.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->work_line->setVisible(true);
    ui->work_line_2->setVisible(false);
    ui->work_line_3->setVisible(false);

    request = new NetWorkRequester(this);
    reader  = new NetWorkReader(this);
    search  = new ClientSearch(request, reader,ui->page);
    notice  = new Notice(request,reader,ui->page_2);

    // 초기 표시가 0번이면 즉시 한 번 실행
    ui->stackedWidget->setCurrentIndex(0);
    if (ui->stackedWidget->currentIndex() == 0) {
        PageChanged(0);
    }
    auto workLabel     = qobject_cast<ClickableLabel*>(ui->work);
    auto announceLabel = qobject_cast<ClickableLabel*>(ui->announcement);
    connect(request, &NetWorkRequester::replyReady, reader, &NetWorkReader::replyread);
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, &MainWindow::PageChanged);
    connect(workLabel, &ClickableLabel::clicked, this, &MainWindow::WorkLabelClicked);
    connect(announceLabel, &ClickableLabel::clicked, this, &MainWindow::AnnouceLabelClicked);
}

MainWindow::~MainWindow() {}

void MainWindow::PageChanged(int index)
{
    switch (index) {
    case 0:
        search->init();
        break;
    case 1:
        notice->init();
        break;
    default:
        break;
    }
}

void MainWindow::WorkLabelClicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->work_line->setVisible(true);
    ui->work_line_2->setVisible(false);
    ui->work_line_3->setVisible(false);
}

void MainWindow::AnnouceLabelClicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->work_line->setVisible(false);
    ui->work_line_2->setVisible(true);
    ui->work_line_3->setVisible(false);
}


