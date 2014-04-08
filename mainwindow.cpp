#include <QSqlError>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "initdb.h"
#include "assetdelegate.h"
#include "asseteditdelegate.h"
#include "marketdata.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if (!QSqlDatabase::drivers().contains("QSQLITE"))
        QMessageBox::critical(this, "Unable to load database", "This demo needs the SQLITE driver");

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/Users/vz/work/cryptoportfolio/assets.db");

    if (!db.open()) { showError(db.lastError()); return; }

    QStringList tables = db.tables();
    if (!tables.contains("assets", Qt::CaseInsensitive)) {
        // initialize the database
        QSqlError err = initDb();
        if (err.type() != QSqlError::NoError) {
            showError(err);
            return;
        }
    }

    // Create the data model
    model = new QSqlRelationalTableModel(ui->assetsTable);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("assets");

    currency_column_index = model->fieldIndex("currency_id");
    model->setRelation(currency_column_index, QSqlRelation("currencies", "id", "code"));

    model->setHeaderData(currency_column_index, Qt::Horizontal, tr("Currency"));
    model->setHeaderData(model->fieldIndex("quantity"), Qt::Horizontal, tr("Quantity"));
    model->setHeaderData(model->fieldIndex("price_btc"), Qt::Horizontal, tr("Price (BTC)"));
    model->setHeaderData(model->fieldIndex("market_value_btc"), Qt::Horizontal, tr("Market Value (BTC)"));
    model->setHeaderData(model->fieldIndex("price_usd"), Qt::Horizontal, tr("Price (USD)"));
    model->setHeaderData(model->fieldIndex("market_value_usd"), Qt::Horizontal, tr("Market Value (USD)"));
    //model->setHeaderData(model->fieldIndex("year"), Qt::Horizontal, tr("Year"));

    // Populate the model
    if (!model->select()) {
        showError(model->lastError());
        return;
    }

    // Set the model and hide the ID column
    ui->assetsTable->setModel(model);
    //ui->assetsTable->setItemDelegate(new AssetDelegate(ui->assetsTable));
    ui->assetsTable->setItemDelegate(new QSqlRelationalDelegate(ui->assetsTable));
    ui->assetsTable->setColumnHidden(model->fieldIndex("id"), true);
    //ui->assetsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->assetsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    //mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    //mapper->setItemDelegate(new QItemDelegate(this));
    AssetEditDelegate *delegate = new AssetEditDelegate(this);
    mapper->setItemDelegate(delegate);
    mapper->setModel(model);
    mapper->addMapping(ui->quantityEdit, model->fieldIndex("quantity"));
    //mapper->addMapping(ui->yearEdit, model->fieldIndex("year"));

    //ui->currencyComboBox->view()->setEditTriggers(QAbstractItemView::AllEditTriggers);
    QSqlTableModel *relationModel = model->relationModel(currency_column_index);
    ui->currencyComboBox->setModel(relationModel);
    ui->currencyComboBox->setModelColumn(relationModel->fieldIndex("code"));
    ui->currencyComboBox->installEventFilter(delegate);
    mapper->addMapping(ui->currencyComboBox, currency_column_index);


    connect(ui->assetsTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));

    ui->assetsTable->setCurrentIndex(model->index(0, 0));

    updateMarketValue();

    for (int c = 0; c < ui->assetsTable->horizontalHeader()->count(); ++c)
        ui->assetsTable->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);
}

void MainWindow::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}

MainWindow::~MainWindow()
{
    model->submitAll();
    db.close();
    delete ui;
}

void MainWindow::updateMarketValue()
{
    int quantity_index = model->fieldIndex("quantity");
    int price_btc_index = model->fieldIndex("price_btc");
    int mv_btc_index = model->fieldIndex("market_value_btc");
    int price_usd_index = model->fieldIndex("price_usd");
    int mv_usd_index = model->fieldIndex("market_value_usd");
    QScopedPointer<MarketDataProvider> mdp(new CryptocoinChartsMDP());
    double btc_usd_rate = mdp->getBtcUsdRate();
    for(int i = 0; i < model->rowCount(); ++i) {
        QSqlRecord r = model->record(i);

        QSqlField f_currency = r.field(currency_column_index);
        QSqlField f_quantity = r.field(quantity_index);
        QSqlField f_price_btc = r.field(price_btc_index);
        QSqlField f_market_value_btc("market_value_btc", QVariant::Double);
        QSqlField f_price_usd = r.field(price_usd_index);
        QSqlField f_market_value_usd("market_value_usd", QVariant::Double);

        double quantity = f_quantity.value().toDouble();
        PriceValue price = mdp->getBtcPrice(f_currency.value().toString());

        f_price_btc.setValue(QVariant(price.value));
        f_market_value_btc.setValue(QVariant(quantity * price.value));

        f_price_usd.setValue(QVariant(price.value * btc_usd_rate));
        f_market_value_usd.setValue(QVariant(quantity * price.value * btc_usd_rate));

        r.replace(price_btc_index, f_price_btc);
        r.replace(mv_btc_index, f_market_value_btc);
        r.replace(price_usd_index, f_price_usd);
        r.replace(mv_usd_index, f_market_value_usd);

        model->setRecord(i,r);
    }
}

void MainWindow::on_removeAssetButton_clicked()
{
    //QMessageBox::warning(this, "button clicked", "Remove Asset");
    int row = ui->assetsTable->currentIndex().row();
    model->removeRow(row);
    model->submitAll();
    //model->select();
}

void MainWindow::on_addAssetButton_clicked()
{
    //QMessageBox::warning(this, "button clicked", "Add Asset");
    QSqlRecord record;
    QSqlField f1("currency_id", QVariant::String);
    QSqlField f2("quantity", QVariant::Int);
    f1.setValue(QVariant(2));
    f2.setValue(QVariant(1));
    record.append(f1);
    record.append(f2);
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
        qDebug() << "---- on_currencyComboBox_currentIndexChanged" << current_value << "->" << new_value;
        mapper->submit();
    }
}
