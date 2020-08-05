#ifndef PAPERTABLE_H
#define PAPERTABLE_H

#include "maininclude.h"
#include "neworder.h"
#include "newitemorder.h"
#include <QWidget>

namespace Ui {
class PaperTable;
}

class PaperTable : public QWidget {
    Q_OBJECT

public:
    explicit PaperTable(QWidget* parent = nullptr);
    ~PaperTable();
    int onwhich;
    void setOrderTitle();
    void setOrderVec(QVector<OneOrder>* _orders);
    void setIsResOrder(bool isit=true);
    void clear();
    void setTitles(QStringList);
    void flush();
    int getCurrentID();
    QSet<int> getCurrentIDs();

signals:
    void finishFlush();

private slots:
    void afterFlush();

    void on_buttonLastPaper_clicked();
    void on_buttonJumpTo_clicked();
    void on_buttonNextPaper_clicked();
    void on_buttonFlush_clicked();
    void on_Table_cellDoubleClicked(int row, int column);

private:
    bool turnToPage(int index);
    int rowCnt;
    int colCnt;
    int currentPage;
    int onePageRows;
    bool isResOrder;
    Ui::PaperTable* ui;
    QVector<OneOrder>* orders;
    QStringList orderSL;
    //TODO 搜索模块待实现
};

#endif // PAPERTABLE_H
