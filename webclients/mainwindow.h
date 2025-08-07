#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class NetWorkReader;
class NetWorkRequester;
class ClientSearch;
class Notice;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow   *ui;
    NetWorkRequester *request;
    NetWorkReader    *reader;
    ClientSearch     *search;
    Notice           *notice;

    //const QString CLIENTDB_URL = "http://127.0.0.1:8081/client/clientdb";

private slots:
    void PageChanged(int index);
    void WorkLabelClicked();
    void AnnouceLabelClicked();

};
#endif // MAINWINDOW_H
