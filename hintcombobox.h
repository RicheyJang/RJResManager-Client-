#ifndef HINTCOMBOBOX_H
#define HINTCOMBOBOX_H
#include "maininclude.h"

class HintList : public QListWidget {
public:
    HintList(QWidget* parent = nullptr);
};

class HintComboBox : public QComboBox {
public:
    HintComboBox(QWidget* parent = nullptr);
    void setStringList(QStringList sl);

public slots:
    void showHint();

protected:
    bool eventFilter(QObject* o, QEvent* e);

private:
    void flushList();
    void selectOne();
    HintList* list;
    QStringList items;
};

#endif // HINTCOMBOBOX_H
