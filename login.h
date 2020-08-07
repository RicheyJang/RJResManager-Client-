#ifndef LOGIN_H
#define LOGIN_H

#include "maininclude.h"
#include <QWidget>

namespace Ui {
class Login;
}

class Login : public QWidget {
    Q_OBJECT

public:
    explicit Login(QWidget* parent = nullptr);
    ~Login();

signals:
    void succsesslogin();
    void faillogin();
    void wrongpassword();
    void suddenClose();
    void allSuccess();
    void hasNewVersion();

private slots:
    void closeEvent(QCloseEvent* event) override;
    void on_checkIn_clicked();
    void finishTrylogin(QNetworkReply* reply);
    void whenSuccesslogin();
    void whenHasNewVersion();
    void whenFaillogin();
    void whenWrongpassword();

private:
    void allInit();
    int trylogin(QString name, QString password);
    void download(QString url);
    Ui::Login* ui;
    QNetworkAccessManager* manager;
    QNetworkRequest* request;
};

#endif // LOGIN_H
