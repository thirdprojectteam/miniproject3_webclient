#ifndef NETWORKREQUESTER_H
#define NETWORKREQUESTER_H

#include <QObject>
#include <QNetworkReply>

class NetWorkRequester : public QObject
{
    Q_OBJECT
public:
    explicit NetWorkRequester(QObject *parent = nullptr);

    void RequestGet(const QUrl &url);

signals:
    void replyReady(QNetworkReply*);

private slots:
    void finished();
};

#endif // NETWORKREQUESTER_H
