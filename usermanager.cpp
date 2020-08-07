#include "usermanager.h"
#include "ui_usermanager.h"
#include "userdialog.h"

UserManager::UserManager(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::UserManager)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, true);

    ui->table_User->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_User->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->table_User->setFocusPolicy(Qt::NoFocus);
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
    getAllUsers(users);
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

void UserManager::on_table_User_cellDoubleClicked(int row, int column)
{
    int id=ui->table_User->item(row,0)->text().toInt();
    if(id==0)
        return;

    User *user=getUserByID(id,&users);
    if(user==nullptr)
        return;

    UserDialog* UD=new UserDialog(*user);
    UD->show();
}
