#include "errorwidget.h"

ErrorWidget::ErrorWidget(QWidget* parent)
    : QWidget(parent)
{

    int width = parent->width();
    this->resize(width, 30);
    //设置标题栏隐藏
    this->setWindowFlags(Qt::FramelessWindowHint);
    //设置背景色透明
    move(this->pos().x(),this->pos().y()+25);

    QPalette palette;
    QColor color(190, 230, 250);
    color.setAlphaF(0.6);
    palette.setBrush(this->backgroundRole(), color);
    this->setPalette(palette);
    //如果这个QWidget直接show，是有背景色的，但是如果放到一个父Widget中时，它就没有了效果。添加如下代码后就可以了：
    this->setAutoFillBackground(true);

    //构建关闭按钮
    close_button = new QToolButton(this);
    QPixmap close_pix = QApplication::style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
    close_button->setIcon(close_pix);
    close_button->setStyleSheet("QToolButton{background-color: transparent;}");

    //获取主界面的宽度
    int height = this->height();
    close_button->setGeometry(width - 20, 0, 20, 20);
    //设置提示图片
    msg_label = new QLabel(this);
    msg_label->setGeometry(QRect(5, 5, 20, 20));
    msg_label->setStyleSheet("background-color: transparent;");
    msg_label->setScaledContents(true);
    //设置提示信息
    ask_label = new QLabel(this);
    ask_label->setStyleSheet("background-color: transparent; color: red;");
    ask_label->setGeometry(QRect(30, 0, width - 60, height));
    ask_label->setAlignment(Qt::AlignCenter);
    close_button->setCursor(Qt::PointingHandCursor);
    QObject::connect(close_button, SIGNAL(clicked()), this, SLOT(closeWidget()));

    /*    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(msg_label);
    hLayout->addWidget(ask_label);
    hLayout->addWidget(close_button);
    setLayout(hLayout); */

    hide();
}

ErrorWidget::~ErrorWidget()
{
    //Setting::freePointer(ask_label);
    //Setting::freePointer(msg_label);
    //Setting::freePointer(close_button);
}

void ErrorWidget::setTipInfo(QString info)
{
    //设置提示信息
    ask_label->setText(info);
}
void ErrorWidget::setTipIcon(QPixmap pixmap)
{
    msg_label->setPixmap(pixmap);
}

//关闭按钮主要进行提示框的隐藏
bool ErrorWidget::closeWidget()
{
    this->hide();
    return true;
}
