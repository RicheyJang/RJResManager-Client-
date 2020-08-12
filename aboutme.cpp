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
    titles << QString("ID") << QString("用户名") << QString("真实姓名") << QString("密码") << QString("身份") << QString("所属车间");

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
    needChangePass=false;
    QPushButton* button=new QPushButton(QString("修改密码"));
    tab->setCellWidget(3,1,button);
    connect(button,&QPushButton::clicked,this,&AboutMe::whenNeedChangePass);
    tab->setItem(4, 1, new QTableWidgetItem(thisUser.identity));
    tab->setItem(5, 1, new QTableWidgetItem(thisUser.workshop));
}

/*-----------槽函数---------------*/
void AboutMe::whenNeedChangePass()
{
    if(!needChangePass)
    {
        QLabel* label1=new QLabel(QString("旧密码："));
        QLabel* label2=new QLabel(QString("新密码："));
        QLabel* label3=new QLabel(QString("重复新密码："));
        ui->layout_changePass->addWidget(label1,0,0);
        ui->layout_changePass->addWidget(label2,1,0);
        ui->layout_changePass->addWidget(label3,2,0);
        oldPassEdit=new QLineEdit();
        newPassEdit=new QLineEdit();
        newPassAgEdit=new QLineEdit();
        oldPassEdit->setMinimumHeight(40);
        newPassEdit->setMinimumHeight(40);
        newPassAgEdit->setMinimumHeight(40);
        oldPassEdit->setEchoMode(QLineEdit::Password);
        newPassEdit->setEchoMode(QLineEdit::Password);
        newPassAgEdit->setEchoMode(QLineEdit::Password);
        ui->layout_changePass->addWidget(oldPassEdit,0,1);
        ui->layout_changePass->addWidget(newPassEdit,1,1);
        ui->layout_changePass->addWidget(newPassAgEdit,2,1);
        tipLabel=new QLabel("");
        ui->layout_changePass->addWidget(tipLabel,3,0,1,2);
        needChangePass=true;
        resize(this->width(),this->height()+150);
    }
}

void AboutMe::on_conferm_clicked()
{
    if(needChangePass)
    {
        QString pass[3];
        pass[0]=oldPassEdit->text();
        if(toSHA256(pass[0])!=thisUser.password)
        {
            QString tips=QString("密码错误");
            tipLabel->setText(tips);
            tipLabel->setStyleSheet("color:red");
            return;
        }
        pass[1]=newPassEdit->text();
        if(!passwordCheck(pass[1]))
        {
            QString tips=QString("新密码格式错误\n正确：6~20位英文字母或数字或._");
            tipLabel->setText(tips);
            tipLabel->setStyleSheet("color:red");
            return;
        }
        pass[2]=newPassAgEdit->text();
        if(pass[1]!=pass[2])
        {
            QString tips=QString("两次输入的密码不相同");
            tipLabel->setText(tips);
            tipLabel->setStyleSheet("color:red");
            return;
        }

        QJsonObject json;
        QJsonObject appInf;
        QJsonObject userInf;
        userInf.insert("username", thisUser.username);
        userInf.insert("password", thisUser.password);

        appInf.insert("id",thisUser.id);
        appInf.insert("password",toSHA256(pass[1]));

        json.insert("userInformation", QJsonValue(userInf));
        json.insert("applicantInformation", QJsonValue(appInf));

        QString tips=QString("请稍候...");
        tipLabel->setText(tips);
        tipLabel->setStyleSheet("color:yellow");
        Messenger* messenger=new Messenger();
        messenger->changeUser(json);
        connect(messenger,&Messenger::gotResponse,this,&AboutMe::getChangeReply);
        ui->conferm->setDisabled(true);
        return;
    }
    else
        this->close();
}

void AboutMe::getChangeReply(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QString tips=QString("修改密码完成，请立即重启本软件");
        tipLabel->setText(tips);
        tipLabel->setStyleSheet("color:green");
        QMessageBox::information(nullptr,QString("警告"),tips);
        reply->deleteLater();
        this->close();
    } else {
        QString tips=QString("修改密码失败");
        tipLabel->setText(tips);
        tipLabel->setStyleSheet("color:red");
        reply->deleteLater();
        ui->conferm->setDisabled(false);
    }
}

void AboutMe::closeEvent(QCloseEvent *event)
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
}
/*-----------槽函数end-------------*/
