#include "ButtonDelegate.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>

ButtonDelegate::ButtonDelegate(const QIcon& icon, QObject* parent)
    : QStyledItemDelegate(parent),
      m_icon(icon),
      m_iconSize(24, 24),
      m_colorNormal("blue"),
      m_colorHover("#87c38f"),
      m_colorPressed("#1296db"),
      m_colorDisabled("#808080") {}


void ButtonDelegate::paint(QPainter* painter,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const {
    // 禁用默认选中效果
    QStyleOptionViewItem opt = option;
    opt.state &= ~QStyle::State_Selected;
    QStyledItemDelegate::paint(painter, opt, index);

    // 获取按钮状态
    const bool enabled = index.data(Qt::UserRole).toBool();
    ButtonState state = StateDisabled;
    if (enabled) {
        if (index == m_pressedIndex) {
            state = StatePressed;
        } else if (index == m_hoverIndex) {
            state = StateHover;
        } else {
            state = StateNormal; // 明确指定默认状态
        }
    }

    // 设置颜色
    QColor color;
    switch(state) {
    case StateNormal: color = m_colorNormal; break;
    case StateHover: color = m_colorHover; break;
    case StatePressed: color = m_colorPressed; break;
    default: color = m_colorDisabled; break;
    }

    // 绘制图标
    if(!m_icon.isNull()) {
        QPixmap pixmap = m_icon.pixmap(m_iconSize);

        // 应用颜色过滤
        QPainter pixPainter(&pixmap);
        pixPainter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        pixPainter.fillRect(pixmap.rect(), color);
        pixPainter.end();

        // 居中绘制
        QRect iconRect = QRect(0, 0, m_iconSize.width(), m_iconSize.height());
        iconRect.moveCenter(option.rect.center());
        painter->drawPixmap(iconRect, pixmap);
    }

    qDebug() << "State:" << state
             << "Color:" << color
             << "Enabled:" << enabled;

    QRect rect = option.rect;
//    rect.setRect(option.rect); // 正确使用方法修改 QRect
     m_buttonRects[index] = option.rect;
}

bool ButtonDelegate::editorEvent(QEvent* event,
                                QAbstractItemModel* model,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index) {
    if(!index.isValid()) return false;

    const QRect btnRect = m_buttonRects.value(index);
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

    switch(event->type()) {
    case QEvent::MouseMove: {
        bool contains = btnRect.contains(mouseEvent->pos());
        if(contains != (index == m_hoverIndex)) {
            m_hoverIndex = contains ? index : QModelIndex();
            model->dataChanged(index, index);
        }
        return contains;
    }
    case QEvent::MouseButtonPress: {
        if(index.data(Qt::UserRole).toBool()) {
            m_pressedIndex = index;
            model->dataChanged(index, index);
            return true;
        }
        break;
    }
    case QEvent::MouseButtonRelease: {
        if(index == m_pressedIndex) {
            m_pressedIndex = QModelIndex();
            if(btnRect.contains(mouseEvent->pos())) {
                emit buttonClicked(index);
            }
            model->dataChanged(index, index);
            return true;
        }
        break;
    }
    case QEvent::Leave: {
        if(m_hoverIndex.isValid()) {
            m_hoverIndex = QModelIndex();
            model->dataChanged(index, index);
        }
        break;
    }
    }
    return false;
}

void ButtonDelegate::setIcon(const QIcon& icon) {
    m_icon = icon;
}

void ButtonDelegate::setIconSize(const QSize& size) {
    m_iconSize = size;
}

void ButtonDelegate::setColors(QColor normal, QColor hover,
                              QColor pressed, QColor disabled) {

    m_colorNormal = normal;
    m_colorHover = hover;
    m_colorPressed = pressed;
    m_colorDisabled = disabled;
}
