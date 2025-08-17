#include "CheckBoxDelegateRW.h"
#include <QApplication>


CheckBoxDelegateRW::CheckBoxDelegateRW(QObject* parent)
    : QStyledItemDelegate(parent) {}

void CheckBoxDelegateRW::paint(QPainter* painter,
                            const QStyleOptionViewItem& option,
                            const QModelIndex& index) const
{
    QStyleOptionViewItem viewOption(option);
    if (index.data(Qt::CheckStateRole).toInt() == Qt::Checked) {
        viewOption.state |= QStyle::State_On;
    } else {
        viewOption.state |= QStyle::State_Off;
    }
    QStyledItemDelegate::paint(painter, viewOption, index);
}

QSize CheckBoxDelegateRW::sizeHint(const QStyleOptionViewItem& option,
                                const QModelIndex& index) const
{
    return m_itemSize.isValid() ? m_itemSize
                               : QStyledItemDelegate::sizeHint(option, index);
}

void CheckBoxDelegateRW::setItemSize(const QSize& size)
{
    m_itemSize = size;
}

