#include "asseteditdelegate.h"
#include <QtCore>
#include <QtSql>

AssetEditDelegate::AssetEditDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}


void AssetEditDelegate::setEditorData(QWidget *editor,
                          const QModelIndex &index) const
   {

    qDebug() << "--- Delegate::setEditorData" << index.data();

    //QSqlRelationalTableModel *sqlModel = qobject_cast<QSqlRelationalTableModel *>(model);
    //QSqlTableModel *childModel = sqlModel ? sqlModel->relationModel(index.column()) : 0;
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    if (!combo) {
        QItemDelegate::setEditorData(editor, index);
        return;
    }
    combo->setCurrentIndex(combo->findText(index.data().toString()));
   }

void AssetEditDelegate::setModelData(QWidget *editor,
                     QAbstractItemModel *model,
                     const QModelIndex &index) const
{
    qDebug() << "ItemDelegate::setModelData";
    //QItemDelegate::setModelData(editor, model, index);
    //return;

    if (!index.isValid())
        return;

    QSqlRelationalTableModel *sqlModel = qobject_cast<QSqlRelationalTableModel *>(model);
    QSqlTableModel *childModel = sqlModel ? sqlModel->relationModel(index.column()) : 0;
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    if (!sqlModel || !childModel || !combo) {
        QItemDelegate::setModelData(editor, model, index);
        return;
    }

    int currentItem = combo->currentIndex();
    int childColIndex = childModel->fieldIndex(sqlModel->relation(index.column()).displayColumn());
    int childEditIndex = childModel->fieldIndex(sqlModel->relation(index.column()).indexColumn());
    QVariant data1 = childModel->data(childModel->index(currentItem, childColIndex), Qt::DisplayRole);
    QVariant data2 = childModel->data(childModel->index(currentItem, childEditIndex), Qt::EditRole);
    qDebug() << "---- setModelData" << data1 << data2;
    sqlModel->setData(index, data1, Qt::DisplayRole);
    sqlModel->setData(index, data2, Qt::EditRole);
}
