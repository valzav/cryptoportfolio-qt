#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QMainWindow>
#include <QtSql>

namespace Ui {
class MainWindow;
}

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

private:

    void showError(const QSqlError &err);
    void updateMarketValue();
    QSqlRelationalTableModel *model;
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QDataWidgetMapper *mapper;
    int currency_column_index;
};

#endif // MAINWINDOW_H
