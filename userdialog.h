#ifndef USERDIALOG_H
#define USERDIALOG_H

#include "maininclude.h"
#include "messenger.h"
#include <QCloseEvent>
#include <QWidget>

#define ONADDNEWUSER 111
#define ONCHANGEUSER 112

namespace Ui {
class UserDialog;
}

class UserDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UserDialog(QWidget *parent = nullptr);
    void setChangeUser(User user);
    void setAddNewUser();
    ~UserDialog();

signals:
    void finishAll();

private slots:
    void closeEvent(QCloseEvent* event) override;
    void afterConfirmReply(QNetworkReply*);
    void on_button_cancel_clicked();
    void on_button_confirm_clicked();

private:
    void setTable(User user);

    int dowhat;
    bool allFinish;
    User forOne;
    Messenger* messenger;
    Ui::UserDialog *ui;
};

#endif // USERDIALOG_H
