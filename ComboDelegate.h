#ifndef COMBODELEGATE_H
#define COMBODELEGATE_H

#include <QStyledItemDelegate>

class ComboDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit ComboDelegate(const QStringList &options, QObject *parent = nullptr);

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;
    void setOptions(const QStringList& options);
signals:
    // 值改变信号
    void valueChanged(const QModelIndex &index, const QString &newValue);

private:
    QStringList m_options;
    QModelIndex *m_idnex;
    QMap<QWidget*, QString> m_editorInitialValues; // 记录编辑器初始值
    
};

#endif // COMBODELEGATE_H
