#include "ColorDelegate.h"
#include <QApplication>
#include <QStyleOptionViewItem>
#include <QDebug>
#include <QStyle>
#include <QStyleOptionButton>

ColorDelegate::ColorDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , m_colorBoxSize(16)  // 默认16x16像素，类似复选框大小
    , m_borderWidth(1)
{
}

void ColorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 先绘制默认的单元格背景（保持单元格原有样式）
    QStyleOptionViewItem modifiedOption = option;
    modifiedOption.text = ""; // 清除文本，我们不需要显示文本
    QStyledItemDelegate::paint(painter, modifiedOption, index);
    
    // 获取要显示的颜色
    QColor displayColor;
    QVariant colorData = index.data(Qt::UserRole + 1); // 使用自定义角色存储颜色
    
    if (colorData.isValid() && colorData.canConvert<QColor>()) {
        displayColor = colorData.value<QColor>();
    } else {
        // 如果没有设置颜色，使用默认白色
        displayColor = QColor(Qt::white);
    }
    
    // 计算颜色方块区域（参考复选框的位置算法）
    QRect colorBoxRect = getColorBoxRect(option);
    
    // 保存当前painter状态
    painter->save();
    
    // 启用抗锯齿
    painter->setRenderHint(QPainter::Antialiasing, true);
    
    // 绘制颜色方块背景
    painter->fillRect(colorBoxRect, displayColor);
    
    // 绘制边框
    QPen borderPen(QColor(128, 128, 128), m_borderWidth);
    painter->setPen(borderPen);
    painter->drawRect(colorBoxRect);
    
    // 如果单元格被选中，绘制选中状态的边框
    if (option.state & QStyle::State_Selected) {
        QPen selectedPen(QColor(0, 120, 215), 2); // 蓝色选中边框
        painter->setPen(selectedPen);
        painter->drawRect(colorBoxRect.adjusted(-1, -1, 1, 1));
    }
    
    // 恢复painter状态
    painter->restore();
}

QSize ColorDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    
    // 获取默认大小
    QSize defaultSize = QStyledItemDelegate::sizeHint(option, index);
    
    // 确保有足够的空间显示颜色方块
    int minWidth = m_colorBoxSize + 20;  // 颜色方块 + 左右边距
    int minHeight = m_colorBoxSize + 10; // 颜色方块 + 上下边距
    
    return QSize(qMax(defaultSize.width(), minWidth), 
                 qMax(defaultSize.height(), minHeight));
}

void ColorDelegate::setColorBoxSize(int size)
{
    if (size > 0) {
        m_colorBoxSize = size;
    }
}

int ColorDelegate::getMinimumColumnWidth() const
{
    return m_colorBoxSize + 20; // 颜色方块大小 + 边距
}

bool ColorDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, 
                               const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(model)
    
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            QRect colorBoxRect = getColorBoxRect(option);
            
            // 检查点击是否在颜色方块区域内
            if (colorBoxRect.contains(mouseEvent->pos())) {
                emit colorClicked(index);
                return true;
            }
        }
    }
    
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QRect ColorDelegate::getColorBoxRect(const QStyleOptionViewItem &option) const
{
    // 参考Qt复选框的居中算法
    QRect cellRect = option.rect;
    
    // 计算颜色方块在单元格中央的位置
    int x = cellRect.x() + (cellRect.width() - m_colorBoxSize) / 2;
    int y = cellRect.y() + (cellRect.height() - m_colorBoxSize) / 2;
    
    return QRect(x, y, m_colorBoxSize, m_colorBoxSize);
} 