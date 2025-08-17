#include "SwitchButtonDelegate.h"
#include <QPainter>
#include <QMouseEvent>
#include <QEvent>

SwitchButtonDelegate::SwitchButtonDelegate(QObject* parent)
    : QStyledItemDelegate(parent),
      m_onColor(Qt::green),
      m_offColor(Qt::gray),
      m_borderRadius(10),
      m_buttonSize(20) {}

void SwitchButtonDelegate::paint(QPainter* painter,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index) const {
    // 1. 绘制背景（不调用父类的 paint，避免默认绘制 CheckBox）
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }
    else {
        painter->fillRect(option.rect, option.palette.base());
    }

    // 2. 获取开关状态
    bool isOn = index.data(Qt::CheckStateRole).toBool();

    // 3. 计算按钮位置（居中）
    const int totalWidth = m_buttonSize * 2;  // 开关总宽度
    const int xPos = option.rect.x() + (option.rect.width() - totalWidth) / 2;
    const int yPos = option.rect.y() + (option.rect.height() - m_buttonSize) / 2;

    QRect buttonRect(xPos, yPos, totalWidth, m_buttonSize);

    // 4. 绘制背景轨道
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(isOn ? m_onColor : m_offColor);
    painter->drawRoundedRect(buttonRect, m_borderRadius, m_borderRadius);

    // 5. 绘制滑动按钮
    QRect thumbRect = buttonRect.adjusted(2, 2, -2, -2);
    thumbRect.setWidth(m_buttonSize - 4);
    if (isOn) {
        thumbRect.moveLeft(buttonRect.right() - m_buttonSize + 2);
    }
    painter->setBrush(Qt::white);
    painter->drawEllipse(thumbRect);

    painter->restore();

    // 存储按钮位置用于点击检测
    const_cast<SwitchButtonDelegate*>(this)->m_buttonRects[index] = buttonRect;
}

bool SwitchButtonDelegate::editorEvent(QEvent* event,
                                      QAbstractItemModel* model,
                                      const QStyleOptionViewItem& option,
                                      const QModelIndex& index) {
    if(event->type() == QMouseEvent::MouseButtonPress) {
        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        if(m_buttonRects.value(index).contains(e->pos())) {
            // 切换状态
            bool newState = !index.data(Qt::CheckStateRole).toBool();
            model->setData(index, newState, Qt::CheckStateRole);
            emit stateChanged(index, newState);
            return true;
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void SwitchButtonDelegate::setButtonStyle(QColor onColor, QColor offColor,
                                        int borderRadius, int buttonSize) {
    m_onColor = onColor;
    m_offColor = offColor;
    m_borderRadius = borderRadius;
    m_buttonSize = buttonSize;
}
