#include "usermanager.h"
#include "ui_usermanager.h"

UserManager::UserManager(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::UserManager)
{
    ui->setupUi(this);
    ui->table_User->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_User->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_User->setFocusPolicy(Qt::NoFocus);
    ui->table_User->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_User->setSortingEnabled(true);

    titles << QString("编号") << QString("用户名") << QString("真实姓名") << QString("身份") << QString("所属车间") << QString("可用性");
    flushUsers();
}

UserManager::~UserManager()
{
    delete ui;
}

void UserManager::flushUsers()
{
    users.clear();
    Database* base = new Database(config.ip, config.dataPort, config.basename, thisUser.useName, thisUser.usePassword);
    QSqlDatabase database = base->getDatabase();
    QSqlQuery query(database);
    query.exec("select id,username,truename,identity,workshop,isUseful from user;");
    while (query.next()) {
        User user;
        user.id = query.value(0).toInt();
        user.username = query.value(1).toString();
        user.truename = query.value(2).toString();
        user.identity = query.value(3).toString();
        user.workshop = query.value(4).toString();
        user.isUseful = query.value(5).toBool();
        users.push_back(user);
    }
    base->close();
    delete base;
    std::sort(users.begin(), users.end());
    setTable();
}

void UserManager::setTable()
{
    QTableWidget* tab = ui->table_User;

    tab->clear();
    tab->setColumnCount(titles.count());
    tab->setHorizontalHeaderLabels(titles);
    int cnt = 0;
    for (User user : users) {
        cnt++;
        tab->setRowCount(cnt);
        tab->setItem(cnt - 1, 0, new QTableWidgetItem(QString::number(user.id)));
        tab->setItem(cnt - 1, 1, new QTableWidgetItem(user.username));
        tab->setItem(cnt - 1, 2, new QTableWidgetItem(user.truename));
        tab->setItem(cnt - 1, 3, new QTableWidgetItem(user.identity));
        tab->setItem(cnt - 1, 4, new QTableWidgetItem(user.workshop));
        if (user.isUseful)
            tab->setItem(cnt - 1, 5, new QTableWidgetItem(QString("可用")));
        else
            tab->setItem(cnt - 1, 5, new QTableWidgetItem(QString("不可用")));
    }
}
