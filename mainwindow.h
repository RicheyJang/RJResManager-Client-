#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "aboutme.h"
#include "itemsmanager.h"
#include "login.h"
#include "maininclude.h"
#include "neworder.h"
#include "usermanager.h"
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
    void flushMainWindow();

    void on_newOrder_clicked();
    void on_showDeal_clicked();
    void on_showNow_clicked();

    void changeThisOrder();
    void afterChange();
    bool changeToByID(QString dowhat, int i);
    void agreeTeacher();
    void disagreeTeacher();
    void agreeHeader();
    void disagreeHeader();
    void agreeAdmin();
    void disagreeAdmin();
    void agreeKeeper();
    void disagreeKeeper();
    void againOrder();

    void finishPost(QNetworkReply* reply);
    void on_about_me_triggered();
    void on_about_writer_triggered();
    void on_store_showAll_triggered();
    void on_showHistory_clicked();
    void on_people_change_triggered();

private:
    Login* login;
    Ui::MainWindow* ui;
    QPushButton* dealButton[10] = { nullptr };
    void setDealButton();
    void setNowButton();
    void setHistoryButton();
    bool changeStatus(int orderID, QString toStatus);
    bool finishOrder(int orderID);
    void postOn(QJsonObject json, QString uri);
};

#ifndef ISMAINCPP
extern MainWindow* mainWindow;
#endif

#endif // MAINWINDOW_H
