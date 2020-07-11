#ifndef MESSENGER_H
#define MESSENGER_H

#include <QObject>
#include "maininclude.h"

class Messenger : public QObject
{
    Q_OBJECT
public:
    explicit Messenger(QObject *parent = nullptr);
    void postON(QJsonObject json,QString uri);
    ~Messenger();

signals:
    void gotResponse(QNetworkReply* reply);

private slots:
    void finishPost();

private:
    QNetworkAccessManager* manager;
};

#endif // MESSENGER_H
