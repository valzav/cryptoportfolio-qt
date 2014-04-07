#include "asseteditdelegate.h"
#include <QtCore>
#include <QtSql>

AssetEditDelegate::AssetEditDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}


//void AssetEditDelegate::setEditorData(QWidget *editor,
//                          const QModelIndex &index) const
//   {
////     if (!editor->metaObject()->userProperty().isValid()) {
////       if (editor->property("currentIndex").isValid()) {
////         editor->setProperty("currentIndex", index.data());
////         return;
////       }
////     }
//    qDebug() << "Delegate::setEditorData";
//     QItemDelegate::setEditorData(editor, index);
//   }

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
    sqlModel->setData(index,
            childModel->data(childModel->index(currentItem, childColIndex), Qt::DisplayRole),
            Qt::DisplayRole);
    sqlModel->setData(index,
            childModel->data(childModel->index(currentItem, childEditIndex), Qt::EditRole),
            Qt::EditRole);
}
