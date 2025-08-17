#ifndef CHECKBOXDELEGATE_H
#define CHECKBOXDELEGATE_H

#include <QStyledItemDelegate>

class CheckBoxDelegateRW : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CheckBoxDelegateRW(QObject* parent = nullptr);
    void paint(QPainter* painter,
              const QStyleOptionViewItem& option,
              const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                  const QModelIndex& index) const override;
    void setItemSize(const QSize& size);

private:
    QSize m_itemSize = QSize(-1, 28); // 默认高度28px
};

#endif // CHECKBOXDELEGATE_H
