#include <QSqlError>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "initdb.h"
#include "assetdelegate.h"
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
    model->setRelation(model->fieldIndex("currency_id"), QSqlRelation("currencies", "id", "code"));

    model->setHeaderData(model->fieldIndex("currency_id"), Qt::Horizontal, tr("Currency"));
    model->setHeaderData(model->fieldIndex("quantity"), Qt::Horizontal, tr("Quantity"));
    model->setHeaderData(model->fieldIndex("market_value_btc"), Qt::Horizontal, tr("Market Value (BTC)"));
    model->setHeaderData(model->fieldIndex("market_value_usd"), Qt::Horizontal, tr("Market Value (USD)"));
    //model->setHeaderData(model->fieldIndex("year"), Qt::Horizontal, tr("Year"));

    // Populate the model
    if (!model->select()) {
        showError(model->lastError());
        return;
    }

    // Set the model and hide the ID column
    ui->assetsTable->setModel(model);
    ui->assetsTable->setItemDelegate(new AssetDelegate(ui->assetsTable));
    ui->assetsTable->setColumnHidden(model->fieldIndex("id"), true);
    ui->assetsTable->setSelectionMode(QAbstractItemView::SingleSelection);

//    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
//    mapper->setModel(model);
//    mapper->setItemDelegate(new AssetDelegate(this));
//    mapper->addMapping(ui->titleEdit, model->fieldIndex("title"));
//    mapper->addMapping(ui->yearEdit, model->fieldIndex("year"));

//    connect(ui->assetsTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
//            mapper, SLOT(setCurrentModelIndex(QModelIndex)));

    ui->assetsTable->setCurrentIndex(model->index(0, 0));

    updateMarketValue();
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
    int currency_index = 1;//model->fieldIndex("currency_id");
    int quantity_index = model->fieldIndex("quantity");
    int mv_btc_index = model->fieldIndex("market_value_btc");
    QScopedPointer<MarketDataProvider> mdp(new CryptocoinChartsMDP());
    for(int i = 0; i < model->rowCount(); ++i) {
        QSqlRecord r = model->record(i);
        QSqlField f_currency = r.field(currency_index);
        QSqlField f_quantity = r.field(quantity_index);
        double quantity = f_quantity.value().toDouble();
        QSqlField f_market_value_btc("market_value_btc", QVariant::Double);
        PriceValue price = mdp->getBtcPrice(f_currency.value().toString());
        qDebug() << "price.value=" << price.value;
        qDebug() << "f_market_value_btc=" << quantity * price.value;
        f_market_value_btc.setValue(QVariant(quantity * price.value));
        r.replace(mv_btc_index, f_market_value_btc);
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
    //QSqlField f1("title", QVariant::String);
    //QSqlField f2("year", QVariant::Int);
    //f1.setValue(QVariant(""));
    //f2.setValue(QVariant(0));
    //record.append(f1);
    //record.append(f2);
    model->insertRecord(model->rowCount(), record);
}
