#ifndef ERRORWIDGET_H
#define ERRORWIDGET_H

#include "maininclude.h"
#include <QWidget>

class ErrorWidget : public QWidget {
    Q_OBJECT

public:
    ErrorWidget(QWidget* parent = nullptr);
    ~ErrorWidget();
    void setTipInfo(QString info);
    void setTipIcon(QPixmap pixmap);

private:
    QToolButton* close_button;
    QLabel* msg_label;
    QLabel* ask_label;

private slots:
    bool closeWidget();
};

#endif // ERRORWIDGET_H
