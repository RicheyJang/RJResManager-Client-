#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setLoginWindow(Login* _login)
{
    this->login = _login;
    connect(_login, &Login::allSuccess, this, &MainWindow::initMainWindow);
    connect(_login, &Login::suddenClose, this, &MainWindow::close);
}

void MainWindow::initMainWindow()
{
    ui->orderTable->setOrderVec(&dealorders);
    this->show();
}

//以下为测试所用
void MainWindow::on_newOrder_clicked()
{
    NewOrder* order = new NewOrder();
    order->show();
}
