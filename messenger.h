#ifndef MESSENGER_H
#define MESSENGER_H

#include <QObject>
#include "maininclude.h"

class Messenger : public QObject
{
    Q_OBJECT
public:
    explicit Messenger(QObject *parent = nullptr);

    void finishOneOrder(int orderID);
    void finishResOrder(int orderID);
    void changeOneStatus(int orderID, QString toStatus);
    void newOrder(QJsonObject json);
    void changeOrder(QJsonObject json);
    void newResOrder(QJsonObject json);
    void changeResOrder(QJsonObject json);
    void changeUser(User afUser);

    ~Messenger();

signals:
    void gotResponse(QNetworkReply* reply);

private slots:
    void finishPost();

private:
    QNetworkAccessManager* manager;
    void postON(QJsonObject json,QString uri);
};

#endif // MESSENGER_H
