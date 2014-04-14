#ifndef TESTMARKETDATA_H
#define TESTMARKETDATA_H

#include <QObject>

class DBHelper;
class MarketDataProvider;

class TestMarketData : public QObject
{
    Q_OBJECT
public:
    explicit TestMarketData(QObject *parent = 0);
    ~TestMarketData();

private:
    DBHelper *db_helper;
    MarketDataProvider *market;

signals:

private slots:
    void TestUpdateCurrencies();
    void TestGetBtcPrice();
};

#endif // TESTMARKETDATA_H
