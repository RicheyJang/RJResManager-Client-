#include "messenger.h"

Messenger::Messenger(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager();
}

Messenger::~Messenger()
{
    manager->deleteLater();
}

void Messenger::postON(QJsonObject json, QString uri)
{
    QJsonDocument document;
    document.setObject(json);

    QByteArray data = document.toJson(QJsonDocument::Indented);

    QNetworkRequest* request = new QNetworkRequest;

    request->setHeader(QNetworkRequest::UserAgentHeader, config.UserAgent);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/json"); //上面语句固定这么写，要不然会报错“contest—type is missing”

    QString url = QString("http://") + config.ip + ':' + QString::number(config.serverPort) + uri;
    request->setUrl(QUrl(url));

    QNetworkReply* reply=manager->post(*request, data);
    connect(reply,&QNetworkReply::finished,this,&Messenger::finishPost);
    delete request;
    request = nullptr;
}

void Messenger::changeOneStatus(int orderID, QString toStatus)
{
    QJsonObject json;
    QJsonObject orderInf;
    QJsonObject userInf;
    userInf.insert("username", thisUser.username);
    userInf.insert("password", thisUser.password);

    QJsonArray theOrders; //订单序列
    QJsonObject aOrder; //单个订单信息
    aOrder.insert("id",QJsonValue(orderID)); //填充单个订单的ID
    theOrders.append(QJsonValue(aOrder)); //将该订单添加到订单序列

    orderInf.insert("theOrders",QJsonValue(theOrders)); //将订单序列添加到订单信息
    orderInf.insert("isSame",QJsonValue(true)); //所有订单所要转换到的状态相同
    orderInf.insert("status", QJsonValue(toStatus)); //同一状态

    json.insert("userInformation", QJsonValue(userInf));
    json.insert("orderInformation", QJsonValue(orderInf));

    postON(json, QString("/changestatus")); //uri
}

void Messenger::finishOneOrder(int orderID)
{
    QJsonObject json;
    QJsonObject orderInf;
    QJsonObject userInf;
    userInf.insert("username", thisUser.username);
    userInf.insert("password", thisUser.password);

    orderInf.insert("id", QJsonValue(orderID));

    json.insert("userInformation", QJsonValue(userInf));
    json.insert("orderInformation", QJsonValue(orderInf));

    postON(json, QString("/finishorder")); //uri
}

void Messenger::newOrder(QJsonObject json)
{
    postON(json, QString("/new/forOrder"));
}
void Messenger::changeOrder(QJsonObject json)
{
    postON(json, QString("/changeorder"));
}

//槽函数：
void Messenger::finishPost()
{
    QNetworkReply* reply=qobject_cast<QNetworkReply *>(sender());
    emit gotResponse(reply);
}
