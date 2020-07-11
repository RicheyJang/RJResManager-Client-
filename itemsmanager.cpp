#include "itemsmanager.h"
#include "ui_itemsmanager.h"

ItemsManager::ItemsManager(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::ItemsManager)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->tabWidget->setTabText(0, config.itemsList[0]);
    ui->tabWidget->setTabText(1, config.itemsList[1]);
    setWindowTitle(QString("库存管理"));
    QTableWidget* tab = ui->table_Out;
    tab->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab->setSelectionMode(QAbstractItemView::NoSelection);
    tab->setFocusPolicy(Qt::NoFocus);
    tab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tab->setSortingEnabled(true);
    tab = ui->table_Rent;
    tab->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab->setSelectionMode(QAbstractItemView::NoSelection);
    tab->setFocusPolicy(Qt::NoFocus);
    tab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tab->setSortingEnabled(true);
    ui->tabWidget->setCurrentIndex(0);
    titles.clear();
    titles << QString("物品编号") << QString("名称") << QString("型号") << QString("数量") << QString("单位");
    setTable(ui->table_Out, config.itemsList[0]);
}

ItemsManager::~ItemsManager()
{
    delete ui;
}

void ItemsManager::setTable(QTableWidget* tab, QString res)
{
    initResItems();

    tab->clear();
    tab->setColumnCount(titles.count());
    tab->setHorizontalHeaderLabels(titles);
    int cnt = 0;
    QSet<OneResItem> st = getResItemsByRes(res);
    for (OneResItem item : st) {
        cnt++;
        tab->setRowCount(cnt);
        tab->setItem(cnt - 1, 0, new QTableWidgetItem(QString::number(item.pid)));
        tab->setItem(cnt - 1, 1, new QTableWidgetItem(item.name));
        tab->setItem(cnt - 1, 2, new QTableWidgetItem(item.type));
        tab->setItem(cnt - 1, 3, new QTableWidgetItem(QString::number(item.cnt, 'g', 3)));
        tab->setItem(cnt - 1, 4, new QTableWidgetItem(item.units));
    }
}

void ItemsManager::on_tabWidget_currentChanged(int index)
{
    if (index == 0)
        setTable(ui->table_Out, config.itemsList[0]);
    else if (index == 1)
        setTable(ui->table_Rent, config.itemsList[1]);
}
