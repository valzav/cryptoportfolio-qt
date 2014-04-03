#include "assetdelegate.h"

#include <QtWidgets>

AssetDelegate::AssetDelegate(QObject *parent)
    : QSqlRelationalDelegate(parent)
{
}

void AssetDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
        QStyleOptionViewItemV3 opt = option;
        opt.rect.adjust(0, 0, -1, -1); // since we draw the grid ourselves
        QSqlRelationalDelegate::paint(painter, opt, index);

    QPen pen = painter->pen();
    painter->setPen(option.palette.color(QPalette::Mid));
    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
    painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
    painter->setPen(pen);
}

QSize AssetDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{

    return QSqlRelationalDelegate::sizeHint(option, index) + QSize(1, 1); // since we draw the grid ourselves
}

bool AssetDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index)
{
    //if (index.column() != 5)
        return QSqlRelationalDelegate::editorEvent(event, model, option, index);

//    if (event->type() == QEvent::MouseButtonPress) {
//        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
//        int stars = qBound(0, int(0.7 + qreal(mouseEvent->pos().x()
//            - option.rect.x()) / star.width()), 5);
//        model->setData(index, QVariant(stars));
//        return false; //so that the selection can change
//    }

    return true;
}

QWidget *AssetDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    if (index.column() != 4)
        return QSqlRelationalDelegate::createEditor(parent, option, index);

    // for editing the year, return a spinbox with a range from -1000 to 2100.
    QSpinBox *sb = new QSpinBox(parent);
    sb->setFrame(false);
    sb->setMaximum(2100);
    sb->setMinimum(-1000);

    return sb;
}

