
#ifndef DYNAMIC_DELEGATE_H
#define DYNAMIC_DELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>

class AttributeDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    enum DataRole {
        DelegateTypeRole = Qt::UserRole + 100,
        ComboItemsRole
    };

    enum DelegateType {
        DefaultEditor,
        TextEditor,
        ComboBoxEditor,
        MultiComboEditor
    };

    explicit AttributeDelegate(QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void setEditorData(QWidget* editor,
        const QModelIndex& index) const override;

    void setModelData(QWidget* editor,
        QAbstractItemModel* model,
        const QModelIndex& index) const override;

signals:
    void valueChanged(const QModelIndex& index, const QString& newValue);

};
#endif
