#include "userdialog.h"
#include "ui_userdialog.h"

UserDialog::UserDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, true);

    ui->comboBox_isUse->setView(new QListView());
    ui->comboBox_identity->setView(new QListView());

    messenger=new Messenger();
    connect(messenger,&Messenger::gotResponse,this,&UserDialog::afterConfirmReply);
    setAddNewUser();
    allFinish=false;
}

UserDialog::~UserDialog()
{
    delete ui;
}

/*--------类型设置---------*/
void UserDialog::setChangeUser(User user)
{
    forOne=user;
    setTable(forOne);
    ui->usernameEdit->setReadOnly(true);
    dowhat=ONCHANGEUSER;
}
void UserDialog::setAddNewUser()
{
    forOne.id=0;
    forOne.identity=config.userIdentityList[0];
    forOne.username="";
    forOne.truename="";
    forOne.workshop="";
    forOne.isUseful=true;
    forOne.password=nullptr;
    setTable(forOne);
    dowhat=ONADDNEWUSER;
}
/*--------类型设置end-------*/

/*--------界面设置---------*/
void UserDialog::setTable(User theOne)
{
    ui->usernameEdit->setText(theOne.username);
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
    if(!allFinish)
    {
        QMessageBox::StandardButton re=QMessageBox::question(nullptr,QString("确定？"),QString("用户修改尚未保存提交\n\t确认退出修改？"));

        if(re==QMessageBox::Yes)
            this->close();
        else
            event->ignore();
    }
}
/*--------界面设置end-------*/

/*------------按钮响应---------*/
void UserDialog::on_button_cancel_clicked()
{
    this->close();
}

void UserDialog::on_button_confirm_clicked()
{
    forOne.username=ui->usernameEdit->text();
    forOne.truename=ui->truenameEdit->text();
    forOne.workshop=ui->workshopEdit->text();
    if(ui->rePass_check->isChecked())
        forOne.password=QString("000000");
    else
        forOne.password=nullptr;
    forOne.isUseful=ui->comboBox_isUse->currentIndex();
    int i=ui->comboBox_identity->currentIndex();
    forOne.identity=config.userIdentityList[i];
    if(!userCheck(forOne))
    {
        QMessageBox::warning(nullptr,QString("警告"),QString("信息有误，请检查后提交"));
        return ;
    }

    if(dowhat==ONCHANGEUSER)
    {
        QJsonObject json;
        QJsonObject appInf;
        QJsonObject userInf;
        userInf.insert("username", thisUser.username);
        userInf.insert("password", thisUser.password);

        appInf.insert("id",forOne.id);
        appInf.insert("truename",forOne.truename);
        appInf.insert("workshop",forOne.workshop);
        if(forOne.password!=nullptr)
        {
            appInf.insert("password",toSHA256(forOne.password));
        }
        int isUse=forOne.isUseful ? 1 : 0;
        appInf.insert("isUseful",isUse);
        appInf.insert("identity",forOne.identity);

        json.insert("userInformation", QJsonValue(userInf));
        json.insert("applicantInformation", QJsonValue(appInf));

        messenger->changeUser(json);
    }
    else if(dowhat==ONADDNEWUSER)
    {
        QVector<User> tmpU;
        tmpU.push_back(forOne);
        messenger->addnewUser(tmpU);
    }
    ui->button_confirm->setDisabled(true);
}
void UserDialog::afterConfirmReply(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        if (dowhat == ONADDNEWUSER)
            QMessageBox::information(nullptr, QString("完成"), QString("新增用户成功"));
        else if(dowhat == ONCHANGEUSER)
            QMessageBox::information(nullptr, QString("完成"), QString("修改用户成功"));
        reply->deleteLater();
        allFinish=true;
        emit finishAll();
        this->close();
    } else {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败！\n请检查是否有名字重复用户或请稍后重试"));
        reply->deleteLater();
        ui->button_confirm->setDisabled(false);
    }
}
/*------------按钮响应end-------*/
