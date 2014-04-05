#ifndef MARKET_DATA_H
#define MARKET_DATA_H

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
    virtual PriceValue getBtcPrice(const QString& ticker) = 0;
    virtual double getBtcUsdRate() = 0;
protected:
    PriceValue getPriceFromCache(const QString &trading_pair);
    void addPriceToCache(const QString &trading_pair, double price);
};

class CryptocoinChartsMDP : public MarketDataProvider {
    PriceValue getPrice(const QString& trading_pair);
public:
    virtual PriceValue getBtcPrice(const QString& ticker);
    virtual double getBtcUsdRate();
};

#endif // MARKET_DATA_H
