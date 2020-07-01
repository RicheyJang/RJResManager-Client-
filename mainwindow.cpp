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
    for(int i=0;i<10;i++)
        dealButton[i]=nullptr;
    //ui->orderTable->setOrderVec(&dealorders);
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
    QLayoutItem *child;
    while ((child = ui->buttomLayout->itemAt(0)) != nullptr) {
        ui->buttomLayout->removeItem(child);
        //child->widget()->disconnect();
        delete child->widget();
        delete child;
        child = nullptr;
    }
    for(int i=0;i<10;i++)
        dealButton[i]=nullptr;
}
/*-----------顶部按钮end------------*/

/*-----------底部按钮--------------*/
void MainWindow::setDealButton()
{
    int num=0;
    for(int i=0;i<10;i++)
    {
        if(dealButton[i]!=nullptr)
        {
            dealButton[i]->disconnect();
            delete dealButton[i];
        }
        dealButton[i]=nullptr;
    }
    if(thisUser.identity==QString("teacher")) {
        dealButton[1]=new QPushButton(QString("提交审核"));
        dealButton[2]=new QPushButton(QString("修改"));
        connect(dealButton[2],&QPushButton::clicked,this,&MainWindow::changeThisOrder);
        dealButton[3]=new QPushButton(QString("撤回订单"));
        num=3;
    } else if(thisUser.identity==QString("header")) {
        dealButton[1]=new QPushButton(QString("同意"));
        connect(dealButton[1],&QPushButton::clicked,this,&MainWindow::agreeHeader);
        dealButton[2]=new QPushButton(QString("修改"));
        connect(dealButton[2],&QPushButton::clicked,this,&MainWindow::changeThisOrder);
        dealButton[3]=new QPushButton(QString("撤回订单"));
        num=3;
    } else if(thisUser.identity==QString("admin")) {
        dealButton[1]=new QPushButton(QString("修改"));
        connect(dealButton[1],&QPushButton::clicked,this,&MainWindow::changeThisOrder);
        num++;
    } else if(thisUser.identity==QString("keeper")) {
    } else if(thisUser.identity==QString("accountant")) {
    }
    ui->buttomLayout->addStretch();
    for(int i=1;i<=num;i++)
    {
        dealButton[i]->setMinimumHeight(35);
        ui->buttomLayout->addWidget(dealButton[i]);
    }
}
void MainWindow::changeThisOrder()
{
    int id=ui->orderTable->getCurrentID();
    OneOrder* order=getOrder(id);
    if(order==nullptr)
        return;
    NewOrder* newOrder = new NewOrder();
    newOrder->changeOrder(order);
    newOrder->show();
}
void MainWindow::agreeHeader()
{
    int id=ui->orderTable->getCurrentID();
    OneOrder* order=getOrder(id);
    if(order==nullptr)
    {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    changeStatus(id,config.statusList[3]);
    return ;
}
void MainWindow::disagreeHeader()
{
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
    orderInf.insert("status",QJsonValue(toStatus));

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
    Ptype=QString("/changestatus");
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
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        reply->deleteLater();
    }
}
/*-----------修改状态end------------*/
