#pragma once
#include <QStyledItemDelegate>
#include <QObject>

class CheckBoxDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit CheckBoxDelegate(QObject* parent = nullptr);
    void setAlignment(Qt::AlignmentFlag align);

    // 必须重写的虚函数
    void paint(QPainter* painter,
              const QStyleOptionViewItem& option,
              const QModelIndex& index) const override;

    bool editorEvent(QEvent* event,
                    QAbstractItemModel* model,
                    const QStyleOptionViewItem& option,
                    const QModelIndex& index) override;

signals:
    // 新增的复选框状态改变信号
    void checkStateChanged(const QModelIndex& index, bool checked);

private:
    QRect getCheckRect(const QStyleOptionViewItem& option) const;
    Qt::AlignmentFlag m_align = Qt::AlignmentFlag::AlignLeft;
    
};
