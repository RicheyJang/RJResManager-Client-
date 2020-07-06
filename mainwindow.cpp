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

void MainWindow::flushMainWindow()
{
    if (thisUser.identity != QString("admin") && thisUser.identity != QString("keeper") && thisUser.identity != QString("accountant"))
        ui->store->menuAction()->setVisible(false);
    else
        ui->store->menuAction()->setVisible(true);
    if (thisUser.identity != QString("admin"))
        ui->people->menuAction()->setVisible(false);
    else
        ui->people->menuAction()->setVisible(true);
    if (thisUser.identity != QString("teacher"))
        ui->newOrder->setDisabled(true);
    else
        ui->newOrder->setDisabled(false);
    on_showDeal_clicked();
}

void MainWindow::initMainWindow()
{
    ui->orderTable->setOrderTitle();
    for (int i = 0; i < 10; i++)
        dealButton[i] = nullptr;
    flushMainWindow();
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
    setNowButton();
}

void MainWindow::on_showHistory_clicked()
{
    ui->orderTable->setOrderVec(&historys);
    setHistoryButton();
}
/*-----------顶部按钮end------------*/

/*-----------底部按钮--------------*/
void MainWindow::setHistoryButton()
{
    int num = 0;
    QLayoutItem* child;
    while ((child = ui->buttomLayout->itemAt(0)) != nullptr) {
        ui->buttomLayout->removeItem(child);
        delete child->widget();
        delete child;
        child = nullptr;
    }
    for (int i = 0; i < 10; i++)
        dealButton[i] = nullptr;
    if (thisUser.identity != QString("keeper") && thisUser.identity != QString("accountant")) {
        dealButton[1] = new QPushButton(QString("重新发起该订单"));
        connect(dealButton[1], &QPushButton::clicked, this, &MainWindow::againOrder);
        num = 1;
    }
    ui->buttomLayout->addStretch();
    for (int i = 1; i <= num; i++) {
        dealButton[i]->setMinimumHeight(35);
        ui->buttomLayout->addWidget(dealButton[i]);
    }
}
void MainWindow::setNowButton()
{
    int num = 0;
    QLayoutItem* child;
    while ((child = ui->buttomLayout->itemAt(0)) != nullptr) {
        ui->buttomLayout->removeItem(child);
        delete child->widget();
        delete child;
        child = nullptr;
    }
    for (int i = 0; i < 10; i++)
        dealButton[i] = nullptr;
    if (thisUser.identity == QString("teacher")) {
        dealButton[1] = new QPushButton(QString("撤回订单"));
        connect(dealButton[1], &QPushButton::clicked, this, &MainWindow::disagreeTeacher);
        num = 1;
    }
    ui->buttomLayout->addStretch();
    for (int i = 1; i <= num; i++) {
        dealButton[i]->setMinimumHeight(35);
        ui->buttomLayout->addWidget(dealButton[i]);
    }
}
void MainWindow::setDealButton()
{
    int num = 0;
    QLayoutItem* child;
    while ((child = ui->buttomLayout->itemAt(0)) != nullptr) {
        ui->buttomLayout->removeItem(child);
        delete child->widget();
        delete child;
        child = nullptr;
    }
    for (int i = 0; i < 10; i++)
        dealButton[i] = nullptr;
    if (thisUser.identity == QString("teacher")) {
        dealButton[1] = new QPushButton(QString("提交审核"));
        connect(dealButton[1], &QPushButton::clicked, this, &MainWindow::agreeTeacher);
        dealButton[2] = new QPushButton(QString("修改"));
        connect(dealButton[2], &QPushButton::clicked, this, &MainWindow::changeThisOrder);
        dealButton[3] = new QPushButton(QString("撤回订单"));
        connect(dealButton[3], &QPushButton::clicked, this, &MainWindow::disagreeTeacher);
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
        dealButton[1] = new QPushButton(QString("已出(入)库完成"));
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
bool askForConferm(QString text)
{
    QMessageBox::StandardButton res = QMessageBox::question(nullptr, QString("确定？"), text);
    return res == QMessageBox::Yes;
}
void MainWindow::againOrder() //重新发起订单
{
    int id = ui->orderTable->getCurrentID();
    if (id == 0) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    OneOrder* order = getOrder(id, historys); //WARNING 只能重新发起历史订单
    if (order == nullptr) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    QString ask = QString("确定重新发起该订单(编号为%1)吗？").arg(id);
    if (!askForConferm(ask))
        return;
    NewOrder* Norder = new NewOrder();
    Norder->setModel(*order);
    Norder->show();
}
void MainWindow::changeThisOrder() //修改选中订单
{
    int id = ui->orderTable->getCurrentID();
    if (id == 0)
        return;
    OneOrder* order = getOrder(id, dealorders); //只可以修改待处理订单
    if (order == nullptr)
        return;
    NewOrder* newOrder = new NewOrder();
    newOrder->changeOrder(order);
    connect(newOrder, &NewOrder::finishChange, this, &MainWindow::afterChange);
    newOrder->show();
}
bool MainWindow::changeToByID(QString dowhat, int i) //修改订单状态
{
    int id = ui->orderTable->getCurrentID();
    if (id == 0)
        return false;
    OneOrder* order = getOrder(id, dealorders);
    if (order == nullptr) { //除撤销订单外，只能修改待处理订单
        if (i == 0)
            order = getOrder(id, noworders);
        if (order == nullptr) {
            QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
            return false;
        }
    }
    QString ask = QString("确定") + dowhat + QString("该订单(编号为%1)吗？").arg(id);
    if (!askForConferm(ask))
        return false;
    return changeStatus(id, config.statusList[i]);
}
void MainWindow::afterChange()
{
    ui->orderTable->flush();
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
    int id = ui->orderTable->getCurrentID();
    if (id == 0) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    OneOrder* order = getOrder(id, dealorders); //只能同意待处理订单
    if (order == nullptr) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    if (!orderCheck(*order)) {
        QMessageBox::warning(nullptr, QString("错误"), QString("仓库中物品数量不足！\n请修改订单或检查库存"));
        return;
    }
    changeToByID(QString("同意"), 5);
}
void MainWindow::disagreeAdmin()
{
    changeToByID(QString("驳回"), 4);
}
void MainWindow::agreeKeeper()
{
    int id = ui->orderTable->getCurrentID();
    if (id == 0) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    OneOrder* order = getOrder(id, dealorders); //只能同意待处理订单
    if (order == nullptr) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    QString ask = QString("确定该订单(编号为%1)已出(还)库完成吗？").arg(id);
    if (!askForConferm(ask))
        return;
    finishOrder(id);
    return;
}
void MainWindow::disagreeKeeper()
{
    int id = ui->orderTable->getCurrentID();
    if (id == 0)
        return;
    OneOrder* order = getOrder(id, dealorders);
    if (order == nullptr) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    if (order->status != config.statusList[5]) {
        QMessageBox::warning(nullptr, QString("错误"), QString("此类订单无法驳回"));
        return;
    }
    QString ask = QString("确定无法完成该订单(编号为%1)吗？").arg(id);
    if (!askForConferm(ask))
        return;
    changeStatus(id, config.statusList[6]);
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
bool MainWindow::finishOrder(int orderID)
{
    QJsonObject json;
    QJsonObject orderInf;
    QJsonObject userInf;
    userInf.insert("username", thisUser.username);
    userInf.insert("password", thisUser.password);

    orderInf.insert("id", QJsonValue(orderID));

    json.insert("userInformation", QJsonValue(userInf));
    json.insert("orderInformation", QJsonValue(orderInf));

    postOn(json, QString("/finishorder"));
    return true;
}

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

    postOn(json, QString("/changestatus"));
    return true;
}

void MainWindow::postOn(QJsonObject json, QString uri)
{
    QJsonDocument document;
    document.setObject(json);

    QByteArray data = document.toJson(QJsonDocument::Indented);

    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QNetworkRequest* request = new QNetworkRequest;
    request->setHeader(QNetworkRequest::UserAgentHeader, config.UserAgent);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/json"); //上面语句固定这么写，要不然会报错“contest—type is missing”

    QString url = QString("http://") + config.ip + ':' + QString::number(config.serverPort) + uri;
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
        afterChange();
    } else {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，网络错误或无权操作\n请稍后重试或更换账号"));
        reply->deleteLater();
    }
}
/*-----------修改状态end------------*/

/*-----------菜单栏-----------------*/
void MainWindow::on_about_me_triggered()
{
    AboutMe* am = new AboutMe();
    connect(am, &AboutMe::needFlushMain, this, &MainWindow::flushMainWindow);
    am->show();
}

void MainWindow::on_about_writer_triggered()
{
    QString emailA = QString("richeyjang@163.com");
    QString text = QString("RJ仓库管理系统 v") + config.nowClientVersion
        + QString("\n反馈：") + emailA + QString("\n\t作者：姜雨奇\n\t院校：HUST");
    QMessageBox::information(nullptr, QString("关于本软件"), text);
}

void MainWindow::on_store_showAll_triggered()
{
    ItemsManager* itemM = new ItemsManager();
    itemM->show();
}

void MainWindow::on_people_change_triggered()
{
    UserManager* um = new UserManager();
    um->show();
}
