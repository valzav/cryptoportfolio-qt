#include "dbhelper.h"
#include "marketdata.h"

DBHelper::DBHelper(const QString &_db_path)
    : db_path(_db_path)
{
}

DBHelper::~DBHelper() {
    db.close();
}

QSqlError DBHelper::initDb() {
    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        QSqlError er("", "Unable to find database driver", QSqlError::ConnectionError, -1);
        return er;
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(db_path);

    if (!db.open()) return db.lastError();

    QStringList tables = db.tables();
    if (tables.contains("assets", Qt::CaseInsensitive)) return QSqlError();

    QSqlQuery q;

    if (!q.exec(QString("create table currencies(id integer primary key, code varchar, name varchar, description varchar, btc_price_path varchar)")))
        return q.lastError();
    if (!q.exec(QString("create unique index idx_currencies_code ON currencies(code)")))
       return q.lastError();

    if (!q.prepare(QString("insert into currencies (code, name) values (?, ?)")))
        return q.lastError();
    addCurrency("", "");
    addCurrency("BTC", "Bitcoin");
    addCurrency("LTC", "Litecoin");

    if (!q.exec(QString("create table assets(id integer primary key, currency_id integer, quantity real, price_btc real, market_value_btc real, price_usd real, market_value_usd real)")))
        return q.lastError();

    if (!q.prepare(QString("insert into assets (currency_id, quantity) values (?, ?)")))
        return q.lastError();
    q.addBindValue(2);
    q.addBindValue(0.1);
    q.exec();
    q.addBindValue(3);
    q.addBindValue(0.2);
    q.exec();

    if (!q.exec(QString("create table pricecache(trading_pair varchar, price real, timestamp datetime)")))
        return q.lastError();
     if (!q.exec(QString("create unique index idx_trading_pair ON pricecache(trading_pair)")))
        return q.lastError();

    return QSqlError();
}

QSqlError DBHelper::updateCurrenciesTable(const QList<Currency>& list) {
    QSqlQuery q;
    for(QList<Currency>::const_iterator i=list.begin(); i!=list.end(); ++i) {
        q.prepare(QString("select name from currencies where code=?"));
        q.addBindValue(i->code);
        q.exec();
        if(q.next()) { q.clear(); continue; }
        q.clear();

        q.prepare(QString("insert into currencies (code, name) values (?, ?)"));
        q.addBindValue(i->code);
        q.addBindValue(i->name);
        if(!q.exec()) return q.lastError();
    }
    return QSqlError();
}


void DBHelper::addCurrency(const QString &code, const QString &name) {
    q.addBindValue(code);
    q.addBindValue(name);
    q.exec();
}

void DBHelper::addBook(const QString &title, int year) {
    q.addBindValue(title);
    q.addBindValue(year);
    q.exec();
}
