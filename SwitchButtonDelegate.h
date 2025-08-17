#include <QStyledItemDelegate>
#include <QStyleOptionButton>
#include <QApplication>

class SwitchButtonDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit SwitchButtonDelegate(QObject* parent = nullptr);

    // 必须重写的虚函数
    void paint(QPainter* painter,
              const QStyleOptionViewItem& option,
              const QModelIndex& index) const override;

    bool editorEvent(QEvent* event,
                    QAbstractItemModel* model,
                    const QStyleOptionViewItem& option,
                    const QModelIndex& index) override;

    // 可选样式配置
    void setButtonStyle(QColor onColor = Qt::green,
                       QColor offColor = Qt::gray,
                       int borderRadius = 10,
                       int buttonSize = 20);

signals:
    void stateChanged(const QModelIndex& index, bool state);

private:
    // 样式配置参数
    QColor m_onColor;
    QColor m_offColor;
    int m_borderRadius;
    int m_buttonSize;
    QMap<QModelIndex, QRect> m_buttonRects; // 存储按钮位置:cite[10]
};
