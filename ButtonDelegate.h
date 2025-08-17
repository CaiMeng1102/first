#ifndef BUTTONDELEGATE_H
#define BUTTONDELEGATE_H

#include <QStyledItemDelegate>
#include <QIcon>
#include <QHash>

class ButtonDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    enum ButtonState {
        StateNormal,
        StateHover,
        StatePressed,
        StateDisabled
    };

    explicit ButtonDelegate(const QIcon& icon = QIcon(), QObject* parent = nullptr);

    void paint(QPainter* painter,
              const QStyleOptionViewItem& option,
              const QModelIndex& index) const override;

    bool editorEvent(QEvent* event,
                    QAbstractItemModel* model,
                    const QStyleOptionViewItem& option,
                    const QModelIndex& index) override;

    void setIcon(const QIcon& icon);
    void setIconSize(const QSize& size);
    void setColors(QColor normal, QColor hover, QColor pressed, QColor disabled);

signals:
    void buttonClicked(const QModelIndex& index);

private:
    QIcon m_icon;
    QSize m_iconSize;
    mutable QHash<QModelIndex, QRect> m_buttonRects; // 添加 mutable 修饰

    QColor m_colorNormal;
    QColor m_colorHover;
    QColor m_colorPressed;
    QColor m_colorDisabled;

    QModelIndex m_hoverIndex;
    QModelIndex m_pressedIndex;
};

#endif // BUTTONDELEGATE_H
