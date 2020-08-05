#include "neworder.h"
#include "ui_neworder.h"

NewOrder::NewOrder(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::NewOrder)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, true);
    messenger=new Messenger();

    ui->itemTable->setColumnCount(6);
    QStringList titles;
    titles << QString("pid") << QString("品类") << QString("名称") << QString("型号") << QString("数量") << QString("备注");
    ui->itemTable->setHorizontalHeaderLabels(titles);
    flushBox(1);
}

NewOrder::~NewOrder()
{
    delete ui;
}

void NewOrder::addnewOrder()
{
    dowhat = ONADDNEWORDER;
    connect(messenger,&Messenger::gotResponse,this,&NewOrder::afterConfirmReply);
    QString wkshop = QString("所用车间：") + thisUser.workshop;
    ui->workshopLabel->setText(wkshop);
    setWindowTitle(QString("发起新订单"));
}

void NewOrder::addnewOrder_WithModel(OneOrder order)
{
    addnewOrder();
    setOrder(order);
}

void NewOrder::changeOrder(OneOrder* order) //修改订单 窗口
{
    dowhat = ONCHANGEORDER;
    connect(messenger,&Messenger::gotResponse,this,&NewOrder::afterConfirmReply);
    theOrder = order;
    setOrder(*order);
    QString title = QString("修改订单 编号：") + QString::number(order->id);
    setWindowTitle(title);
}

void NewOrder::showOrder(OneOrder order) //展示订单 窗口
{
    dowhat = ONSHOWORDER;
    QLayoutItem* child;
    while ((child = ui->Layout_add->itemAt(0)) != nullptr) {
        ui->Layout_add->removeItem(child);
        delete child->widget();
        delete child;
        child = nullptr;
    }
    while ((child = ui->Layout_change->itemAt(0)) != nullptr) {
        ui->Layout_change->removeItem(child);
        delete child->widget();
        delete child;
        child = nullptr;
    }
    while ((child = ui->Layout_conferm->itemAt(0)) != nullptr) {
        ui->Layout_conferm->removeItem(child);
        delete child->widget();
        delete child;
        child = nullptr;
    }
    ui->classEdit->setReadOnly(true);
    ui->moreEdit->setReadOnly(true);
    setOrder(order);
    QString title = QString("当前订单 编号：") + QString::number(order.id);
    setWindowTitle(title);
}
/*----------辅助函数--------------*/
void NewOrder::setOrder(OneOrder order)
{
    for (OneItem item : order.items) {
        addOneItem(item);
    }
    ui->classEdit->setText(order.useclass);
    ui->moreEdit->setText(order.more);
    QString wkshop = QString("所用车间：") + order.workshop;
    ui->workshopLabel->clear();
    ui->workshopLabel->setText(wkshop);
}

void NewOrder::addOneItem(OneItem item)
{
    int row = ui->itemTable->rowCount();
    ui->itemTable->setRowCount(row + 1);
    QTableWidgetItem* pid = new QTableWidgetItem(QString::number(item.pid));
    ui->itemTable->setItem(row, 0, pid);
    OneResItem tmp = getResItem(item.pid);
    QTableWidgetItem* res = new QTableWidgetItem(tmp.res);
    QTableWidgetItem* name = new QTableWidgetItem(tmp.name);
    QTableWidgetItem* type = new QTableWidgetItem(tmp.type);
    QString u = QString::number(item.number) + " " + tmp.units;
    QTableWidgetItem* num = new QTableWidgetItem(u);
    QTableWidgetItem* more = new QTableWidgetItem(item.more);
    pid->setFlags(pid->flags() & (~Qt::ItemIsEditable));
    res->setFlags(res->flags() & (~Qt::ItemIsEditable));
    name->setFlags(name->flags() & (~Qt::ItemIsEditable));
    type->setFlags(type->flags() & (~Qt::ItemIsEditable));
    num->setFlags(num->flags() & (~Qt::ItemIsEditable));
    more->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
    ui->itemTable->setItem(row, 1, res);
    ui->itemTable->setItem(row, 2, name);
    ui->itemTable->setItem(row, 3, type);
    ui->itemTable->setItem(row, 4, num);
    ui->itemTable->setItem(row, 5, more);
}

void NewOrder::flushBox(int index)
{
    //QSet<QString> set;
    if (index == 1) {
        //set = getResSet();
        /*for (OneType atype : allType) {
            set.insert(atype.res);
        }*/
        //ui->resBox->setStringList(QStringList::fromSet(set));
        QStringList sl = getResList();
        ui->resBox->setStringList(sl);
        return;
    } else if (index == 2) {
        QString res = ui->resBox->currentText();
        //set = getNameSet(res);
        /*for (OneType atype : allType) {
            if (res.compare(atype.res) == 0) {
                set.insert(atype.name);
            }
        }*/
        //ui->nameBox->setStringList(QStringList::fromSet(set));
        QStringList sl = getNameList(res);
        ui->nameBox->setStringList(sl);
        return;
    } else if (index == 3) {
        QString res = ui->resBox->currentText();
        QString name = ui->nameBox->currentText();
        //set = getTypeSet(res, name);
        /*for (OneType atype : allType) {
            if (res.compare(atype.res) == 0 && name.compare(atype.name) == 0) {
                set.insert(atype.type);
            }
        }*/
        //ui->typeBox->setStringList(QStringList::fromSet(set));
        QStringList sl = getTypeList(res, name);
        ui->typeBox->setStringList(sl);
    }
    //    qDebug() << QString::number(index) + " flush";
}
/*----------辅助函数end----------*/

/*-------按钮响应---------*/
void NewOrder::on_addItem_clicked()
{
    QString nums = ui->numEdit->text();
    double num = nums.toDouble();
    if (num <= 0) {
        QMessageBox::information(nullptr, QString("错误"), QString("请正确填入物品数量"));
        return;
    }
    QString res = ui->resBox->currentText();
    QString name = ui->nameBox->currentText();
    QString type = ui->typeBox->currentText();
    int pid = getResItemPid(res, name, type);
    if (pid == 0) {
        QMessageBox::information(nullptr, QString("错误"), QString("查无此物，请正确填入物品信息"));
        return;
    }
    OneItem item;
    item.pid = pid;
    item.number = num;
    item.more = ui->itemMoreEdit->text();
    addOneItem(item);
}

void NewOrder::on_changeItem_clicked()
{
    int row = ui->itemTable->currentRow();
    QString res = ui->itemTable->item(row, 1)->text();
    QString name = ui->itemTable->item(row, 2)->text();
    QString type = ui->itemTable->item(row, 3)->text();
    QString num = ui->itemTable->item(row, 4)->text().split(" ")[0];
    QString more = ui->itemTable->item(row, 5)->text();
    ui->resBox->setCurrentText(res);
    ui->nameBox->setCurrentText(name);
    ui->typeBox->setCurrentText(type);
    ui->numEdit->setText(num);
    ui->itemMoreEdit->setText(more);

    int rowCnt = ui->itemTable->rowCount();
    ui->itemTable->removeRow(row);
    ui->itemTable->setRowCount(rowCnt - 1);
}

void NewOrder::on_deleteItem_clicked()
{
    int row = ui->itemTable->rowCount();
    int cr = ui->itemTable->currentRow();
    ui->itemTable->removeRow(cr);
    ui->itemTable->setRowCount(row - 1);
}

void NewOrder::on_resBox_currentTextChanged(const QString& arg1)
{
    flushBox(2);
}

void NewOrder::on_nameBox_currentTextChanged(const QString& arg1)
{
    flushBox(3);
}

void NewOrder::on_typeBox_currentTextChanged(const QString& arg1)
{
    QString u = getUnits(ui->resBox->currentText(), ui->nameBox->currentText(), ui->typeBox->currentText());
    if (u != nullptr) {
        ui->units->setText(QString("单位(") + u + ")");
    }
}

void NewOrder::on_confirm_clicked()
{
    if (dowhat != ONADDNEWORDER && dowhat != ONCHANGEORDER)
        return;
    QJsonObject json;
    QJsonObject orderInf;
    QJsonObject userInf;
    userInf.insert("username", thisUser.username);
    userInf.insert("password", thisUser.password);

    if (dowhat == ONCHANGEORDER)
        orderInf.insert("id", QJsonValue(theOrder->id));
    orderInf.insert("useclass", QJsonValue(ui->classEdit->text()));
    if (dowhat == ONADDNEWORDER)
        orderInf.insert("starttime", QJsonValue(QDate::currentDate().toString("yyyy-MM-dd")));
    orderInf.insert("more", QJsonValue(ui->moreEdit->toPlainText()));
    //WARNING 订单所属车间的添加 暂未考虑教师的多身份性
    orderInf.insert("workshop", QJsonValue(thisUser.workshop));

    QJsonArray items;
    int row = ui->itemTable->rowCount();
    for (int i = 0; i < row; i++) {
        QJsonObject item;
        item.insert("pid", QJsonValue(ui->itemTable->item(i, 0)->text().toInt()));
        QString s = ui->itemTable->item(i, 4)->text().split(" ")[0];
        item.insert("cnt", QJsonValue(s.toDouble()));
        item.insert("more", QJsonValue(ui->itemTable->item(i, 5)->text()));
        items.append(QJsonValue(item));
    }
    if (row == 0) {
        QMessageBox::StandardButton result = QMessageBox::question(nullptr, QString("确定？"), QString("订单物品栏为空，确定继续？"));
        if (result != QMessageBox::Yes)
            return;
    }

    json.insert("userInformation", QJsonValue(userInf));
    json.insert("orderInformation", QJsonValue(orderInf));
    json.insert("itemsInformation", QJsonValue(items));

    if (dowhat == ONADDNEWORDER)
        messenger->newOrder(json);
    else if (dowhat == ONCHANGEORDER)
        messenger->changeOrder(json);
    else
        return;
    ui->confirm->setDisabled(true);
}

void NewOrder::on_cancel_clicked()
{
    QMessageBox::StandardButton result = QMessageBox::question(nullptr, QString("确定？"), QString("是否放弃本订单"));
    if (result == QMessageBox::Yes)
        this->close();
    return;
}
/*-------按钮响应end-------*/

/*-------上传函数-------*/
void NewOrder::afterConfirmReply(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        if (dowhat == ONADDNEWORDER)
            QMessageBox::information(nullptr, QString("完成"), QString("发起订单成功"));
        else if(dowhat == ONCHANGEORDER)
            QMessageBox::information(nullptr, QString("完成"), QString("修改订单成功"));
        reply->deleteLater();
        this->close();
        emit finishChange();
    } else {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        reply->deleteLater();
        ui->confirm->setDisabled(false);
    }
}
void NewOrder::finishPost(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        if (dowhat == ONADDNEWORDER)
            QMessageBox::information(nullptr, QString("完成"), QString("发起订单成功"));
        else if(dowhat == ONCHANGEORDER)
            QMessageBox::information(nullptr, QString("完成"), QString("修改订单成功"));
        reply->deleteLater();
        this->close();
        emit finishChange();
    } else {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        reply->deleteLater();
    }
}
/*-------上传函数end-------*/
