#include "uimanager.h"
#include "networkrequester.h"
#include "networkreader.h"
#include "clientsearch.h"

UIManager::UIManager(Ui::MainWindow *ui, NetWorkRequester *req, NetWorkReader *reader, QObject *parent)
    : QObject{parent}
{

}
