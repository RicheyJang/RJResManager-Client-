#ifndef ITEMSMANAGER_H
#define ITEMSMANAGER_H

#include "maininclude.h"
#include "messenger.h"
#include <QMainWindow>

namespace Ui {
class ItemsManager;
}

class ItemsManager : public QMainWindow {
    Q_OBJECT

public:
    explicit ItemsManager(QWidget* parent = nullptr);
    ~ItemsManager();

private slots:
    void on_tabWidget_currentChanged(int index);

    bool changeToByID(QString dowhat, int i);
    void finishPost(QNetworkReply* reply);
    void afterChange();
    void agreeAdmin();
    void disagreeAdmin();
    void changeThisOrder();
    void agreeKeeper();
    void disagreeKeeper();

private:
    QPushButton* dealButton[10] = { nullptr };
    Messenger* messenger;
    Ui::ItemsManager* ui;
    QStringList titles;
    void setTable(QTableWidget* table, QString res);
    void setDealButton();
    bool changeStatus(int orderID, QString toStatus);
};

#endif // ITEMSMANAGER_H
