#include "papertable.h"
#include "ui_papertable.h"

PaperTable::PaperTable(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PaperTable)
{
    ui->setupUi(this);
    orderSL << QString("编号") << QString("发起日期") << QString("使用班级") << QString("所属车间") << QString("状态")
            << QString("备注") << QString("所含物品") << QString("教师") << QString("车间主任") << QString("实习科长")
            << QString("仓库管理员");
    onePageRows = config.TableOnePageRows;
    orders = nullptr;
    onwhich = ONDealOrder;
    ui->dateStart->setDate(config.orderStartDay);
    ui->dateEnd->setDate(QDate::currentDate());
    connect(this, &PaperTable::finishFlush, this, &PaperTable::afterFlush);
    setIsResOrder(false);
    clear();
}

PaperTable::~PaperTable()
{
    delete ui;
}

/*-------外界函数-----------*/
int PaperTable::getCurrentID()
{
    int row = ui->Table->currentRow();
    QTableWidgetItem* item = ui->Table->item(row, 0);
    if (item == nullptr)
        return 0;
    QString nums = item->text();
    return nums.toInt();
}
QSet<int> PaperTable::getCurrentIDs()
{
    QSet<int> vecItemIndex; //保存选中行的索引
    QItemSelectionModel* selections = ui->Table->selectionModel(); //返回当前的选择模式
    QModelIndexList selectedsList = selections->selectedRows(); //返回所有选定的模型项目索引列表
    for (int i = 0; i < selectedsList.count(); i++) {
        int row = selectedsList.at(i).row();
        QString nums = ui->Table->item(row, 0)->text();
        vecItemIndex.insert(nums.toInt());
    }
    return vecItemIndex;
}
void PaperTable::clear()
{
    ui->Table->clear();
    rowCnt = 0;
    colCnt = 0;
    currentPage = 1;
    ui->editPaper->clear();
    ui->editPaper->setText(QString::number(1));
    ui->lablePaperSum->setText(QString("共1页"));
}
void PaperTable::setOrderVec(QVector<OneOrder>* _orders)
{
    this->clear();
    orders = _orders;
    rowCnt = orders->size();
    setOrderTitle();
    if (orders == &historys)
        onwhich = ONHistory;
    else if (orders == &noworders)
        onwhich = ONNowOrder;
    else
        onwhich = ONDealOrder;
    currentPage = 1;
    on_buttonFlush_clicked();
}
void PaperTable::setOrderTitle()
{
    setTitles(orderSL);
}
void PaperTable::flush()
{
    on_buttonFlush_clicked();
}
/*-------外界函数end---------*/

/*-------表格整理函数-----------*/
void PaperTable::setIsResOrder(bool isit)
{
    isResOrder=isit;
    if(isit)
    {
        orderSL.clear();
        orderSL << QString("编号") << QString("发起日期") << QString("状态")
                << QString("备注") << QString("所含物品") << QString("实习科长")
                << QString("仓库管理员");
        setTitles(orderSL);
    }
}

void PaperTable::setTitles(QStringList sl)
{
    colCnt = sl.count();
    ui->Table->setColumnCount(colCnt);
    ui->Table->setHorizontalHeaderLabels(sl);
}

void PaperTable::afterFlush()
{
    turnToPage(currentPage);
    ui->buttonFlush->setDisabled(false);
    ui->buttonFlush->setEnabled(true);
}

bool PaperTable::turnToPage(int index)
{
    if (orders != nullptr)
        rowCnt = orders->size();
    else
        return false;
    if (rowCnt == 0) {
        ui->Table->clearContents();
        ui->Table->setRowCount(0);
        currentPage = 1;
        ui->editPaper->setText(QString::number(currentPage));
        int pageCnt = 1;
        ui->lablePaperSum->setText(QString("共") + QString::number(pageCnt) + QString("页"));
        return true;
    }
    if (rowCnt < 0 || index <= 0 || (rowCnt - 1) / onePageRows + 1 < index)
        return false;
    index--;
    int st = index * onePageRows;
    int ed = qMin(index * onePageRows + 9, rowCnt - 1);
    if (ed < st)
        return false;
    ui->Table->clearContents();
    ui->Table->setRowCount(ed - st + 1);
    QTableWidgetItem* witem;
    for (int i = 0; i < ed - st + 1; i++) {
        for (int j = 0; j < colCnt; j++) {
            if (ui->Table->item(i, j) == nullptr) {
                witem = new QTableWidgetItem("");
                ui->Table->setItem(i, j, witem);
            }
        }
    }
    int k = 0;
    for (int i = st; i <= ed; i++, k++) {
        const OneOrder* order = &(orders->at(i));
        if(!isResOrder)
        {
            ui->Table->item(k, 0)->setText(QString::number(order->id));
            ui->Table->item(k, 1)->setText(order->starttime.toString("yyyy/MM/dd"));
            ui->Table->item(k, 2)->setText(order->useclass);
            ui->Table->item(k, 3)->setText(order->workshop);
            ui->Table->item(k, 4)->setText(order->status);
            ui->Table->item(k, 5)->setText(order->more);
            QString s = "";
            for (int j = 0; j < qMin(3, order->items.size()); j++) {
                OneResItem type = getResItem(order->items.at(j).pid);
                if (type.pid == 0)
                    continue;
                s = s + type.name + "(" + type.type + ") ";
            }
            if (order->items.size() == 0)
                s = QString("无物品");
            else
                s = s + QString("等");
            ui->Table->item(k, 6)->setText(s);
            ui->Table->item(k, 7)->setText(order->teacher);
            ui->Table->item(k, 8)->setText(order->header);
            ui->Table->item(k, 9)->setText(order->admin);
            ui->Table->item(k, 10)->setText(order->keeper);
        }
        else
        {
            ui->Table->item(k, 0)->setText(QString::number(order->id));
            ui->Table->item(k, 1)->setText(order->starttime.toString("yyyy/MM/dd"));
            ui->Table->item(k, 2)->setText(order->status);
            ui->Table->item(k, 3)->setText(order->more);
            QString s = "";
            for (int j = 0; j < qMin(3, order->newItems.size()); j++) {
                s = s + order->newItems.at(j).name + "(" + order->newItems.at(j).type + ") ";
            }
            if (order->newItems.size() == 0)
                s = QString("无物品");
            else
                s = s + QString("等");
            ui->Table->item(k, 4)->setText(s);
            ui->Table->item(k, 5)->setText(order->admin);
            ui->Table->item(k, 6)->setText(order->keeper);
        }
    }
    currentPage = index + 1;
    ui->editPaper->setText(QString::number(currentPage));
    int pageCnt = (rowCnt - 1) / onePageRows + 1;
    ui->lablePaperSum->setText(QString("共") + QString::number(pageCnt) + QString("页"));
    return true;
}
/*-------表格整理函数end---------*/

/*-------按钮响应函数------------*/
void PaperTable::on_buttonLastPaper_clicked()
{
    turnToPage(currentPage - 1);
    ui->editPaper->setText(QString::number(currentPage));
}

void PaperTable::on_buttonJumpTo_clicked()
{
    int page = ui->editPaper->text().toInt();
    turnToPage(page);
}

void PaperTable::on_buttonNextPaper_clicked()
{
    turnToPage(currentPage + 1);
}

void PaperTable::on_buttonFlush_clicked()
{
    ui->buttonFlush->setDisabled(true);
    QDate start = ui->dateStart->date();
    QDate end = ui->dateEnd->date();
    if(!isResOrder)
    {
        if (onwhich == ONNowOrder)
            flushNowOrders(start, end);
        else if (onwhich == ONHistory)
            flushHistoryOrders(start, end);
        else
            flushDealOrders(start, end);
    }
    else
    {
        if (onwhich == ONHistory)
        {
            //TODO flushHistoryOrders(start, end);
        }
        else
            flushDealResOrders(start, end);
    }
    emit finishFlush();
}

void PaperTable::on_Table_cellDoubleClicked(int row, int column)
{
    int id = getCurrentID();
    if (id == 0)
        return;
    if(!isResOrder)
    {
        OneOrder* order = getOrder(id, dealorders);
        if (order == nullptr)
            order = getOrder(id, noworders);
        if (order == nullptr)
            order = getOrder(id, historys);
        if (order == nullptr)
            return;
        NewOrder* newOrder = new NewOrder();
        newOrder->showOrder(*order);
        newOrder->show();
    }
    else
    {
        NewItemOrder* nio=new NewItemOrder();
        if(nio->showOrder(id))
            nio->show();
        else
            delete nio;
    }
}
/*-------按钮响应函数end----------*/
