#ifndef INITDB_H
#define INITDB_H

#include <QtSql>

void addCurrency(QSqlQuery &q, const QString &code, const QString &name)
{
    q.addBindValue(code);
    q.addBindValue(name);
    q.exec();
}

void addBook(QSqlQuery &q, const QString &title, int year)
{
    q.addBindValue(title);
    q.addBindValue(year);
    q.exec();
}


QSqlError initDb()
{
    QSqlQuery q;

    if (!q.exec(QString("create table currencies(id integer primary key, code varchar, name varchar, description varchar, btc_price_path varchar)")))
        return q.lastError();

    if (!q.prepare(QString("insert into currencies (code, name) values (?, ?)")))
        return q.lastError();
    addCurrency(q, "BTC", "Bitcoin");
    addCurrency(q, "LTC", "Litecoin");

    if (!q.exec(QString("create table assets(id integer primary key, currency_id integer, quantity real, market_value_btc real, market_value_usd real)")))
        return q.lastError();

    if (!q.prepare(QString("insert into assets (currency_id, quantity) values (?, ?)")))
        return q.lastError();
    q.addBindValue(1);
    q.addBindValue(0.1);
    q.exec();
    q.addBindValue(2);
    q.addBindValue(0.2);
    q.exec();

    if (!q.exec(QString("create table pricecache(trading_pair varchar, price real, timestamp datetime)")))
        return q.lastError();
     if (!q.exec(QString("create unique index idx_trading_pair ON pricecache(trading_pair)")))
        return q.lastError();

    return QSqlError();
}

#endif
