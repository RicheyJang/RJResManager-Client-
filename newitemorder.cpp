#include "newitemorder.h"
#include "ui_newitemorder.h"

NewItemOrder::NewItemOrder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewItemOrder)
{
    ui->setupUi(this);
}

NewItemOrder::~NewItemOrder()
{
    delete ui;
}
