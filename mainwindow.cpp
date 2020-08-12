#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setLoginWindow(Login* _login)
{
    this->login = _login;
    connect(_login, &Login::allSuccess, this, &MainWindow::initMainWindow);
    connect(_login, &Login::suddenClose, this, &MainWindow::close);
}

void MainWindow::flushMainWindow()
{
    if (thisUser.identity != QString("admin") && thisUser.identity != QString("keeper") && thisUser.identity != QString("accountant"))
        ui->store->menuAction()->setVisible(false);
    else {
        ui->store->menuAction()->setVisible(true);
        initNewItems();
        if (dealResOrders.size() > 0) {
            QString tips = QString("<font color=green>您有新的待处理<font color=red>入库账目</font>，请及时处理</font>");
            errorW->setTipInfo(tips);
            errorW->show();
        }
    }
    if (thisUser.identity != QString("admin"))
        ui->people->menuAction()->setVisible(false);
    else
        ui->people->menuAction()->setVisible(true);
    if (thisUser.identity != QString("teacher"))
        ui->newOrder->setDisabled(true);
    else
        ui->newOrder->setDisabled(false);
    on_showDeal_clicked();
}

void MainWindow::initMainWindow()
{
    setWindowTitle(config.softName);
    errorW = new ErrorWidget(this);
    errorW->hide();
    ui->orderTable->setOrderTitle();
    for (int i = 0; i < 10; i++)
        dealButton[i] = nullptr;

    flushMainWindow();

    if(thisUser.identity == QString("keeper"))
    {
        QAction* newItemAction=new QAction(QString("物品入库"));
        connect(newItemAction,&QAction::triggered,this,&MainWindow::on_newItemOrder);
        ui->store->addAction(newItemAction);
        QAction* newItemTemplateAction=new QAction(QString("生成添加物品模板..."));
        connect(newItemTemplateAction,&QAction::triggered,this,&MainWindow::on_newItemOrderTemplate);
        ui->store->addAction(newItemTemplateAction);
    }

    messenger=new Messenger();
    connect(messenger,&Messenger::gotResponse,this,&MainWindow::finishPost);
    on_showDeal_clicked();
    this->show();
}

/*-----------顶部按钮--------------*/
void MainWindow::on_newOrder_clicked()
{
    NewOrder* order = new NewOrder();
    order->addnewOrder();
    order->show();
}

void MainWindow::on_showDeal_clicked()
{
    ui->orderTable->setOrderVec(&dealorders);
    setDealButton();
}

void MainWindow::on_showNow_clicked()
{
    ui->orderTable->setOrderVec(&noworders);
    setNowButton();
}

void MainWindow::on_showHistory_clicked()
{
    ui->orderTable->setOrderVec(&historys);
    setHistoryButton();
}
/*-----------顶部按钮end------------*/

/*-----------底部按钮--------------*/
void MainWindow::setHistoryButton()
{
    int num = 0;
    QLayoutItem* child;
    while ((child = ui->buttomLayout->itemAt(0)) != nullptr) {
        ui->buttomLayout->removeItem(child);
        delete child->widget();
        delete child;
        child = nullptr;
    }
    for (int i = 0; i < 10; i++)
        dealButton[i] = nullptr;
    if (thisUser.identity == QString("teacher")) {
        dealButton[1] = new QPushButton(QString("重新发起该订单"));
        connect(dealButton[1], &QPushButton::clicked, this, &MainWindow::againOrder);
        num = 1;
    }
    ui->buttomLayout->addStretch();
    for (int i = 1; i <= num; i++) {
        dealButton[i]->setMinimumHeight(35);
        ui->buttomLayout->addWidget(dealButton[i]);
    }
}
void MainWindow::setNowButton()
{
    int num = 0;
    QLayoutItem* child;
    while ((child = ui->buttomLayout->itemAt(0)) != nullptr) {
        ui->buttomLayout->removeItem(child);
        delete child->widget();
        delete child;
        child = nullptr;
    }
    for (int i = 0; i < 10; i++)
        dealButton[i] = nullptr;
    if (thisUser.identity == QString("teacher")) {
        dealButton[1] = new QPushButton(QString("撤回订单"));
        connect(dealButton[1], &QPushButton::clicked, this, &MainWindow::disagreeTeacher);
        num = 1;
    }
    ui->buttomLayout->addStretch();
    for (int i = 1; i <= num; i++) {
        dealButton[i]->setMinimumHeight(35);
        ui->buttomLayout->addWidget(dealButton[i]);
    }
}
void MainWindow::setDealButton()
{
    int num = 0;
    QLayoutItem* child;
    while ((child = ui->buttomLayout->itemAt(0)) != nullptr) {
        ui->buttomLayout->removeItem(child);
        delete child->widget();
        delete child;
        child = nullptr;
    }
    for (int i = 0; i < 10; i++)
        dealButton[i] = nullptr;
    if (thisUser.identity == QString("teacher")) {
        dealButton[1] = new QPushButton(QString("提交审核"));
        connect(dealButton[1], &QPushButton::clicked, this, &MainWindow::agreeTeacher);
        dealButton[2] = new QPushButton(QString("修改"));
        connect(dealButton[2], &QPushButton::clicked, this, &MainWindow::changeThisOrder);
        dealButton[3] = new QPushButton(QString("撤回订单"));
        connect(dealButton[3], &QPushButton::clicked, this, &MainWindow::disagreeTeacher);
        num = 3;
    } else if (thisUser.identity == QString("header")) {
        dealButton[1] = new QPushButton(QString("提交二审"));
        connect(dealButton[1], &QPushButton::clicked, this, &MainWindow::agreeHeader);
        dealButton[2] = new QPushButton(QString("修改"));
        connect(dealButton[2], &QPushButton::clicked, this, &MainWindow::changeThisOrder);
        dealButton[3] = new QPushButton(QString("驳回"));
        connect(dealButton[3], &QPushButton::clicked, this, &MainWindow::disagreeHeader);
        num = 3;
    } else if (thisUser.identity == QString("admin")) {
        dealButton[1] = new QPushButton(QString("允许出库"));
        connect(dealButton[1], &QPushButton::clicked, this, &MainWindow::agreeAdmin);
        dealButton[2] = new QPushButton(QString("修改"));
        connect(dealButton[2], &QPushButton::clicked, this, &MainWindow::changeThisOrder);
        dealButton[3] = new QPushButton(QString("驳回"));
        connect(dealButton[3], &QPushButton::clicked, this, &MainWindow::disagreeAdmin);
        num = 3;
    } else if (thisUser.identity == QString("keeper")) {
        dealButton[1] = new QPushButton(QString("打印订单"));
        connect(dealButton[1], &QPushButton::clicked, this, &MainWindow::whenPrintOrder);
        dealButton[2] = new QPushButton(QString("已出(入)库完成"));
        connect(dealButton[2], &QPushButton::clicked, this, &MainWindow::agreeKeeper);
        dealButton[3] = new QPushButton(QString("无法出库"));
        connect(dealButton[3], &QPushButton::clicked, this, &MainWindow::disagreeKeeper);
        num = 3;
    } else if (thisUser.identity == QString("accountant")) {
    }
    ui->buttomLayout->addStretch();
    for (int i = 1; i <= num; i++) {
        dealButton[i]->setMinimumHeight(35);
        ui->buttomLayout->addWidget(dealButton[i]);
    }
}

void MainWindow::againOrder() //重新发起订单
{
    int id = ui->orderTable->getCurrentID();
    if (id == 0) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    OneOrder* order = getOrder(id, historys); //WARNING 只能重新发起历史订单
    if (order == nullptr) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    QString ask = QString("确定重新发起该订单(编号为%1)吗？").arg(id);
    if (!askForConferm(ask))
        return;
    NewOrder* Norder = new NewOrder();
    Norder->addnewOrder_WithModel(*order);
    Norder->show();
}

void MainWindow::changeThisOrder() //修改选中订单
{
    int id = ui->orderTable->getCurrentID();
    if (id == 0)
        return;
    OneOrder* order = getOrder(id, dealorders); //只可以修改待处理订单
    if (order == nullptr)
        return;
    NewOrder* newOrder = new NewOrder();
    newOrder->changeOrder(order);
    connect(newOrder, &NewOrder::finishChange, this, &MainWindow::afterChange);
    newOrder->show();
}
void MainWindow::afterChange()
{
    ui->orderTable->flush();
}

void MainWindow::agreeHeader()
{
    changeToByID(QString("同意"), 3);
}
void MainWindow::disagreeHeader()
{
    changeToByID(QString("驳回"), 2);
}
void MainWindow::agreeAdmin()
{
    int id = ui->orderTable->getCurrentID();
    if (id == 0) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    OneOrder* order = getOrder(id, dealorders); //只能同意待处理订单
    if (order == nullptr) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    if (!orderCheck(*order)) {
        QMessageBox::warning(nullptr, QString("错误"), QString("仓库中物品数量不足！\n请修改订单或检查库存"));
        return;
    }
    changeToByID(QString("同意"), 5);
}
void MainWindow::disagreeAdmin()
{
    changeToByID(QString("驳回"), 4);
}
void MainWindow::agreeKeeper()
{
    int id = ui->orderTable->getCurrentID();
    if (id == 0) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    OneOrder* order = getOrder(id, dealorders); //只能同意待处理订单
    if (order == nullptr) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    QString ask = QString("确定该订单(编号为%1)已出(还)库完成吗？").arg(id);
    if (!askForConferm(ask))
        return;
    finishOrder(id);
    return;
}
void MainWindow::disagreeKeeper()
{
    int id = ui->orderTable->getCurrentID();
    if (id == 0)
        return;
    OneOrder* order = getOrder(id, dealorders);
    if (order == nullptr) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    if (order->status != config.statusList[5]) {
        QMessageBox::warning(nullptr, QString("错误"), QString("此类订单无法驳回"));
        return;
    }
    QString ask = QString("确定无法完成该订单(编号为%1)吗？").arg(id);
    if (!askForConferm(ask))
        return;
    changeStatus(id, config.statusList[6]);
}
void MainWindow::agreeTeacher()
{
    changeToByID(QString("提交"), 1);
}
void MainWindow::disagreeTeacher()
{
    changeToByID(QString("撤销"), 0);
}
/*-----------底部按钮end------------*/

/*-----------修改状态---------------*/
bool MainWindow::changeToByID(QString dowhat, int i) //修改订单状态
{
    int id = ui->orderTable->getCurrentID();
    if (id == 0)
        return false;
    OneOrder* order = getOrder(id, dealorders);
    if (order == nullptr) { //除撤销订单外，只能修改待处理订单
        if (i == 0)
            order = getOrder(id, noworders);
        if (order == nullptr) {
            QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
            return false;
        }
    }
    QString ask = QString("确定") + dowhat + QString("该订单(编号为%1)吗？").arg(id);
    if (!askForConferm(ask))
        return false;
    return changeStatus(id, config.statusList[i]);
}
bool MainWindow::changeStatus(int orderID, QString toStatus)
{
    messenger->changeOneStatus(orderID,toStatus);
    return true;
}

bool MainWindow::finishOrder(int orderID)
{
    messenger->finishOneOrder(orderID);
    return true;
}

void MainWindow::finishPost(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QMessageBox::information(nullptr, QString("完成"), QString("成功完成，请等待下一步"));
        reply->deleteLater();
        afterChange();
    } else {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，网络错误或无权操作\n请稍后重试或更换账号"));
        reply->deleteLater();
    }
}
/*-----------修改状态end------------*/

/*----------打印订单----------------*/
void MainWindow::whenPrintOrder()
{
    int id = ui->orderTable->getCurrentID();
    if (id == 0)
        return;
    OneOrder* order = getOrder(id, dealorders);
    if (order == nullptr) {
        QMessageBox::warning(nullptr, QString("错误"), QString("失败，请稍后重试"));
        return;
    }
    printOrder(*order);
}
void MainWindow::printOrder(OneOrder order)
{
    QPrinter printer(QPrinter::HighResolution);
    //自定义纸张大小，特别重要，不然预览效果极差
    printer.setPageSize(QPrinter::Custom);
    printer.setPaperSize(QSizeF(1100,1550),
                         QPrinter::Point);
    QPrintPreviewDialog preview(&printer, this);// 创建打印预览对话框

    preview.setMinimumSize(1000,600);
    /*
     * QPrintPreviewDialog类提供了一个打印预览对话框，里面功能比较全，
     * paintRequested(QPrinter *printer)是系统提供的，
     * 当preview.exec()执行时该信号被触发，
     * drawPic(QPrinter *printer)是自定义的槽函数，图像的绘制就在这个函数里。
     */
    //drawPic(preview.printer());
    connect(&preview, SIGNAL(paintRequested(QPrinter*)),this, SLOT(drawPic(QPrinter*)));

    preview.exec();
}
void MainWindow::drawPic(QPrinter *printerPixmap)
{
    QPixmap pix = QPixmap(1100,1550);
    //这个函数算是画模板的函数吧，毕竟打印时有模板的
    createPix(&pix);
    //pix.save("./pix.png");
    //pix.save(sFilePix);
    //纵向：Portrait 横向：Landscape
    printerPixmap->setOrientation(QPrinter::Landscape);
    //获取界面的图片
    QPainter painterPixmap(this);
    painterPixmap.begin(printerPixmap);
    QRect rect = painterPixmap.viewport();
    int x = rect.width() / pix.width();
    int y = rect.height() / pix.height();
    //设置图像长宽是原图的多少倍
    painterPixmap.scale(x, y);
    painterPixmap.drawPixmap(0, 0, pix);
    painterPixmap.end();
}
void MainWindow::createPix(QPixmap *pix)
{
    int wSize=1100,hSize=1550;
    int lBorder=50,rBorder=wSize-50,uBorder=50,dBorder=hSize-50;
    int yPis[6]={uBorder,uBorder+100,uBorder+150,uBorder+190,dBorder-350,dBorder-250};
    int fontW=rBorder-lBorder,fontH=30;

    QPainter *painter = new QPainter(this);
    painter->begin(pix);
    painter->setRenderHint(QPainter::Antialiasing, true);
    // 设置画笔颜色、宽度
    painter->setPen(QPen(QColor(255, 255, 255), 2));
    // 设置画刷颜色
    painter->setBrush(QColor(255, 255, 255));
    QRect rect(0,0,wSize,hSize);
    //整张图设置画刷白底
    painter->fillRect(rect,QColor(255, 255, 255));
    painter->drawRect(rect);

    int id = ui->orderTable->getCurrentID();
    if (id == 0){
        painter->end();
        return;
    }
    OneOrder* order = getOrder(id, dealorders);
    if (order == nullptr) {
        painter->end();
        return;
    }

    //画数据部分的线条
    painter->setPen(QPen(QColor(0, 0, 0), 2));
    QVector<QLine> lines;
    lines.append(QLine(QPoint(lBorder,uBorder),QPoint(rBorder,uBorder)));//上边
    lines.append(QLine(QPoint(rBorder,uBorder),QPoint(rBorder,dBorder)));//右边
    lines.append(QLine(QPoint(lBorder,dBorder),QPoint(rBorder,dBorder)));//下边
    lines.append(QLine(QPoint(lBorder,uBorder),QPoint(lBorder,dBorder)));//左边
    lines.append(QLine(QPoint(lBorder,yPis[1]),QPoint(rBorder,yPis[1])));//第一部分标题下边
    lines.append(QLine(QPoint(lBorder,yPis[2]),QPoint(rBorder,yPis[2])));//第二部分信息下边
    lines.append(QLine(QPoint(lBorder,yPis[3]),QPoint(rBorder,yPis[3])));//第三部分分栏下边
    lines.append(QLine(QPoint(lBorder,yPis[4]),QPoint(rBorder,yPis[4])));//第四部分物品列表下边
    lines.append(QLine(QPoint(130,yPis[2]),QPoint(130,yPis[3])));//类型分割边
    lines.append(QLine(QPoint(200,yPis[2]),QPoint(200,yPis[3])));//类型分割边
    lines.append(QLine(QPoint(270,yPis[2]),QPoint(270,yPis[3])));//类型分割边
    lines.append(QLine(QPoint(340,yPis[2]),QPoint(340,yPis[3])));//类型分割边
    lines.append(QLine(QPoint(450,yPis[2]),QPoint(450,yPis[3])));//类型分割边
    lines.append(QLine(QPoint(lBorder,yPis[5]),QPoint(rBorder,yPis[5])));//第五部分下边
    painter->drawLines(lines);
    QFont font;
    //font.setPixelSize(300);
    font.setPointSize(13);
    font.setFamily("黑体");

    //font.setItalic(true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setFont(font);

    //第一部分
    //painter->drawText(50,50,700,40,Qt::AlignCenter,QString("单位"));//单位名称
    painter->drawText(QPoint(650,yPis[1]-30),QString("No: ")+QString::number(order->id));
    font.setPointSize(20);
    painter->setFont(font);
    painter->drawText(lBorder,uBorder+30,fontW,60,Qt::AlignCenter,QString("订单"));//报告名称
    font.setPointSize(13);
    painter->setFont(font);
    //第二部分
    painter->drawText(lBorder,yPis[1],fontW,yPis[2]-yPis[1],Qt::AlignVCenter,QString("使用班级： ")+order->useclass);
    painter->drawText(lBorder+350,yPis[1],fontW,yPis[2]-yPis[1],Qt::AlignVCenter,QString("车间： ")+order->workshop);
    //第三部分
    painter->drawText(50,yPis[2]+5,70,fontH,Qt::AlignCenter,"序号");
    painter->drawText(130,yPis[2]+5,70,fontH,Qt::AlignCenter,"品类");
    painter->drawText(200,yPis[2]+5,70,fontH,Qt::AlignCenter,"名称");
    painter->drawText(270,yPis[2]+5,70,fontH,Qt::AlignCenter,"类型");
    painter->drawText(340,yPis[2]+5,110,fontH,Qt::AlignCenter,"数目");
    painter->drawText(450,yPis[2]+5,70,fontH,Qt::AlignCenter,"备注");
    //第四部分 物品列表
    int rowPix=5;
    for(OneItem item : order->items)
    {
        OneResItem rItem=getResItem(item.pid);
        if(rItem.pid==0)
            continue;
        painter->drawText(53,yPis[3]+rowPix,70,fontH,Qt::AlignCenter,QString::number(item.pid));
        painter->drawText(130,yPis[3]+rowPix,70,fontH,Qt::AlignCenter,rItem.res);
        painter->drawText(200,yPis[3]+rowPix,70,fontH,Qt::AlignCenter,rItem.name);
        painter->drawText(270,yPis[3]+rowPix,70,fontH,Qt::AlignCenter,rItem.type);
        QString s=QString::number(item.number,'g',2)+rItem.units;
        painter->drawText(340,yPis[3]+rowPix,110,fontH,Qt::AlignCenter,s);
        painter->drawText(450,yPis[3]+rowPix,fontW-450,fontH,Qt::AlignLeft,item.more);
        rowPix+=fontH;
    }
    //第五部分
    painter->drawText(lBorder,yPis[4]+5,fontW,fontH,Qt::AlignVCenter,QString("申请教师： "));
    painter->drawText(lBorder+fontW/2,yPis[4]+5,fontW,fontH,Qt::AlignVCenter,QString("仓库管理员： "));
    painter->drawText(lBorder,yPis[5]-40,fontW,fontH,
                      Qt::AlignVCenter,QString("发起日期：")+order->starttime.toString(QString("yyyy年MM月dd日")));
    painter->drawText(lBorder+fontW/2,yPis[5]-40,fontW,fontH,Qt::AlignVCenter,QString("领取日期：     年   月  日"));
    //第六部分
    painter->drawText(lBorder,yPis[5],fontW,dBorder-yPis[5],Qt::AlignVCenter,QString("备注： "));

    painter->end();
}
/*----------打印订单end--------------*/

/*-----------菜单栏-----------------*/
void MainWindow::on_newItemOrder()
{
    NewItemOrder* nio=new NewItemOrder();
    nio->show();
}

void MainWindow::on_newItemOrderTemplate()
{
    QString file=QFileDialog::getSaveFileName(nullptr,QString("选择保存路径"),"/添加物品模板","Exel file(*.xlsx)");
    if(file==nullptr || !file.endsWith(".xlsx"))
        return;
    QXlsx::Document xlsx;
    xlsx.write(1,1,QString("大类"));
    xlsx.write(1,2,QString("名称"));
    xlsx.write(1,3,QString("型号"));
    xlsx.write(1,4,QString("数量"));
    xlsx.write(1,5,QString("单位"));
    xlsx.write(1,6,QString("备注"));
    xlsx.saveAs(file);
}

void MainWindow::on_people_template_triggered()
{
    QString file=QFileDialog::getSaveFileName(nullptr,QString("选择保存路径"),"/添加人员模板","Exel file(*.xlsx)");
    if(file==nullptr || !file.endsWith(".xlsx"))
        return;
    QXlsx::Document xlsx;
    xlsx.write(1,1,QString("用户名"));
    xlsx.write(1,2,QString("真实姓名"));
    xlsx.write(1,3,QString("所属车间"));
    xlsx.write(1,4,QString("身份"));
    xlsx.saveAs(file);
}

void MainWindow::on_about_me_triggered()
{
    AboutMe* am = new AboutMe();
    connect(am, &AboutMe::needFlushMain, this, &MainWindow::flushMainWindow);
    am->show();
}

void MainWindow::on_about_writer_triggered()
{
    QString emailA = QString("<a href='mailto:richeyjang@163.com'>richeyjang@163.com</a>");
    QString text = config.softName + QString(" v") + config.nowClientVersion
        + QString("<br>反馈：") + emailA + QString("<br>作者：姜雨奇<br>院校：HUST");
    QMessageBox::information(nullptr, QString("关于本软件"), text);
}

void MainWindow::on_store_showAll_triggered()
{
    ItemsManager* itemM = new ItemsManager();
    itemM->show();
}

void MainWindow::on_people_change_triggered()
{
    UserManager* um = new UserManager();
    um->show();
}
