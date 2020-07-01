#ifndef QDBC_H
#define QDBC_H

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>

class Database {
private:
    QSqlDatabase indatabase;
    QString host, databaseName, userName, password;
    int port;
    bool canUse = false;

public:
    inline bool check()
    {
        return canUse;
    }
    Database(QString _host, int _port, QString _databaseName, QString _userName, QString _password)
        : host(_host)
        , databaseName(_databaseName)
        , userName(_userName)
        , password(_password)
    {
        port = _port;
        if (QSqlDatabase::contains(userName))
            indatabase = QSqlDatabase::database(userName);
        else
            indatabase = QSqlDatabase::addDatabase("QMYSQL", userName);
        indatabase.setHostName(host);
        indatabase.setPort(port);
        indatabase.setDatabaseName(databaseName); //这里输入你的数据库名
        indatabase.setUserName(userName);
        indatabase.setPassword(password); //这里输入你的密码
        if (!indatabase.open()) {
            canUse = false;
            //QMessageBox::information(nullptr, QString("warning"), QString("载入数据库失败"));
            return;
        }
        canUse = true;
        return;
    }
    bool exec(QString sql)
    {
        if (check()) {
            QSqlQuery query(indatabase);
            return query.exec(sql);
        }
        return false;
    }
    void close()
    {
        if (check()) {
            indatabase.close();
        }
        canUse = false;
        return;
    }
    QSqlDatabase getDatabase()
    {
        return indatabase;
    }
    ~Database()
    {
        if (check()) {
            indatabase.close();
        }
    }
};
#endif // QDBC_H
