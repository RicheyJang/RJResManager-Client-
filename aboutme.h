#ifndef ABOUTME_H
#define ABOUTME_H

#include "maininclude.h"
#include <QWidget>

namespace Ui {
class AboutMe;
}

class AboutMe : public QWidget {
    Q_OBJECT

public:
    explicit AboutMe(QWidget* parent = nullptr);
    ~AboutMe();

private slots:
    void on_conferm_clicked();

private:
    Ui::AboutMe* ui;

    void setAboutMe();
};

#endif // ABOUTME_H
