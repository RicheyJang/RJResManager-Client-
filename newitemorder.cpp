#include "newitemorder.h"
#include "ui_newitemorder.h"

NewItemOrder::NewItemOrder(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::NewItemOrder)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle(QString("发起新入库账目"));
    dowhat = ONADDNEWORDER;
    ui->itemTable->setColumnCount(6);
    QStringList titles;
    titles << QString("pid") << QString("品类") << QString("名称") << QString("型号") << QString("数量") << QString("备注");
    ui->itemTable->setHorizontalHeaderLabels(titles);
    flushBox(1);
    messenger=new Messenger();
    connect(messenger,&Messenger::gotResponse,this,&NewItemOrder::afterConfirmReply);

    ui->resBox->setNotMustHint(true);
    ui->nameBox->setNotMustHint(true);
    ui->typeBox->setNotMustHint(true);
}

NewItemOrder::~NewItemOrder()
{
    delete ui;
}

void NewItemOrder::setOrder(OneResOrder order)
{
    for (OneNewItem item : order.newItems) {
        addOneItem(item);
    }
    ui->moreEdit->setText(order.more);
}

void NewItemOrder::setModel(OneResOrder order)
{
    setOrder(order);
}

void NewItemOrder::changeOrder(OneResOrder* order) //修改订单 窗口
{
    dowhat = ONCHANGEORDER;
    theOrder = order;
    setOrder(*order);
    QString title = QString("修改账目 编号：") + QString::number(order->id);
    setWindowTitle(title);
}
bool NewItemOrder::showOrder(int id) //展示订单 窗口
{
    QVector<OneResOrder>& orders=dealResOrders;
    OneResOrder order;
    order.id = id;
    QVector<OneResOrder>::iterator it = std::lower_bound(orders.begin(), orders.end(), order);
    if (it == orders.end() || it->id != id) {
        return false;
    }
    showOrder(*it);
    return true;
}
void NewItemOrder::showOrder(OneResOrder order) //展示订单 窗口
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
    ui->moreEdit->setReadOnly(true);
    setOrder(order);
    QString title = QString("当前账目 编号：") + QString::number(order.id);
    setWindowTitle(title);
}

/*----------辅助函数--------------*/
void NewItemOrder::addOneItem(OneNewItem item)
{
    QString pids=QString::number(item.pid);
    QTableWidgetItem* pid = new QTableWidgetItem(pids);
    QTableWidgetItem* res = new QTableWidgetItem(item.res);
    QTableWidgetItem* name = new QTableWidgetItem(item.name);
    QTableWidgetItem* type = new QTableWidgetItem(item.type);
    QString u = QString::number(item.number) + " " + item.units;
    QTableWidgetItem* num = new QTableWidgetItem(u);
    QTableWidgetItem* more = new QTableWidgetItem(item.more);

    pid->setFlags(pid->flags() & (~Qt::ItemIsEditable));
    res->setFlags(res->flags() & (~Qt::ItemIsEditable));
    name->setFlags(name->flags() & (~Qt::ItemIsEditable));
    type->setFlags(type->flags() & (~Qt::ItemIsEditable));
    num->setFlags(num->flags() & (~Qt::ItemIsEditable));
    more->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);

    int row = ui->itemTable->rowCount();
    ui->itemTable->setRowCount(row + 1);
    ui->itemTable->setItem(row, 0, pid);
    ui->itemTable->setItem(row, 1, res);
    ui->itemTable->setItem(row, 2, name);
    ui->itemTable->setItem(row, 3, type);
    ui->itemTable->setItem(row, 4, num);
    ui->itemTable->setItem(row, 5, more);
}

void NewItemOrder::flushBox(int index)
{
    if (index == 1) {
        QStringList sl = getResList();
        ui->resBox->setStringList(sl);
        return;
    } else if (index == 2) {
        QString res = ui->resBox->currentText();
        QStringList sl = getNameList(res);
        ui->nameBox->setStringList(sl);
        return;
    } else if (index == 3) {
        QString res = ui->resBox->currentText();
        QString name = ui->nameBox->currentText();
        QStringList sl = getTypeList(res, name);
        ui->typeBox->setStringList(sl);
    }
}
/*----------辅助函数end----------*/

/*-------按钮响应---------*/
void NewItemOrder::on_addItem_clicked()
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
    QString units = ui->unitsEdit->text();
    if(res.length()<=0 || name.length()<=0 || type.length()<=0 || units.length()<=0)
    {
        QMessageBox::information(nullptr, QString("错误"), QString("请正确填入物品信息"));
        return;
    }

    OneNewItem item;
    int pid = getResItemPid(res, name, type);
    item.isNew=pid==0;
    item.pid = pid;
    item.number = num;
    item.more = ui->itemMoreEdit->text();
    item.res=res;
    item.name=name;
    item.type=type;
    item.units=units;
    addOneItem(item);
}

void NewItemOrder::on_changeItem_clicked()
{
    int row = ui->itemTable->currentRow();
    QString res = ui->itemTable->item(row, 1)->text();
    QString name = ui->itemTable->item(row, 2)->text();
    QString type = ui->itemTable->item(row, 3)->text();
    QString num = ui->itemTable->item(row, 4)->text().split(" ")[0];
    QString units = ui->itemTable->item(row, 4)->text().split(" ")[1];
    QString more = ui->itemTable->item(row, 5)->text();
    ui->resBox->setCurrentText(res);
    ui->nameBox->setCurrentText(name);
    ui->typeBox->setCurrentText(type);
    ui->numEdit->setText(num);
    ui->itemMoreEdit->setText(more);
    ui->unitsEdit->setText(units);

    int rowCnt = ui->itemTable->rowCount();
    ui->itemTable->removeRow(row);
    ui->itemTable->setRowCount(rowCnt - 1);
}

void NewItemOrder::on_deleteItem_clicked()
{
    int row = ui->itemTable->rowCount();
    int cr = ui->itemTable->currentRow();
    ui->itemTable->removeRow(cr);
    ui->itemTable->setRowCount(row - 1);
}

void NewItemOrder::on_resBox_currentTextChanged(const QString& arg1)
{
    flushBox(2);
}

void NewItemOrder::on_nameBox_currentTextChanged(const QString& arg1)
{
    flushBox(3);
}

void NewItemOrder::on_typeBox_currentTextChanged(const QString& arg1)
{
    QString u = getUnits(ui->resBox->currentText(), ui->nameBox->currentText(), ui->typeBox->currentText());
    if (u != nullptr) {
        ui->unitsEdit->setText(u);
    }
}

void NewItemOrder::on_confirm_clicked()
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
    if (dowhat == ONADDNEWORDER)
        orderInf.insert("starttime", QJsonValue(QDate::currentDate().toString("yyyy-MM-dd")));
    orderInf.insert("more", QJsonValue(ui->moreEdit->toPlainText()));

    QJsonArray items;
    int row = ui->itemTable->rowCount();
    for (int i = 0; i < row; i++) {
        QJsonObject item;
        int pid=ui->itemTable->item(i, 0)->text().toInt();
        int tmpNew=(pid==0)? 1 : 0;
        item.insert("isNew",QJsonValue(tmpNew));
        item.insert("pid", QJsonValue(pid));
        item.insert("res", QJsonValue(ui->itemTable->item(i, 1)->text()));
        item.insert("name", QJsonValue(ui->itemTable->item(i, 2)->text()));
        item.insert("type", QJsonValue(ui->itemTable->item(i, 3)->text()));
        QString s = ui->itemTable->item(i, 4)->text().split(" ")[0];
        item.insert("cnt", QJsonValue(s.toDouble()));
        s = ui->itemTable->item(i, 4)->text().split(" ")[1];
        item.insert("units", QJsonValue(s));
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
        messenger->newResOrder(json);
    else if (dowhat == ONCHANGEORDER)
        messenger->changeResOrder(json);
    else
        return;
    ui->confirm->setDisabled(true);
}

void NewItemOrder::on_cancel_clicked()
{
    QMessageBox::StandardButton result = QMessageBox::question(nullptr, QString("确定？"), QString("是否放弃本订单"));
    if (result == QMessageBox::Yes)
        this->close();
    return;
}
/*-------按钮响应end-------*/

/*-------上传函数-------*/
void NewItemOrder::afterConfirmReply(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        if (dowhat == ONADDNEWORDER)
            QMessageBox::information(nullptr, QString("完成"), QString("发起账目成功"));
        else
            QMessageBox::information(nullptr, QString("完成"), QString("修改账目成功"));
        reply->deleteLater();
        this->close();
        emit finishChange();
    } else {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        reply->deleteLater();
        ui->confirm->setDisabled(false);
    }
}
/*-------上传函数end-------*/
