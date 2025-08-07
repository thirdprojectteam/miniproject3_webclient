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
    void RequestPost(const QUrl &url, const QByteArray &data);

signals:
    void replyReady(QNetworkReply*);
    void endPost();
    void endAnnLog();

public slots:
    void finished();
    void popupFinishPost();
    void popupAnnLogFinishPost();
};

#endif // NETWORKREQUESTER_H
