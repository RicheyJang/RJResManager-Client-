#include "itemsmanager.h"
#include "ui_itemsmanager.h"

ItemsManager::ItemsManager(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::ItemsManager)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->tabWidget->setTabText(0, config.itemsList[0]);
    ui->tabWidget->setTabText(1, config.itemsList[1]);
    setWindowTitle(QString("库存管理"));
    QTableWidget* tab = ui->table_Out;
    tab->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab->setSelectionMode(QAbstractItemView::NoSelection);
    tab->setFocusPolicy(Qt::NoFocus);
    tab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tab->setSortingEnabled(true);
    tab = ui->table_Rent;
    tab->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab->setSelectionMode(QAbstractItemView::NoSelection);
    tab->setFocusPolicy(Qt::NoFocus);
    tab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tab->setSortingEnabled(true);
    ui->tabWidget->setCurrentIndex(0);
    titles.clear();
    titles << QString("物品编号") << QString("名称") << QString("型号") << QString("数量") << QString("单位");
    setTable(ui->table_Out, config.itemsList[0]);
    setDealButton();

    messenger=new Messenger();
    connect(messenger,&Messenger::gotResponse,this,&ItemsManager::finishPost);
    ui->table_dealOrder->setIsResOrder(true);
    ui->table_dealOrder->setOrderVec(&dealResOrders);
    ui->table_hisOrder->setIsResOrder(true);
}

ItemsManager::~ItemsManager()
{
    delete ui;
}

void ItemsManager::on_tabWidget_currentChanged(int index)
{
    if (index == 0)
        setTable(ui->table_Out, config.itemsList[0]);
    else if (index == 1)
        setTable(ui->table_Rent, config.itemsList[1]);
    else if(index==2)
        ui->table_dealOrder->flush();
    else if(index==3)
        ui->table_hisOrder->flush();
}

void ItemsManager::setTable(QTableWidget* tab, QString res)
{
    initResItems();

    tab->clear();
    tab->setColumnCount(titles.count());
    tab->setHorizontalHeaderLabels(titles);
    int cnt = 0;
    QSet<OneResItem> st = getResItemsByRes(res);
    for (OneResItem item : st) {
        cnt++;
        tab->setRowCount(cnt);
        tab->setItem(cnt - 1, 0, new QTableWidgetItem(QString::number(item.pid)));
        tab->setItem(cnt - 1, 1, new QTableWidgetItem(item.name));
        tab->setItem(cnt - 1, 2, new QTableWidgetItem(item.type));
        tab->setItem(cnt - 1, 3, new QTableWidgetItem(QString::number(item.cnt, 'g', 3)));
        tab->setItem(cnt - 1, 4, new QTableWidgetItem(item.units));
    }
}

/*---------按钮设置-------------*/
void ItemsManager::setDealButton()
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
    if (thisUser.identity == QString("admin")) {
        dealButton[1] = new QPushButton(QString("允许入库"));
        connect(dealButton[1], &QPushButton::clicked, this, &ItemsManager::agreeAdmin);
        dealButton[2] = new QPushButton(QString("驳回"));
        connect(dealButton[2], &QPushButton::clicked, this, &ItemsManager::disagreeAdmin);
        num = 2;
    } else if (thisUser.identity == QString("keeper")) {
        dealButton[1] = new QPushButton(QString("修改账目"));
        connect(dealButton[1], &QPushButton::clicked, this, &ItemsManager::changeThisOrder);
        dealButton[2] = new QPushButton(QString("提交审核"));
        connect(dealButton[2], &QPushButton::clicked, this, &ItemsManager::agreeKeeper);
        dealButton[3] = new QPushButton(QString("撤回"));
        connect(dealButton[3], &QPushButton::clicked, this, &ItemsManager::disagreeKeeper);
        num = 3;
    }
    ui->buttomLayout->addStretch();
    for (int i = 1; i <= num; i++) {
        dealButton[i]->setMinimumHeight(35);
        ui->buttomLayout->addWidget(dealButton[i]);
    }
}

void ItemsManager::agreeAdmin()
{
    int id = ui->table_dealOrder->getCurrentID();
    if (id == 0) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    OneOrder* order = getOrder(id, dealResOrders); //只能同意待处理订单
    if (order == nullptr) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    QString ask = QString("确定该账目(编号为%1)可以入库吗？").arg(id);
    if (!askForConferm(ask))
        return;
    messenger->finishResOrder(id);
    return;
}
void ItemsManager::disagreeAdmin()
{
    changeToByID(QString("驳回"), 10);
}
void ItemsManager::changeThisOrder() //修改选中订单
{
    int id = ui->table_dealOrder->getCurrentID();
    if (id == 0)
        return;
    OneOrder* order = getOrder(id, dealResOrders); //只可以修改待处理订单
    if (order == nullptr)
        return;
    NewItemOrder* newOrder = new NewItemOrder();
    newOrder->changeOrder(order);
    connect(newOrder, &NewItemOrder::finishChange, this, &ItemsManager::afterChange);
    newOrder->show();
}
void ItemsManager::agreeKeeper()
{
    changeToByID(QString("再次提交"), 9);
}
void ItemsManager::disagreeKeeper()
{
    changeToByID(QString("撤销"), 0);
}
void ItemsManager::afterChange()
{
    ui->table_dealOrder->flush();
}
/*---------按钮设置end-----------*/

/*---------数据上传函数----------*/
bool ItemsManager::changeToByID(QString dowhat, int i) //修改订单状态
{
    int id = ui->table_dealOrder->getCurrentID();
    if (id == 0)
        return false;
    OneResOrder* order = getOrder(id, dealResOrders);
    if (order == nullptr) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return false;
    }
    QString ask = QString("确定") + dowhat + QString("该账目(编号为%1)吗？").arg(id);
    if (!askForConferm(ask))
        return false;
    return changeStatus(id, config.statusList[i]);
}
bool ItemsManager::changeStatus(int orderID, QString toStatus)
{
    messenger->changeOneStatus(orderID,toStatus);
    return true;
}
void ItemsManager::finishPost(QNetworkReply* reply)
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
/*---------数据上传函数end--------*/
