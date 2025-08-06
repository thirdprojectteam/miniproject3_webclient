#include "mainwindow.h"
#include "ui_mini3_browser.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->work_line->setVisible(false);
    ui->work_frame->setVisible(false);

    search = new ClientSearch(ui->page);
}

MainWindow::~MainWindow() {}
