#include "QDBC.h"
#include "maininclude.h"

#include <QJsonDocument>
#include <QJsonObject>

bool initItems()
{
    allType.clear();
    Database base(config.ip, config.dataPort, config.basename, thisUser.useName, thisUser.usePassword);
    QSqlDatabase database = base.getDatabase();
    QSqlQuery query(database);
    query.exec("select * from allitems;");
    while (query.next()) {
        OneType atype;
        atype.pid = query.value(0).toInt();
        atype.res = query.value(1).toString();
        atype.name = query.value(2).toString();
        atype.type = query.value(3).toString();
        atype.units = query.value(4).toString();
        atype.cnt = query.value(5).toDouble();
        addType(atype, false);
    }
    base.close();
    return true;
}
QString getOrderSql(int onwhich)
{
    Database* base = new Database(config.ip, config.dataPort, config.basename, thisUser.useName, thisUser.usePassword);
    QSqlDatabase database = base->getDatabase();
    QSqlQuery query(database);
    QSqlQuery itemQuery(database);
    QString iden = thisUser.identity;
    for (QChar c : iden) {
        if (!c.isLetter())
            return nullptr;
    }
    if (onwhich == ONDealOrder)
        query.exec("select status from authority where " + iden + "=1 or " + iden + "=3;");
    else if (onwhich == ONNowOrder)
        query.exec("select status from authority where " + iden + "=2 or " + iden + "=3;");
    else
        return nullptr;
    QString sql = " false";
    while (query.next()) {
        sql = sql + " or status='" + query.value(0).toString() + "'";
    }
    sql = sql + " ";
    base->close();
    return sql;
}
bool formOrders(QString whereSql, QVector<OneOrder>& orders)
{
    orders.clear();
    Database* base = new Database(config.ip, config.dataPort, config.basename, thisUser.useName, thisUser.usePassword);
    QSqlDatabase database = base->getDatabase();
    QSqlQuery query(database);
    QSqlQuery itemQuery(database);
    query.exec("select id,workshop,useclass,starttime,usetime,more,teacher,header,admin,keeper,accountant,status from orders where" + whereSql + ";");
    while (query.next()) {
        OneOrder* order = new OneOrder;
        order->id = query.value(0).toInt();
        order->workshop = query.value(1).toString();
        order->useclass = query.value(2).toString();
        order->starttime = query.value(3).toDate();
        order->usetime = query.value(4).toDateTime();
        order->more = query.value(5).toString();
        order->teacher = query.value(6).toString();
        order->header = query.value(7).toString();
        order->admin = query.value(8).toString();
        order->keeper = query.value(9).toString();
        order->accountant = query.value(10).toString();
        order->status = query.value(11).toString();
        itemQuery.exec("select pid,cnt,status,more from orderitems where orderid=" + QString::number(order->id));
        while (itemQuery.next()) {
            OneItem item;
            item.pid = itemQuery.value(0).toInt();
            item.number = itemQuery.value(1).toDouble();
            item.status = itemQuery.value(2).toString();
            item.more = itemQuery.value(3).toString();
            order->items.push_back(item);
        }
        orders.push_back(*order);
    }
    base->close();
    return true;
}
bool initDealOrders()
{
    Database* base = new Database(config.ip, config.dataPort, config.basename, thisUser.useName, thisUser.usePassword);
    QSqlDatabase database = base->getDatabase();
    QSqlQuery query(database);
    QSqlQuery itemQuery(database);
    QString sql = getOrderSql(ONDealOrder);
    return formOrders(sql, dealorders);
}
bool initNowOrders()
{
    noworders.clear();
    Database* base = new Database(config.ip, config.dataPort, config.basename, thisUser.useName, thisUser.usePassword);
    QSqlDatabase database = base->getDatabase();
    QSqlQuery query(database);
    QSqlQuery itemQuery(database);
    QString sql = getOrderSql(ONNowOrder);
    return formOrders(sql, noworders);
}
//TODO 按照时间范围更新当前订单及待处理订单，重写getOrderSql即可

uint qHash(const OneType key)
{
    return uint(key.pid);
}
bool hasType(QString res, QString name, QString type)
{
    for (OneType atype : allType) {
        if (atype.res.compare(res) == 0 && atype.name.compare(name) == 0 && atype.type.compare(type) == 0)
            return true;
    }
    return false;
}
int getTypePid(QString res, QString name, QString type)
{
    for (OneType atype : allType) {
        if (atype.res.compare(res) == 0 && atype.name.compare(name) == 0 && atype.type.compare(type) == 0)
            return atype.pid;
    }
    return 0;
}
const OneType* getType(int pid)
{
    OneType type;
    type.pid = pid;
    QSet<OneType>::iterator it = allType.find(type);
    if (it == allType.end())
        return nullptr;
    return &(*it);
}
QString getUnits(QString res, QString name, QString type)
{
    for (OneType atype : allType) {
        if (atype.res.compare(res) == 0 && atype.name.compare(name) == 0 && atype.type.compare(type) == 0)
            return atype.units;
    }
    return nullptr;
}
void addType(OneType atype, bool forCheck = false)
{
    if (forCheck == true) {
        if (hasType(atype.res, atype.name, atype.type))
            return;
    }
    allType.insert(atype);
}
bool regCheck(QString reg, QString s)
{
    QRegExp rx(reg);
    QRegExpValidator v(rx);
    int pos = 0;
    if (v.validate(s, pos) == QValidator::Acceptable)
        return true;
    return false;
}

QString toSHA256(QString s)
{
    QByteArray qb = QCryptographicHash::hash(s.toUtf8(), QCryptographicHash::Sha256);
    int n = qb.size();
    QString res;
    unsigned int tmp;
    char cc;
    for (int i = 0; i < n; i++) {
        tmp = (unsigned char)qb[i];
        cc = (tmp & (0xf0)) >> 4;
        if (cc > 9)
            cc = 'a' + cc - 10;
        else
            cc = '0' + cc;
        res.append(cc);
        cc = tmp & (0xf);
        if (cc > 9)
            cc = 'a' + cc - 10;
        else
            cc = '0' + cc;
        res.append(cc);
    }
    return res;
}

Config::Config(QString file)
{
    //TODO 配置文件待实现
    ip = "127.0.0.1";
    serverPort = 2333;
    dataPort = 3306;
    UserAgent = "ResClient";
    basename = "finaltest";
}
