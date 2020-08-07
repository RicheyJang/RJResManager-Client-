#ifndef USERDIALOG_H
#define USERDIALOG_H

#include "maininclude.h"
#include "messenger.h"
#include <QCloseEvent>
#include <QWidget>

namespace Ui {
class UserDialog;
}

class UserDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UserDialog(User user,QWidget *parent = nullptr);
    ~UserDialog();

private slots:
    void closeEvent(QCloseEvent* event) override;

    void on_button_cancel_clicked();

    void on_button_confirm_clicked();

private:
    void setTable();

    User theOne;
    Messenger* messenger;
    Ui::UserDialog *ui;
};

#endif // USERDIALOG_H
