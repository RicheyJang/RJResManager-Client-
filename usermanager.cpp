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

/*----------界面设置--------------*/
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
/*----------界面设置end------------*/

/*----------按钮响应---------------*/
void UserManager::changeThisUser(int row)
{
    int id=ui->table_User->item(row,0)->text().toInt();
    if(id==0)
        return;

    User *user=getUserByID(id,&users);
    if(user==nullptr)
        return;

    UserDialog* UD=new UserDialog();
    UD->setChangeUser(*user);
    connect(UD,&UserDialog::finishAll,this,&UserManager::flushUsers);
    UD->show();
}
void UserManager::on_table_User_cellDoubleClicked(int row, int column)
{
    changeThisUser(row);
}

void UserManager::on_button_changeUser_clicked()
{
    int row=ui->table_User->currentRow();
    changeThisUser(row);
}

void UserManager::on_button_addUser_clicked()
{
    UserDialog* UD=new UserDialog();
    UD->setAddNewUser();
    connect(UD,&UserDialog::finishAll,this,&UserManager::flushUsers);
    UD->show();
}

void UserManager::on_button_addManyUser_clicked()
{
    QString filename=QFileDialog::getOpenFileName(nullptr, QString("选择文件"), "/", QString("Exel file(*.xlsx)"));
    if(!(filename!=nullptr && filename.endsWith("xlsx")))
        return ;
    QXlsx::Document xlsxx(filename);
    xlsxx.selectSheet("Sheet1");
    QXlsx::CellRange range;
    int rowCount = xlsxx.dimension().rowCount();//获取行数
    int colCount = xlsxx.dimension().columnCount();
    if(colCount!=4)
    {
        QMessageBox::warning(nullptr,QString("错误"),QString("文件格式有误，无法读取！"));
        return;
    }
    QVector<User> apps;

    for(int i = 2; i <= rowCount; i ++)
    {
        User appOne;
        appOne.username=xlsxx.read(i,1).toString();
        appOne.truename=xlsxx.read(i,2).toString();
        appOne.workshop=xlsxx.read(i,3).toString();
        QString iden=xlsxx.read(i,4).toString();
        int num=-1,cnt=config.userIdentityList.count();
        for(int k=0;k<cnt;k++)
        {
            if(iden==config.userIdentityList[k] || iden==config.userCNIdentityList[k])
            {
                num=k;
                break;
            }
        }
        if(num<0 || num>=cnt) continue;
        appOne.identity=config.userIdentityList[num];
        appOne.password=nullptr;
        appOne.isUseful=true;
        if(userCheck(appOne))
            apps.push_back(appOne);
    }

    ui->button_addManyUser->setDisabled(true);
    Messenger* messenger=new Messenger();
    messenger->addnewUser(apps);
    connect(messenger,&Messenger::gotResponse,this,&UserManager::afterConfirmReply);
    return;
}

void UserManager::afterConfirmReply(QNetworkReply* reply)
{
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &jsonError);
    QJsonObject json;
    if (reply->error() == QNetworkReply::NoError && !document.isNull() && document.isObject() && jsonError.error == QJsonParseError::NoError) {
        json = document.object();
        int sNum=json.value("successNum").toInt();
        QMessageBox::information(nullptr, QString("完成"), QString("成功添加用户：%0个").arg(sNum));
        reply->deleteLater();
    } else {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败！\n请检查是否有名字重复用户或请稍后重试"));
        reply->deleteLater();
    }
    ui->button_addManyUser->setDisabled(false);
}
