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
    connect(this, &Login::hasNewVersion, this, &Login::whenHasNewVersion);
    connect(this, &Login::faillogin, this, &Login::whenFaillogin);
    connect(this, &Login::wrongpassword, this, &Login::whenWrongpassword);
}

Login::~Login()
{
    delete ui;
}

void Login::on_checkIn_clicked() //点击登录按钮，并检查用户名密码
{
    QString name = ui->idEdit->text();
    QString psword = ui->passwordEdit->text();
    if (name.size() <= 0 || name.size() >= MaxIDLength || psword.size() <= 0 || psword.size() >= MaxIDLength) {
        ui->statusLable->setText(QString("请正确输入账号密码"));
        ui->statusLable->setStyleSheet("color:red");
        return;
    }
    if (!passwordCheck(psword) || !usernameCheck(name)) {
        ui->statusLable->setText(QString("账号密码格式错误"));
        ui->statusLable->setStyleSheet("color:red");
        return;
    }
    trylogin(name, psword);
}

int Login::trylogin(QString username, QString password) //开始登录
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

void Login::finishTrylogin(QNetworkReply* reply) //登录后，接收信息并进行检查
{
    int state = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).value<int>();
    if (state == 200) {
        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &jsonError);
        QJsonObject json;
        QString newClientVersion = config.nowClientVersion;
        bool flag = 0;
        if (!document.isNull() && document.isObject() && jsonError.error == QJsonParseError::NoError) {
            json = document.object();
            flag = 1;
            if (json.contains("isUseful")) {
                if (json.value("isUseful").toInt() == 0)
                    flag = 0;
            } else
                flag = 0;
            if (json.contains("id"))
                thisUser.id = json.value("id").toInt();
            else
                flag = 0;
            if (json.contains("username"))
                thisUser.username = json.value("username").toString();
            else
                flag = 0;
            if (json.contains("password"))
                thisUser.password = json.value("password").toString();
            else
                flag = 0;
            if (json.contains("identity"))
                thisUser.trueIdentity = json.value("identity").toString();
            else
                flag = 0;
            thisUser.identity = thisUser.trueIdentity;
            if (json.contains("useName"))
                thisUser.useName = json.value("useName").toString();
            else
                flag = 0;
            if (json.contains("usePassword"))
                thisUser.usePassword = json.value("usePassword").toString();
            else
                flag = 0;
            if (json.contains("newClientVersion"))
                newClientVersion = json.value("newClientVersion").toString();
        }
        if (flag != 0) {
            flag = 0;
            //Database* base = new Database(config.ip, config.dataPort, config.basename, thisUser.useName, thisUser.usePassword);
            useBase = new Database(config.ip, config.dataPort, config.basename, thisUser.useName, thisUser.usePassword);
            QSqlDatabase database = useBase->getDatabase();
            QSqlQuery query(database);
            query.exec("select truename,workshop,storehouse from user where id=" + QString::number(thisUser.id) + ";");
            while (query.next()) {
                thisUser.truename = query.value(0).toString();
                thisUser.workshop = query.value(1).toString();
                thisUser.storehouse = query.value(2).toString();
                flag = 1;
            }
        }
        if (flag) {
            if (newClientVersion != config.nowClientVersion)
                emit hasNewVersion(newClientVersion);
            else
                emit succsesslogin();
        } else
            emit faillogin();
    } else if (state == 403) {
        emit wrongpassword();
    } else {
        emit faillogin();
    }
    reply->deleteLater();
    manager->deleteLater();
}

void Login::closeEvent(QCloseEvent* event)
{
    emit suddenClose();
}

void Login::whenSuccesslogin() //成功登录信号
{
    ui->statusLable->setText(QString("登录成功"));
    ui->statusLable->setStyleSheet("color:green");
    allInit();
}

void Login::whenFaillogin() //登录失败信号
{
    ui->statusLable->setText(QString("登录失败"));
    ui->statusLable->setStyleSheet("color:red");
}
void Login::whenWrongpassword() //密码错误信号
{
    ui->statusLable->setText(QString("账号或密码错误"));
    ui->statusLable->setStyleSheet("color:red");
}

void Login::allInit() //全局初始化
{
    ui->progressBar->show();
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(20);
    if (ui->asTeacher->isChecked() && !ui->asManager->isChecked()) {
        if (thisUser.identity != QString("keeper") && thisUser.identity != QString("accountant"))
            thisUser.identity = QString("teacher");
    }
    if (!initResItems()) {
        QMessageBox::warning(nullptr, QString("警告"), QString("连接数据库出错\nInitItems wrong!"));
        ui->progressBar->hide();
        return;
    }
    ui->progressBar->setValue(40);
    if (!initDealOrders()) {
        QMessageBox::warning(nullptr, QString("警告"), QString("连接数据库出错\nInitDealOrders wrong!"));
        ui->progressBar->hide();
        return;
    }
    ui->progressBar->setValue(60);
    if (!initNowOrders()) {
        QMessageBox::warning(nullptr, QString("警告"), QString("连接数据库出错\nInitNowOrders wrong!"));
        ui->progressBar->hide();
        return;
    }
    ui->progressBar->setValue(80);
    if (!initSatus()) {
        QMessageBox::warning(nullptr, QString("警告"), QString("连接数据库出错\nInitSatus wrong!"));
        ui->progressBar->hide();
        return;
    }
    ui->progressBar->setValue(100);
    emit allSuccess();
    this->hide();
}

void Login::whenHasNewVersion(QString newVersion) //有新版本
{
    QString url = "http://" + config.ip + ":" + QString::number(config.serverPort) + "/download" + "/RJStoreSystem-"+newVersion+"-setup.exe";
    ui->statusLable->setOpenExternalLinks(true);
    ui->statusLable->setStyleSheet("color:black");
    QString here = "<a href=\"" + url + QString("\">这里</a>");
    QString text = QString("有新版本！点击") + here + QString("下载");
    ui->statusLable->setText(text);
    ui->checkIn->setDisabled(true);
    //TODO 下载新版本download待写
    //ui->statusLable->setText(QString("下载新版本中，下载完成后请删除老版本"));
}

void Login::download(QString url)
{
    QStringList list = url.split("/");
    QString filename = QDir::currentPath() + "/" + list.at(list.length() - 1);
    QFile* file = new QFile(filename);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, "warning", "打开失败");
        return;
    }
}
