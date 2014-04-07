#ifndef ASSETDELEGATE_H
#define ASSETDELEGATE_H

#include <QModelIndex>
#include <QPixmap>
#include <QSize>
#include <QSqlRelationalDelegate>

class Delegate1 : public QItemDelegate
  {
      Q_OBJECT
  public:
      Delegate1(QObject *parent=0);

      virtual ~Delegate1()
       {
           // Do some important cleanup
       }
  };

QT_FORWARD_DECLARE_CLASS(QPainter)

class AssetDelegate : public QSqlRelationalDelegate
{
public:
    AssetDelegate(QObject *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const;

private:
};

#endif // ASSETDELEGATE_H
