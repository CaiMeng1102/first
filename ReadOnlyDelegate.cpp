#include "ReadOnlyDelegate.h"

ReadOnlyDelegate::ReadOnlyDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

QWidget* ReadOnlyDelegate::createEditor(QWidget */*parent*/,
                                      const QStyleOptionViewItem &/*option*/,
                                      const QModelIndex &/*index*/) const {
    return nullptr; // 禁止创建编辑器
}
