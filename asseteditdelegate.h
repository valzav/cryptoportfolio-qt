#ifndef ASSETEDITDELEGATE_H
#define ASSETEDITDELEGATE_H

#include <QItemDelegate>

class AssetEditDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit AssetEditDelegate(QObject *parent = 0);
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

signals:
    void dataChanged(const QModelIndex &index) const;

public slots:

};

#endif // ASSETEDITDELEGATE_H
