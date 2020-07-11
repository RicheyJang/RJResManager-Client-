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
void Messenger::finishPost()
{
    QNetworkReply* reply=qobject_cast<QNetworkReply *>(sender());
    emit gotResponse(reply);
}
