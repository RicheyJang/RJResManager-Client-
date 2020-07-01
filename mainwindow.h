#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "login.h"
#include "maininclude.h"
#include "neworder.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    void setLoginWindow(Login* login);
    ~MainWindow();

private slots:
    void initMainWindow();

    void on_newOrder_clicked();
    void on_showDeal_clicked();
    void on_showNow_clicked();

    void changeThisOrder();
    void agreeHeader();
    void disagreeHeader();

    void finishPost(QNetworkReply* reply);

private:
    Login* login;
    Ui::MainWindow* ui;
    QPushButton* dealButton[10]={nullptr};
    void setDealButton();
    bool changeStatus(int orderID,QString toStatus);
    void postOn(QJsonObject json);
};

#ifndef ISMAINCPP
extern MainWindow* mainWindow;
#endif

#endif // MAINWINDOW_H
