#include "neworder.h"
#include "ui_neworder.h"

NewOrder::NewOrder(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::NewOrder)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, true);
    isInNewOrder = true;
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

void NewOrder::setOrder(OneOrder order)
{
    for (OneItem item : order.items) {
        addOneItem(item);
    }
    ui->classEdit->setText(order.useclass);
    ui->moreEdit->setText(order.more);
}

void NewOrder::changeOrder(OneOrder* order)
{
    isInNewOrder = false;
    theOrder = order;
    setOrder(*order);
}

void NewOrder::setModel(OneOrder order)
{
    setOrder(order);
}

void NewOrder::addOneItem(OneItem item)
{
    int row = ui->itemTable->rowCount();
    ui->itemTable->setRowCount(row + 1);
    QTableWidgetItem* pid = new QTableWidgetItem(QString::number(item.pid));
    ui->itemTable->setItem(row, 0, pid);
    OneType tmp;
    tmp.pid = item.pid;
    QSet<OneType>::iterator it = allType.find(tmp);
    QTableWidgetItem* res = new QTableWidgetItem((*it).res);
    QTableWidgetItem* name = new QTableWidgetItem((*it).name);
    QTableWidgetItem* type = new QTableWidgetItem((*it).type);
    QString u = QString::number(item.number) + " " + (*it).units;
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
    int pid = getTypePid(res, name, type);
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

void NewOrder::on_deleteItem_clicked()
{
    int row = ui->itemTable->rowCount();
    int cr = ui->itemTable->currentRow();
    ui->itemTable->removeRow(cr);
    ui->itemTable->setRowCount(row - 1);
}

void NewOrder::flushBox(int index)
{
    QSet<QString> set;
    if (index == 1) {
        for (OneType atype : allType) {
            set.insert(atype.res);
        }
        ui->resBox->setStringList(QStringList::fromSet(set));
        return;
    } else if (index == 2) {
        QString res = ui->resBox->currentText();
        for (OneType atype : allType) {
            if (res.compare(atype.res) == 0) {
                set.insert(atype.name);
            }
        }
        ui->nameBox->setStringList(QStringList::fromSet(set));
        return;
    } else if (index == 3) {
        QString res = ui->resBox->currentText();
        QString name = ui->nameBox->currentText();
        for (OneType atype : allType) {
            if (res.compare(atype.res) == 0 && name.compare(atype.name) == 0) {
                set.insert(atype.type);
            }
        }
        ui->typeBox->setStringList(QStringList::fromSet(set));
    }
    qDebug() << QString::number(index) + " flush";
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
    if (isInNewOrder) {
        QJsonObject json;
        QJsonObject orderInf;
        QJsonObject userInf;
        userInf.insert("username", thisUser.username);
        userInf.insert("password", thisUser.password);

        orderInf.insert("useclass", QJsonValue(ui->classEdit->text()));
        orderInf.insert("starttime", QJsonValue(QDate::currentDate().toString("yyyy-MM-dd")));
        orderInf.insert("more", QJsonValue(ui->moreEdit->toPlainText()));

        QJsonArray items;
        int row = ui->itemTable->rowCount();
        for (int i = 0; i < row; i++) {
            QJsonObject item;
            item.insert("pid", QJsonValue(ui->itemTable->item(i, 0)->text().toInt()));
            QString s = ui->itemTable->item(i, 4)->text().split(" ")[0];
            item.insert("cnt", QJsonValue(s.toDouble()));
            item.insert("more", QJsonValue(ui->itemTable->item(i, 5)->text()));
            qDebug() << item.value("cnt");
            items.append(QJsonValue(item));
        }
        json.insert("userInformation", QJsonValue(userInf));
        json.insert("orderInformation", QJsonValue(orderInf));
        json.insert("itemsInformation", QJsonValue(items));

        postOn(json);
    }
}

void NewOrder::postOn(QJsonObject json)
{
    QJsonDocument document;
    document.setObject(json);

    QByteArray data = document.toJson(QJsonDocument::Indented);

    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QNetworkRequest* request = new QNetworkRequest;
    request->setHeader(QNetworkRequest::UserAgentHeader, config.UserAgent);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/json"); //上面语句固定这么写，要不然会报错“contest—type is missing”
    //request.setRawHeader("XXX3", "XXX4");

    QString url = QString("http://") + config.ip + ':' + QString::number(config.serverPort) + QString("/neworder");
    request->setUrl(QUrl(url));

    manager->post(*request, data);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishPost(QNetworkReply*)));
    connect(manager, &QNetworkAccessManager::finished, manager, &QNetworkAccessManager::deleteLater);
    delete request;
    request = nullptr;
}

void NewOrder::finishPost(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QMessageBox::information(nullptr, QString("完成"), QString("发起订单成功"));
        reply->deleteLater();
        this->close();
    } else {
        QMessageBox::warning(nullptr, QString("错误"), QString("发起订单失败"));
        reply->deleteLater();
    }
}

void NewOrder::on_cancel_clicked()
{
    QMessageBox::StandardButton result = QMessageBox::question(nullptr, QString("确定？"), QString("是否放弃本订单"));
    if (result == QMessageBox::Yes)
        this->close();
    return;
}
