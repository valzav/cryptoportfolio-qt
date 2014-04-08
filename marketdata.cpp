#include <QCoreApplication>
#include <QDebug>
#include <QApplication>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QVariant>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>
#include <QtSql>

#include "marketdata.h"

QJsonObject sendRequest(const QString &url);

QString PriceValue::toString() const {
    QString res;
    char buf[8];
    switch (status) {
    case OK:
        strncpy(buf,"OK",8); break;
    case EMPTY:
        strncpy(buf,"EMPTY",8); break;
    case ERROR:
        strncpy(buf,"ERROR",8); break;
    case EXPIRED:
        strncpy(buf,"EXPIRED",8); break;
    }
    res.sprintf("PriceValue<%f,%s>",value, buf);
    return res;
}

MarketDataProvider *MarketDataProvider::getMarketDataProvider(){
    return new CryptocoinChartsMDP();
}

PriceValue MarketDataProvider::getPriceFromCache(const QString &trading_pair){
    PriceValue res;
    QSqlQuery q;
    q.prepare(QString("select price, timestamp from pricecache where trading_pair=?"));
    q.addBindValue(trading_pair);
    q.exec();
    if(!q.next()) return res;
    res.value = q.value(0).toDouble();
    QDateTime dt_cache; dt_cache.setTime_t(q.value(1).toUInt());
    QDateTime dt_now = QDateTime::currentDateTime();
    res.status = dt_cache.addSecs(5*60) < dt_now ? PriceValue::EXPIRED : PriceValue::OK;
    qDebug() << "-- MarketDataProvider::getPriceFromCache " << trading_pair << res;
    return res;
}

void MarketDataProvider::addPriceToCache(const QString &trading_pair, double price){
    QSqlQuery q;
    q.prepare(QString("replace into pricecache (trading_pair, price, timestamp) values (?, ?, ?)"));
    q.addBindValue(trading_pair);
    q.addBindValue(price);
    q.addBindValue(QDateTime::currentDateTime().toTime_t());
    q.exec();
}

PriceValue CryptocoinChartsMDP::getPrice(const QString& trading_pair){
    PriceValue price = getPriceFromCache(trading_pair);
    if(true && price.status != PriceValue::OK) {
        QString url = "http://www.cryptocoincharts.info/v2/api/tradingPair/" + trading_pair;
        qDebug() << "CryptocoinChartsMDP::getBtcPrice url: " << url;
        QJsonObject json = sendRequest(url);
        if(json.contains("price")){
            QString price_str = json["price"].toString();
            price.value = price_str.toDouble();
            price.status = PriceValue::OK;
            addPriceToCache(trading_pair, price.value);
        }
        qDebug() << "-- CryptocoinChartsMDP::getPrice " << trading_pair << price;
    }
    return price;
}

PriceValue CryptocoinChartsMDP::getBtcPrice(const QString &ticker) {
    if(ticker == "") return PriceValue(0.0, PriceValue::OK);
    if(ticker == "BTC") return PriceValue(1.0, PriceValue::OK);
    QString trading_pair = ticker.toLower() + "_btc";
    return getPrice(trading_pair);
}

double CryptocoinChartsMDP::getBtcUsdRate() {
    return getPrice("btc_usd").value;
}

QJsonObject sendRequest(const QString &url) {
    // create custom temporary event loop on stack
    QEventLoop eventLoop;
    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    // the HTTP request
    QUrl qurl = QUrl( url );
    QNetworkRequest req( qurl );
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called
    if (reply->error() == QNetworkReply::NoError) {
        QString strReply = (QString)reply->readAll();
        qDebug() << "Response:" << strReply;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        delete reply;
        return jsonResponse.object();
    }
    else {
        //failure
        qDebug() << "sendRequest failure: " <<reply->errorString();
        delete reply;
    }
    return QJsonObject();
}
