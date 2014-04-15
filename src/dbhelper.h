#ifndef DBHELPER_H
#define DBHELPER_H

#include <QtSql>

struct Currency;

class DBHelper {
public:
    DBHelper(const QString &_db_path);
    QSqlError initDb();
    QSqlError updateCurrenciesTable(const QList<Currency> &list);
    ~DBHelper();
private:
    const QString db_path;
    QSqlDatabase db;
    QSqlQuery q;
    void addCurrency(const QString &code, const QString &name);
    void addBook(const QString &title, int year);
};

#endif
