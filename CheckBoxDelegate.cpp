#include "CheckBoxDelegate.h"
#include <QApplication>
#include <QMouseEvent>
#include <QDebug>

CheckBoxDelegate::CheckBoxDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

void CheckBoxDelegate::setAlignment(Qt::AlignmentFlag align)
{
    m_align = align;

}

void CheckBoxDelegate::paint(QPainter* painter,
                           const QStyleOptionViewItem& option,
                           const QModelIndex& index) const {
    // 1. 准备基础样式选项(不绘制默认内容)
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.state &= ~QStyle::State_HasFocus;
    opt.features &= ~QStyleOptionViewItem::HasCheckIndicator;

    // 2. 只绘制背景和文本(不绘制复选框)
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

    // 3. 准备并绘制自定义复选框
    QStyleOptionButton checkOpt;
    checkOpt.state = QStyle::State_Enabled;
    checkOpt.state |= (index.data(Qt::CheckStateRole).toBool() ? QStyle::State_On : QStyle::State_Off);
    checkOpt.rect = getCheckRect(option);

    QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkOpt, painter);

}

bool CheckBoxDelegate::editorEvent(QEvent* event,
                                  QAbstractItemModel* model,
                                  const QStyleOptionViewItem& option,
                                  const QModelIndex& index) {
    if(event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* e = static_cast<QMouseEvent*>(event);

        // 检查是否点击在复选框区域
        if(getCheckRect(option).contains(e->pos())) {
            // 切换状态
//            const bool newState = !index.data(Qt::CheckStateRole).toBool();
            if(index.data(Qt::CheckStateRole) == Qt::Checked){
                model->setData(index, Qt::Unchecked, Qt::CheckStateRole);
                emit checkStateChanged(index, false);

            }else{
                model->setData(index, Qt::Checked, Qt::CheckStateRole);
                emit checkStateChanged(index, true);
            }

            // 发射自定义信号
            return true;
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QRect CheckBoxDelegate::getCheckRect(const QStyleOptionViewItem& option) const {
    QStyleOptionButton checkOpt;
    checkOpt.rect = option.rect;  // 使用传入的option作为基础
    checkOpt.state = QStyle::State_Enabled;

    QRect checkRect = QApplication::style()->subElementRect(
        QStyle::SE_CheckBoxIndicator, &checkOpt, qobject_cast<QWidget*>(parent()));

    // 根据对齐方式调整水平位置
    switch (m_align) {
    case Qt::AlignLeft:
        checkRect.moveLeft(option.rect.left() + 4);
        break;
    case Qt::AlignRight:
        checkRect.moveRight(option.rect.right() - 4);
        break;
    default: // AlignCenter
        checkRect.moveCenter(QPoint(option.rect.center().x(), checkRect.center().y()));
    }

    // 垂直方向始终居中
    checkRect.moveTop(option.rect.top() + (option.rect.height() - checkRect.height()) / 2);

    return checkRect;
}