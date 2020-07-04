#ifndef MAININCLUDE_H
#define MAININCLUDE_H

/*-----宏定义-------*/
#define MaxIDLength 30
#define ONDealOrder 11
#define ONNowOrder 12
#define ONHistory 13
/*-----宏定义结束---*/

/*----头文件-------*/
#include "QDBC.h"
#include <QByteArray>
#include <QCloseEvent>
#include <QComboBox>
#include <QCompleter>
#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPushButton>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSet>
#include <QSettings>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVector>
#include <QtAlgorithms>
#include <qmath.h>
#include <unordered_map>
/*----头文件结束---*/

/*----结构声明-----*/
class Config {
public:
    QString ip;
    int serverPort;
    int dataPort;
    int MaxHistoryOrders;
    QString UserAgent;
    QString basename;
    QStringList statusList;
    QStringList itemsList;
    QString nowClientVersion;
    char itemStartWith[5];
    QDate orderStartDay;

    int TableOnePageRows;
    Config(QString file);
};
struct OneType {
public:
    int pid;
    QString res;
    QString name;
    QString type;
    double cnt;
    QString units;
    int resId;
    int nameId;
    int typeId;
    bool operator<(const OneType a) const
    {
        return pid < a.pid;
    }
    bool operator==(const OneType a) const
    {
        return pid == a.pid;
    }
};
uint qHash(const OneType key);
struct OneItem {
public:
    int pid;
    double number;
    QString status;
    QString more;
};
struct OneOrder {
public:
    int id;
    QVector<OneItem> items;
    QString workshop;
    QString useclass;
    QDate starttime;
    QDateTime usetime;
    QString more;
    QString teacher;
    QString header;
    QString admin;
    QString keeper;
    QString accountant;
    QString status;
    bool operator<(const OneOrder a) const
    {
        return id < a.id;
    }
};
struct User {
public:
    int id;
    QString username;
    QString password;
    QString truename;
    QString workshop;
    QString storehouse;
    QString useName;
    QString usePassword;
    QString identity;
    QString trueIdentity;
    bool isUseful;
    char m_padding[3];
    bool operator<(const User a) const
    {
        return id < a.id;
    }
};
/*--结构声明结束---*/

/*--全局变量声明---*/
#ifndef ISMAINCPP
extern QVector<OneOrder> dealorders;
extern QVector<OneOrder> noworders;
extern QVector<OneOrder> historys;
extern QSet<OneType> allType;
extern User thisUser;
extern Config config;
#endif
/*全局变量声明结束*/

/*函数定义*/
bool initItems();
bool initDealOrders();
bool initNowOrders();
bool initSatus();
bool flushDealOrders(QDate start, QDate end);
bool flushNowOrders(QDate start, QDate end);
bool flushHistoryOrders(QDate start, QDate end);
void addType(OneType atype, bool forCheck);
bool hasType(QString res, QString name, QString type);
OneOrder* getOrder(int id, QVector<OneOrder>& orders);
const OneType* getType(int pid);
int getTypePid(QString res, QString name, QString type);
QString getUnits(QString res, QString name, QString type);
bool regCheck(QString reg, QString s);
bool orderCheck(OneOrder order);
QString toSHA256(QString s);
/*函数定义结束*/

#endif // MAININCLUDE_H
