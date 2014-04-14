#include <QtTest/QtTest>
#include "testmarketdata.h"
#include "../marketdata.h"
#include "../dbhelper.h"

TestMarketData::TestMarketData(QObject *parent) :
    QObject(parent), db_helper(NULL), market(NULL)
{
    db_helper = new DBHelper(":memory:");
    QSqlError err = db_helper->initDb();
    if (err.type() != QSqlError::NoError) {
        qDebug() << "Error: " << err;
    }
    market = new CryptocoinChartsMDP();
}

 TestMarketData::~TestMarketData() {
    delete db_helper;
    delete market;
}

void TestMarketData::TestUpdateCurrencies() {
    QList<Currency> list;
    market->getCurrenciesList(list);
    QVERIFY( list.size() > 100 );
    QSqlError err = db_helper->updateCurrenciesTable(list);
    if (err.type() != QSqlError::NoError) {
        qDebug() << "Error: " << err;
        QFAIL ( err.text().toStdString().c_str() );
    }
    QSqlQuery q;
    q.exec("select count(*) from currencies");
    if(!q.next()) QFAIL("currencies table is empty in TestUpdateCurrencies");
    int count = q.value(0).toInt();
    QCOMPARE(list.size(), count);
}

void TestMarketData::TestGetBtcPrice() {
    PriceValue price = market->getBtcPrice("LTC", true);
    QVERIFY( price.value > 0.01 );
    QVERIFY( price.value < 0.1 );
}

QTEST_MAIN(TestMarketData)
