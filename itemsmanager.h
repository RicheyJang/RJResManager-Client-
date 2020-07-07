#ifndef ITEMSMANAGER_H
#define ITEMSMANAGER_H

#include "maininclude.h"
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

private:
    Ui::ItemsManager* ui;
    QStringList titles;
    void setTable(QTableWidget* table, QString res);
};

#endif // ITEMSMANAGER_H
