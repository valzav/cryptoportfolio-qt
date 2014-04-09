#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QMainWindow>
#include <QtSql>

namespace Ui {
    class MainWindow;
}
class MarketDataProvider;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_removeAssetButton_clicked();
    void on_addAssetButton_clicked();
    void on_currencyComboBox_currentIndexChanged(int index);
    void data_changed(const QModelIndex& index);

    void on_refreshMarketButton_clicked();

private:

    void showError(const QSqlError &err);
    void updateCurrencies();
    void refreshMarketData(bool force_refresh);
    void refreshMarketDataRow(int row_index, bool force_refresh);
    QSqlRelationalTableModel *model;
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QDataWidgetMapper *mapper;
    MarketDataProvider *market;
    int currency_column_index;
    int quantity_index;
    int price_btc_index;
    int mv_btc_index;
    int price_usd_index;
    int mv_usd_index;
};

#endif // MAINWINDOW_H
