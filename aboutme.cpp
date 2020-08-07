#include "aboutme.h"
#include "ui_aboutme.h"

AboutMe::AboutMe(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AboutMe)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAboutMe();
    setWindowTitle(QString("关于我"));

    if (thisUser.identity == QString("teacher"))
        ui->radio_teacher->setChecked(true);
    else
        ui->radio_admin->setChecked(true);
}

AboutMe::~AboutMe()
{
    delete ui;
}

void AboutMe::setAboutMe()
{
    QString text = "";
    ui->table_main->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_main->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_main->setFocusPolicy(Qt::NoFocus);
    ui->table_main->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList titles;
    titles << QString("ID") << QString("用户名") << QString("真实姓名") << QString("身份") << QString("所属车间");

    QTableWidget* tab = ui->table_main;
    tab->clear();
    tab->setRowCount(titles.count());
    tab->setColumnCount(2);

    for (int i = 0; i < titles.count(); i++) {
        tab->setItem(i, 0, new QTableWidgetItem(titles[i]));
    }
    tab->setItem(0, 1, new QTableWidgetItem(QString::number(thisUser.id)));
    tab->setItem(1, 1, new QTableWidgetItem(thisUser.username));
    tab->setItem(2, 1, new QTableWidgetItem(thisUser.truename));
    tab->setItem(3, 1, new QTableWidgetItem(thisUser.identity));
    tab->setItem(4, 1, new QTableWidgetItem(thisUser.workshop));
}

void AboutMe::on_conferm_clicked()
{
    if (ui->radio_teacher->isChecked()) {
        if (thisUser.trueIdentity == QString("keeper") || thisUser.trueIdentity == QString("accountant")) {
            QMessageBox::warning(nullptr, QString("抱歉"), QString("您不具有教师权限"));
        } else {
            thisUser.identity = QString("teacher");
            emit needFlushMain();
        }
    } else if (ui->radio_admin->isChecked()) {
        if (thisUser.trueIdentity == QString("teacher")) {
            QMessageBox::warning(nullptr, QString("抱歉"), QString("您不具有管理员权限"));
        } else {
            thisUser.identity = thisUser.trueIdentity;
            emit needFlushMain();
        }
    }
    this->close();
}
