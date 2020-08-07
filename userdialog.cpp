#include "userdialog.h"
#include "ui_userdialog.h"

UserDialog::UserDialog(User user,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, true);

    ui->comboBox_isUse->setView(new QListView());
    ui->comboBox_identity->setView(new QListView());
    theOne=user;
    setTable();
    messenger=new Messenger();
}

UserDialog::~UserDialog()
{
    delete ui;
}

void UserDialog::setTable()
{
    ui->label_username->setText(theOne.username);
    ui->truenameEdit->setText(theOne.truename);
    ui->workshopEdit->setText(theOne.workshop);
    ui->rePass_check->setChecked(false);

    QStringList sl;
    sl<<QString("不可用")<<QString("可用");
    ui->comboBox_isUse->clear();
    ui->comboBox_isUse->addItems(sl);
    ui->comboBox_isUse->setCurrentIndex(theOne.isUseful);

    ui->comboBox_identity->clear();
    ui->comboBox_identity->addItems(config.userCNIdentityList);
    int i=0;
    for(QString s : config.userIdentityList)
    {
        if(theOne.identity==s)
        {
            ui->comboBox_identity->setCurrentIndex(i);
            break;
        }
        i++;
    }
}

void UserDialog::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton re=QMessageBox::question(nullptr,QString("确定？"),QString("用户修改尚未保存提交\n\t确认退出修改？"));

    if(re==QMessageBox::Yes)
        this->close();
    else
        event->ignore();
}

void UserDialog::on_button_cancel_clicked()
{
    this->close();
}

void UserDialog::on_button_confirm_clicked()
{
    theOne.truename=ui->truenameEdit->text();
    theOne.workshop=ui->workshopEdit->text();
    //TODO 修改用户2
    ui->rePass_check->setChecked(false);

    QStringList sl;
    sl<<QString("不可用")<<QString("可用");
    ui->comboBox_isUse->clear();
    ui->comboBox_isUse->addItems(sl);
    ui->comboBox_isUse->setCurrentIndex(theOne.isUseful);

    ui->comboBox_identity->clear();
    ui->comboBox_identity->addItems(config.userCNIdentityList);
    int i=0;
    for(QString s : config.userIdentityList)
    {
        if(theOne.identity==s)
        {
            ui->comboBox_identity->setCurrentIndex(i);
            break;
        }
        i++;
    }

    messenger->changeUser(theOne);
}
