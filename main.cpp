#define ISMAINCPP 1
#include "login.h"
#include "maininclude.h"
#include "mainwindow.h"
#include <QApplication>

QSet<OneResItem> allResItem;
ResItemsTrie resItemsTrie;
QVector<OneOrder> dealorders;
QVector<OneOrder> noworders;
QVector<OneOrder> historys;
QVector<OneOrder> newitems;
User thisUser;
Config config("./config.ini");

/*
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    //    if (!initItems()) {
    //        return -1;
    //    }
    //Login* login = new Login();
    //login->show();
    MainWindow w;
    //w.setLoginWindow(login);
    //w.hide();
    w.show();
    return a.exec();
}
*/

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QFile file(":/text/style.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());

        a.setStyleSheet(styleSheet);
        file.close();
    }

    Login* login = new Login();
    login->show();
    MainWindow w;
    w.setLoginWindow(login);
    w.hide();

    return a.exec();
}
