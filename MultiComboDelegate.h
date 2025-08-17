#ifndef COMBO_DELEGATE_H
#define COMBO_DELEGATE_H

#include <QStyledItemDelegate>

class MultiComboDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit MultiComboDelegate(const QStringList &options,QObject *parent = nullptr);

    // QAbstractItemDelegate interface
    QWidget *createEditor(QWidget *parent,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index) const override;
    void setEditorData(QWidget *editor,
                      const QModelIndex &index) const override;
    void setModelData(QWidget *editor,
                     QAbstractItemModel *model,
                     const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const override;

signals:
    void valueChanged(const QModelIndex &index, const QString &newValue);

private:
    QStringList m_options; // 存储可配置的选项列表

};

#endif // COMBO_DELEGATE_H
