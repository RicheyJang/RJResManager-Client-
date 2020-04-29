#ifndef PAPERTABLE_H
#define PAPERTABLE_H

#include "maininclude.h"
#include <QWidget>

namespace Ui {
class PaperTable;
}

class PaperTable : public QWidget {
    Q_OBJECT

public:
    explicit PaperTable(QWidget* parent = nullptr);
    ~PaperTable();
    void setOrderVec(QVector<OneOrder>* _orders);
    void clear();
    void setTitles(QStringList);

private slots:
    void on_buttonLastPaper_clicked();

    void on_buttonJumpTo_clicked();

    void on_buttonNextPaper_clicked();

private:
    bool turnToPage(int index);
    int rowCnt;
    int colCnt;
    int currentPage;
    int onePageRows;
    int onwhich;
    Ui::PaperTable* ui;
    QVector<OneOrder>* orders;
};

#endif // PAPERTABLE_H
