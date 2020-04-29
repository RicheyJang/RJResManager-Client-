#include "hintcombobox.h"

HintList::HintList(QWidget* parent)
    : QListWidget(parent)
{
}

HintComboBox::HintComboBox(QWidget* parent)
    : QComboBox(parent)
{
    installEventFilter(this);
    setEditable(true);
    items << QString("默认");
    setStringList(items);
    list = new HintList(parent);
    list->hide();
    connect(this->lineEdit(), &QLineEdit::textEdited, this, &HintComboBox::showHint);
    connect(list, &QListWidget::itemClicked, this, &HintComboBox::selectOne);
}

void HintComboBox::selectOne()
{
    if (list->count() >= 1)
        this->setCurrentText(list->currentItem()->text());
    list->hide();
}

void HintComboBox::flushList()
{
    list->clear();
    //    for (int i = 1; i <= 50; i++) {
    //        list->addItem(QString::number(i));
    //    }
    QString key = this->currentText();
    for (QString s : items) {
        if (s.contains(key)) {
            list->addItem(s);
        }
    }
    if (list->count() <= 0)
        list->addItem(QString("无结果"));
}

void HintComboBox::showHint()
{
    flushList();
    if (list->isHidden()) {
        int x = this->x() + 4;
        int y = this->y() + this->height();
        int w = this->width();
        int h = 100;
        list->setGeometry(x, y, w, h);
        list->setCurrentRow(0);
        list->show();
    }
    list->raise();
    if (this->currentText().compare(list->item(0)->text()) == 0) {
        list->hide();
    }
}

bool HintComboBox::eventFilter(QObject* o, QEvent* e)
{
    if (QEvent::MouseButtonPress == e->type()) //窗口停用
    {
        if (o != list && !list->isHidden()) {
            list->hide();
        }
    }
    return QWidget::eventFilter(o, e);
}

void HintComboBox::setStringList(QStringList sl)
{
    if (sl.size() == 0) {
        sl << QString("默认");
        this->clear();
        this->addItems(sl);
    } else {
        items = sl;
        this->clear();
        this->addItems(sl);
    }
}
