#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "ui_mini3_browser.h"

#include <QObject>

class NetWorkRequester;
class NetWorkReader;
class ClientSearch;

class UIManager : public QObject
{
    Q_OBJECT
public:
    explicit UIManager(Ui::MainWindow *ui, NetWorkRequester *req, NetWorkReader *reader, QObject *parent);

signals:

private:
    ClientSearch *search;
};

#endif // UIMANAGER_H
