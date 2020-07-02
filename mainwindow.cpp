#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setLoginWindow(Login* _login)
{
    this->login = _login;
    connect(_login, &Login::allSuccess, this, &MainWindow::initMainWindow);
    connect(_login, &Login::suddenClose, this, &MainWindow::close);
}

void MainWindow::initMainWindow()
{
    ui->orderTable->setOrderTitle();
    for (int i = 0; i < 10; i++)
        dealButton[i] = nullptr;
    if (thisUser.identity != QString("admin") && thisUser.identity != QString("keeper") && thisUser.identity != QString("accountant"))
        ui->store->menuAction()->setVisible(false);
    if (thisUser.identity != QString("admin"))
        ui->people->menuAction()->setVisible(false);
    //ui->menutest->menuAction()->setVisible(false); //菜单栏某一菜单的隐藏
    on_showDeal_clicked();
    this->show();
}

/*-----------顶部按钮--------------*/
void MainWindow::on_newOrder_clicked()
{
    NewOrder* order = new NewOrder();
    order->show();
}

void MainWindow::on_showDeal_clicked()
{
    ui->orderTable->setOrderVec(&dealorders);
    setDealButton();
}

void MainWindow::on_showNow_clicked()
{
    ui->orderTable->setOrderVec(&noworders);
    QLayoutItem* child;
    while ((child = ui->buttomLayout->itemAt(0)) != nullptr) {
        ui->buttomLayout->removeItem(child);
        //child->widget()->disconnect();
        delete child->widget();
        delete child;
        child = nullptr;
    }
    for (int i = 0; i < 10; i++)
        dealButton[i] = nullptr;
}
/*-----------顶部按钮end------------*/

/*-----------底部按钮--------------*/
void MainWindow::setDealButton()
{
    int num = 0;
    for (int i = 0; i < 10; i++) {
        if (dealButton[i] != nullptr) {
            dealButton[i]->disconnect();
            delete dealButton[i];
        }
        dealButton[i] = nullptr;
    }
    if (thisUser.identity == QString("teacher")) {
        dealButton[1] = new QPushButton(QString("提交审核"));
        connect(dealButton[1], &QPushButton::clicked, this, &MainWindow::agreeTeacher);
        dealButton[2] = new QPushButton(QString("修改"));
        connect(dealButton[2], &QPushButton::clicked, this, &MainWindow::changeThisOrder);
        dealButton[3] = new QPushButton(QString("撤回订单"));
        connect(dealButton[1], &QPushButton::clicked, this, &MainWindow::disagreeTeacher);
        num = 3;
    } else if (thisUser.identity == QString("header")) {
        dealButton[1] = new QPushButton(QString("同意"));
        connect(dealButton[1], &QPushButton::clicked, this, &MainWindow::agreeHeader);
        dealButton[2] = new QPushButton(QString("修改"));
        connect(dealButton[2], &QPushButton::clicked, this, &MainWindow::changeThisOrder);
        dealButton[3] = new QPushButton(QString("驳回"));
        connect(dealButton[3], &QPushButton::clicked, this, &MainWindow::disagreeHeader);
        num = 3;
    } else if (thisUser.identity == QString("admin")) {
        dealButton[1] = new QPushButton(QString("同意"));
        connect(dealButton[1], &QPushButton::clicked, this, &MainWindow::agreeAdmin);
        dealButton[2] = new QPushButton(QString("修改"));
        connect(dealButton[2], &QPushButton::clicked, this, &MainWindow::changeThisOrder);
        dealButton[3] = new QPushButton(QString("驳回"));
        connect(dealButton[3], &QPushButton::clicked, this, &MainWindow::disagreeAdmin);
        num = 3;
    } else if (thisUser.identity == QString("keeper")) {
        dealButton[1] = new QPushButton(QString("已出库完成"));
        connect(dealButton[1], &QPushButton::clicked, this, &MainWindow::agreeKeeper);
        dealButton[2] = new QPushButton(QString("无法出库"));
        connect(dealButton[2], &QPushButton::clicked, this, &MainWindow::disagreeKeeper);
        num = 2;
    } else if (thisUser.identity == QString("accountant")) {
    }
    ui->buttomLayout->addStretch();
    for (int i = 1; i <= num; i++) {
        dealButton[i]->setMinimumHeight(35);
        ui->buttomLayout->addWidget(dealButton[i]);
    }
}
void MainWindow::changeThisOrder()
{
    int id = ui->orderTable->getCurrentID();
    if (id == 0)
        return;
    OneOrder* order = getOrder(id);
    if (order == nullptr)
        return;
    NewOrder* newOrder = new NewOrder();
    newOrder->changeOrder(order);
    newOrder->show();
}
bool askForConferm(QString text)
{
    QMessageBox::StandardButton res = QMessageBox::question(nullptr, QString("确定？"), text);
    return res == QMessageBox::Yes;
}
bool MainWindow::changeToByID(QString dowhat, int i)
{
    int id = ui->orderTable->getCurrentID();
    if (id == 0)
        return false;
    OneOrder* order = getOrder(id);
    if (order == nullptr) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return false;
    }
    QString ask = QString("确定") + dowhat + QString("该订单(编号为%1)吗？").arg(id);
    if (!askForConferm(ask))
        return false;
    return changeStatus(id, config.statusList[i]);
}
void MainWindow::agreeHeader()
{
    changeToByID(QString("同意"), 3);
}
void MainWindow::disagreeHeader()
{
    changeToByID(QString("驳回"), 2);
}
void MainWindow::agreeAdmin()
{
    changeToByID(QString("同意"), 5);
}
void MainWindow::disagreeAdmin()
{
    changeToByID(QString("驳回"), 4);
}
void MainWindow::agreeKeeper()
{
    changeToByID(QString("已完成"), 6);
}
void MainWindow::disagreeKeeper()
{
    changeToByID(QString("无法完成"), 7);
}
void MainWindow::agreeTeacher()
{
    changeToByID(QString("提交"), 1);
}
void MainWindow::disagreeTeacher()
{
    changeToByID(QString("撤销"), 0);
}
/*-----------底部按钮end------------*/

/*-----------修改状态---------------*/
bool MainWindow::changeStatus(int orderID, QString toStatus)
{
    QJsonObject json;
    QJsonObject orderInf;
    QJsonObject userInf;
    userInf.insert("username", thisUser.username);
    userInf.insert("password", thisUser.password);

    orderInf.insert("id", QJsonValue(orderID));
    orderInf.insert("status", QJsonValue(toStatus));

    json.insert("userInformation", QJsonValue(userInf));
    json.insert("orderInformation", QJsonValue(orderInf));

    postOn(json);
    return true;
}

void MainWindow::postOn(QJsonObject json)
{
    QJsonDocument document;
    document.setObject(json);

    QByteArray data = document.toJson(QJsonDocument::Indented);

    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QNetworkRequest* request = new QNetworkRequest;
    request->setHeader(QNetworkRequest::UserAgentHeader, config.UserAgent);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/json"); //上面语句固定这么写，要不然会报错“contest—type is missing”
    //request.setRawHeader("XXX3", "XXX4");

    QString Ptype;
    Ptype = QString("/changestatus");
    QString url = QString("http://") + config.ip + ':' + QString::number(config.serverPort) + Ptype;
    request->setUrl(QUrl(url));

    manager->post(*request, data);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishPost(QNetworkReply*)));
    connect(manager, &QNetworkAccessManager::finished, manager, &QNetworkAccessManager::deleteLater);
    delete request;
    request = nullptr;
}

void MainWindow::finishPost(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QMessageBox::information(nullptr, QString("完成"), QString("成功完成，请等待下一步"));
        reply->deleteLater();
    } else {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，网络错误或无权操作\n请稍后重试或更换账号"));
        reply->deleteLater();
    }
}
/*-----------修改状态end------------*/
