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

private:
    Ui::UserManager* ui;
    QVector<User> users;
    QStringList titles;

    void flushUsers();
    void setTable();
};

#endif // USERMANAGER_H
