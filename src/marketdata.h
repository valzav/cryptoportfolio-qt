#ifndef MARKET_DATA_H
#define MARKET_DATA_H

struct Currency {
    QString code;
    QString name;
    operator QString () const { return QString(code) + QString(":") + QString(name); }
};

struct PriceValue {
    enum Status { OK, EMPTY, ERROR, EXPIRED};
    double value;
    Status status;
    PriceValue() : value(0.0), status(EMPTY) {}
    PriceValue(double _value, Status _status) : value(_value), status(_status) {}
    QString toString() const;
    operator QString () const { return toString(); }
};

class MarketDataProvider {
public:
    static MarketDataProvider *getMarketDataProvider();
    virtual PriceValue getBtcPrice(const QString& ticker, bool force_refresh=false) = 0;
    virtual double getBtcUsdRate() = 0;
    virtual void getCurrenciesList(QList<Currency>& list) = 0;
    virtual ~MarketDataProvider() {}
protected:
    PriceValue getPriceFromCache(const QString &trading_pair);
    void addPriceToCache(const QString &trading_pair, double price);
};

class CryptocoinChartsMDP : public MarketDataProvider {
    PriceValue getPrice(const QString& trading_pair, bool force_refresh=false);
public:
    PriceValue getBtcPrice(const QString& ticker, bool force_refresh=false);
    double getBtcUsdRate();
    void getCurrenciesList(QList<Currency>& list);
    ~CryptocoinChartsMDP() {}
};

#endif // MARKET_DATA_H
