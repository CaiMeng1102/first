#ifndef COLORDELEGATE_H
#define COLORDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QRect>
#include <QColor>
#include <QMouseEvent>
#include <QCheckBox>

class ColorDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ColorDelegate(QObject *parent = nullptr);

    // 重写绘制方法
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    
    // 重写大小提示
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    
    // 设置颜色方块的大小
    void setColorBoxSize(int size);
    
    // 获取颜色方块的最小列宽
    int getMinimumColumnWidth() const;

signals:
    void colorClicked(const QModelIndex &index);

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, 
                     const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    // 计算颜色方块区域（类似复选框的位置）
    QRect getColorBoxRect(const QStyleOptionViewItem &option) const;
    
    // 颜色方块大小
    int m_colorBoxSize;
    
    // 边框宽度
    int m_borderWidth;
};

#endif // COLORDELEGATE_H 