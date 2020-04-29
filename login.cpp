#include "login.h"
#include "mainwindow.h"
#include "ui_login.h"

Login::Login(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle(QString("登录账号"));
    ui->progressBar->setValue(0);
    ui->progressBar->hide();

    connect(this, &Login::succsesslogin, this, &Login::whenSuccesslogin);
    connect(this, &Login::faillogin, this, &Login::whenFaillogin);
    connect(this, &Login::wrongpassword, this, &Login::whenWrongpassword);
}

Login::~Login()
{
    delete ui;
}

int Login::trylogin(QString username, QString password)
{
    QJsonObject json;
    json.insert("username", username);
    QString sendpsw = toSHA256(password);
    json.insert("password", sendpsw);

    QJsonDocument document;
    document.setObject(json);

    QByteArray data = document.toJson(QJsonDocument::Indented);

    manager = new QNetworkAccessManager();
    request = new QNetworkRequest;
    request->setHeader(QNetworkRequest::UserAgentHeader, config.UserAgent);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/json"); //上面语句固定这么写，要不然会报错“contest—type is missing”
    //request.setRawHeader("XXX3", "XXX4");

    QString url = QString("http://") + config.ip + ':' + QString::number(config.serverPort) + QString("/login");
    request->setUrl(QUrl(url));

    manager->post(*request, data);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishTrylogin(QNetworkReply*)));
    delete request;
    request = nullptr;

    return 1;
}

void Login::finishTrylogin(QNetworkReply* reply)
{
    int state = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).value<int>();
    if (state == 200) {
        //QByteArray byte_array = reply->readAll();
        //QString str(byte_array);
        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &jsonError);
        QJsonObject json;
        bool flag = 0;
        if (!document.isNull() && document.isObject() && jsonError.error == QJsonParseError::NoError) {
            json = document.object();
            flag = 1;
            if (json.contains("isUseful")) {
                if (json.value("isUseful").toInt() == 0)
                    flag = 0;
            } else
                flag = 0;
            if (json.contains("username"))
                thisUser.username = json.value("username").toString();
            else
                flag = 0;
            if (json.contains("password"))
                thisUser.password = json.value("password").toString();
            else
                flag = 0;
            if (json.contains("useName"))
                thisUser.useName = json.value("useName").toString();
            else
                flag = 0;
            if (json.contains("usePassword"))
                thisUser.usePassword = json.value("usePassword").toString();
            else
                flag = 0;
            if (json.contains("id"))
                thisUser.id = json.value("id").toInt();
            else
                flag = 0;
            if (json.contains("truename"))
                thisUser.truename = json.value("truename").toString();
            else
                flag = 0;
            if (json.contains("workshop"))
                thisUser.workshop = json.value("workshop").toString();
            else
                flag = 0;
            if (json.contains("storehouse"))
                thisUser.storehouse = json.value("storehouse").toString();
            else
                flag = 0;
            if (json.contains("identity"))
                thisUser.identity = json.value("identity").toString();
            else
                flag = 0;
        }
        if (flag)
            emit succsesslogin();
        else
            emit faillogin();
    } else if (state == 403) {
        emit wrongpassword();

    } else {
        emit faillogin();
    }
    reply->deleteLater();
    manager->deleteLater();
}

void Login::on_checkIn_clicked()
{
    QString name = ui->idEdit->text();
    QString psword = ui->passwordEdit->text();
    if (name.size() <= 0 || name.size() >= MaxIDLength || psword.size() <= 0 || psword.size() >= MaxIDLength) {
        ui->statusLable->setText(QString("请正确输入账号密码"));
        ui->statusLable->setStyleSheet("color:red");
        return;
    }
    QString pwreg("^([A-Za-z0-9]|[._]){6,20}$");
    QString namereg("^(\\w){3,20}$");
    if (!regCheck(namereg, name) || !regCheck(pwreg, psword)) {
        ui->statusLable->setText(QString("账号密码格式错误"));
        ui->statusLable->setStyleSheet("color:red");
        return;
    }
    trylogin(name, psword);
}

void Login::closeEvent(QCloseEvent* event)
{
    emit suddenClose();
}

void Login::whenSuccesslogin()
{
    ui->statusLable->setText(QString("登录成功"));
    ui->statusLable->setStyleSheet("color:green");
    allInit();
}
void Login::whenFaillogin()
{
    ui->statusLable->setText(QString("登录失败"));
    ui->statusLable->setStyleSheet("color:red");
}
void Login::whenWrongpassword()
{
    ui->statusLable->setText(QString("账号或密码错误"));
    ui->statusLable->setStyleSheet("color:red");
}

void Login::allInit()
{
    ui->progressBar->show();
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
    if (!initItems()) {
        QMessageBox::warning(nullptr, QString("警告"), QString("连接数据库出错\nInitItems wrong!"));
        ui->progressBar->hide();
        return;
    }
    ui->progressBar->setValue(20);
    if (!initDealOrders()) {
        QMessageBox::warning(nullptr, QString("警告"), QString("连接数据库出错\nInitItems wrong!"));
        ui->progressBar->hide();
        return;
    }
    ui->progressBar->setValue(40);
    if (!initNowOrders()) {
        QMessageBox::warning(nullptr, QString("警告"), QString("连接数据库出错\nInitItems wrong!"));
        ui->progressBar->hide();
        return;
    }
    ui->progressBar->setValue(60);
    emit allSuccess();
    this->hide();
}
