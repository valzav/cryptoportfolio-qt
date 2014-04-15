#include <QtCore>
#include <QtSql>
#include "asseteditdelegate.h"

AssetEditDelegate::AssetEditDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}


void AssetEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    if (!combo)
        QItemDelegate::setEditorData(editor, index);
    else
        combo->setCurrentIndex(combo->findText(index.data().toString()));
}

void AssetEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    if (!index.isValid()) return;
    QSqlRelationalTableModel *sqlModel = qobject_cast<QSqlRelationalTableModel *>(model);
    QSqlTableModel *childModel = sqlModel ? sqlModel->relationModel(index.column()) : 0;
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    if (!sqlModel || !childModel || !combo) {
        QItemDelegate::setModelData(editor, model, index);
    } else {
        int currentItem = combo->currentIndex();
        int childColIndex = childModel->fieldIndex(sqlModel->relation(index.column()).displayColumn());
        int childEditIndex = childModel->fieldIndex(sqlModel->relation(index.column()).indexColumn());
        QVariant data1 = childModel->data(childModel->index(currentItem, childColIndex), Qt::DisplayRole);
        QVariant data2 = childModel->data(childModel->index(currentItem, childEditIndex), Qt::EditRole);
        sqlModel->setData(index, data1, Qt::DisplayRole);
        sqlModel->setData(index, data2, Qt::EditRole);
    }
    emit dataChanged(index);
}
