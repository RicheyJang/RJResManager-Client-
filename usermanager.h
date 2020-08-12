#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "maininclude.h"
#include <QMainWindow>

namespace Ui {
class UserManager;
}

class UserManager : public QMainWindow {
    Q_OBJECT

public:
    explicit UserManager(QWidget* parent = nullptr);
    ~UserManager();

private slots:
    void on_table_User_cellDoubleClicked(int row, int column);
    void on_button_addUser_clicked();
    void on_button_changeUser_clicked();
    void changeThisUser(int);
    void afterConfirmReply(QNetworkReply* reply);
    void on_button_addManyUser_clicked();

private:
    Ui::UserManager* ui;
    QVector<User> users;
    QStringList titles;

    void flushUsers();
    void setTable();
};

#endif // USERMANAGER_H
