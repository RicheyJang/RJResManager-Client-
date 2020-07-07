#ifndef NEWITEMORDER_H
#define NEWITEMORDER_H

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

private:
    Ui::NewItemOrder *ui;
};

#endif // NEWITEMORDER_H
