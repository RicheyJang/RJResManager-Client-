#ifndef ABOUTME_H
#define ABOUTME_H

#include "maininclude.h"
#include "messenger.h"
#include <QCloseEvent>
#include <QWidget>

namespace Ui {
class AboutMe;
}

class AboutMe : public QWidget {
    Q_OBJECT

public:
    explicit AboutMe(QWidget* parent = nullptr);
    ~AboutMe();
signals:
    void needFlushMain();

private slots:
    void on_conferm_clicked();
    void whenNeedChangePass();
    void getChangeReply(QNetworkReply* reply);
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::AboutMe* ui;
    QLineEdit *oldPassEdit,*newPassEdit,*newPassAgEdit;
    QLabel *tipLabel;
    bool needChangePass;
    void setAboutMe();
};

#endif // ABOUTME_H
