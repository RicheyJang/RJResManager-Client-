#ifndef NEWITEMORDER_H
#define NEWITEMORDER_H

#include "maininclude.h"
#include "messenger.h"
#include <QWidget>

namespace Ui {
class NewItemOrder;
}

class NewItemOrder : public QWidget
{
    Q_OBJECT

public:
    explicit NewItemOrder(QWidget *parent = nullptr);
    ~NewItemOrder();
    void changeOrder(OneResOrder* order); //修改某订单（订单id不变）
    void setModel(OneResOrder order); //作为模板，新建订单（新建id）
    bool showOrder(int id);
    void showOrder(OneResOrder order);
signals:
    void finishChange();

private slots:
    void afterConfirmReply(QNetworkReply*);

    void on_addItem_clicked();
    void flushBox(int);
    void on_resBox_currentTextChanged(const QString& arg1);
    void on_nameBox_currentTextChanged(const QString& arg1);
    void on_typeBox_currentTextChanged(const QString& arg1);
    void on_deleteItem_clicked();
    void on_cancel_clicked();
    void on_confirm_clicked();
    void on_changeItem_clicked();
    void on_button_readExcel_clicked();

private:
    void addOneItem(OneNewItem item);
    void setOrder(OneResOrder order);
    void postOn(QJsonObject json);

    int dowhat;
    bool isInAddItem = false;
    OneResOrder* theOrder = nullptr;
    QPushButton* confirm;
    Messenger* messenger;
    Ui::NewItemOrder *ui;
};

#endif // NEWITEMORDER_H
