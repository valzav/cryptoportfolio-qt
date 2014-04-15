#include <QSqlError>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "asseteditdelegate.h"
#include "marketdata.h"
#include "dbhelper.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString data_path = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
    QDir data_dir;
    data_dir.mkpath(data_path);
    QString db_path = data_path + QDir::separator() + QString("assets.db");
    qDebug() << "DB path : " << db_path;

    db_helper = new DBHelper(db_path);
    QSqlError err = db_helper->initDb();
    if (err.type() != QSqlError::NoError) {
        showError(err);
        throw err;
    }

    market = new CryptocoinChartsMDP();
    updateCurrencies();

    model = new QSqlRelationalTableModel(ui->assetsTable);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("assets");

    currency_column_index = model->fieldIndex("currency_id");
    quantity_index = model->fieldIndex("quantity");
    price_btc_index = model->fieldIndex("price_btc");
    mv_btc_index = model->fieldIndex("market_value_btc");
    price_usd_index = model->fieldIndex("price_usd");
    mv_usd_index = model->fieldIndex("market_value_usd");

    model->setRelation(currency_column_index, QSqlRelation("currencies", "id", "code"));

    model->setHeaderData(currency_column_index, Qt::Horizontal, tr("Currency"));
    model->setHeaderData(model->fieldIndex("quantity"), Qt::Horizontal, tr("Quantity"));
    model->setHeaderData(model->fieldIndex("price_btc"), Qt::Horizontal, tr("Price (BTC)"));
    model->setHeaderData(model->fieldIndex("market_value_btc"), Qt::Horizontal, tr("Market Value (BTC)"));
    model->setHeaderData(model->fieldIndex("price_usd"), Qt::Horizontal, tr("Price (USD)"));
    model->setHeaderData(model->fieldIndex("market_value_usd"), Qt::Horizontal, tr("Market Value (USD)"));

    if (!model->select()) {
        showError(model->lastError());
        return;
    }

    ui->assetsTable->setModel(model);
    ui->assetsTable->setItemDelegate(new QSqlRelationalDelegate(ui->assetsTable));
    ui->assetsTable->setColumnHidden(model->fieldIndex("id"), true);
    ui->assetsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    AssetEditDelegate *delegate = new AssetEditDelegate(this);
    mapper->setItemDelegate(delegate);
    mapper->setModel(model);
    mapper->addMapping(ui->quantityEdit, model->fieldIndex("quantity"));

    QSqlTableModel *relationModel = model->relationModel(currency_column_index);
    relationModel->sort(relationModel->fieldIndex("code"), Qt::AscendingOrder);
    ui->currencyComboBox->setModel(relationModel);
    ui->currencyComboBox->setModelColumn(relationModel->fieldIndex("code"));
    ui->currencyComboBox->installEventFilter(delegate);
    mapper->addMapping(ui->currencyComboBox, currency_column_index);

    connect(ui->assetsTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));

    connect(delegate, SIGNAL(dataChanged (const QModelIndex&)), this, SLOT(data_changed(QModelIndex)));

    ui->assetsTable->setCurrentIndex(model->index(0, 0));

    refreshMarketData(false);

    for (int c = 0; c < ui->assetsTable->horizontalHeader()->count(); ++c)
        ui->assetsTable->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);

}

void MainWindow::data_changed(const QModelIndex& index) {
    refreshMarketDataRow(index.row(), false);
}

void MainWindow::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "App Error", err.text());
}

MainWindow::~MainWindow()
{
    model->submitAll();
    delete ui;
    delete market;
    delete db_helper;
}

void MainWindow::updateCurrencies() {
    QList<Currency> list;
    market->getCurrenciesList(list);
    QSqlError err = db_helper->updateCurrenciesTable(list);
    if (err.type() != QSqlError::NoError) {
        showError(err);
        return;
    }
}

void MainWindow::refreshMarketData(bool force_refresh)
{
    for(int i = 0; i < model->rowCount(); ++i)
        refreshMarketDataRow(i, force_refresh);
}

void MainWindow::refreshMarketDataRow(int row_index, bool force_refresh) {
    double btc_usd_rate = market->getBtcUsdRate();

    QSqlRecord r = model->record(row_index);

    QString currency_ticker = model->data(model->index(row_index, currency_column_index), Qt::DisplayRole).toString();
    QSqlField f_quantity = r.field(quantity_index);
    QSqlField f_price_btc = r.field(price_btc_index);
    QSqlField f_market_value_btc("market_value_btc", QVariant::Double);
    QSqlField f_price_usd = r.field(price_usd_index);
    QSqlField f_market_value_usd("market_value_usd", QVariant::Double);

    double quantity = f_quantity.value().toDouble();
    PriceValue price = market->getBtcPrice(currency_ticker, force_refresh);

    f_price_btc.setValue(QVariant(price.value));
    f_market_value_btc.setValue(QVariant(quantity * price.value));

    f_price_usd.setValue(QVariant(price.value * btc_usd_rate));
    f_market_value_usd.setValue(QVariant(quantity * price.value * btc_usd_rate));

    r.replace(price_btc_index, f_price_btc);
    r.replace(mv_btc_index, f_market_value_btc);
    r.replace(price_usd_index, f_price_usd);
    r.replace(mv_usd_index, f_market_value_usd);

    model->setRecord(row_index,r);
}

void MainWindow::on_removeAssetButton_clicked()
{
    int row = ui->assetsTable->currentIndex().row();
    model->removeRow(row);
    model->submitAll();
}

void MainWindow::on_addAssetButton_clicked()
{
    QSqlRecord record;
    model->insertRecord(model->rowCount(), record);
    model->submitAll();
}

void MainWindow::on_currencyComboBox_currentIndexChanged(int index)
{
    int row = ui->assetsTable->currentIndex().row();
    QVariant current_value = model->data(model->index(row, currency_column_index), Qt::DisplayRole);

    QSqlTableModel *childModel = model->relationModel(currency_column_index);
    int childColIndex = childModel->fieldIndex("code");
    QVariant new_value = childModel->data(childModel->index(index, childColIndex), Qt::DisplayRole);

    if(current_value != new_value) {
        mapper->submit();
    }
}

void MainWindow::on_refreshMarketButton_clicked()
{
    refreshMarketData(true);
}
