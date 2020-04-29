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
    void on_newOrder_clicked();
    void initMainWindow();

private:
    Login* login;
    Ui::MainWindow* ui;
};

#ifndef ISMAINCPP
extern MainWindow* mainWindow;
#endif

#endif // MAINWINDOW_H
