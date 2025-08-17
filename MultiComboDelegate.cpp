#include "MultiComboDelegate.h"
#include "MultiSelectComboBox.h"

MultiComboDelegate::MultiComboDelegate(const QStringList &options, QObject *parent)
: QStyledItemDelegate(parent), m_options(options)
{

}

QWidget *MultiComboDelegate::createEditor(QWidget *parent,
                                   const QStyleOptionViewItem &,
                                   const QModelIndex &) const {
    MultiSelectComboBox *editor = new MultiSelectComboBox(parent);
    for (const QString &option : m_options) {
        editor->addItem(option);
    }
    return editor;
}

void MultiComboDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const {
    QString value = index.model()->data(index, Qt::EditRole).toString();
    MultiSelectComboBox *combo = static_cast<MultiSelectComboBox*>(editor);
    combo->setCheckedItems(value.split("|"));
}

void MultiComboDelegate::setModelData(QWidget *editor,
                                QAbstractItemModel *model,
                                const QModelIndex &index) const {
    MultiSelectComboBox *combo = static_cast<MultiSelectComboBox*>(editor);
    const QString newValue = combo->checkedItems().join("|");
    model->setData(index, newValue, Qt::EditRole);
    emit const_cast<MultiComboDelegate*>(this)->valueChanged(index, newValue);
}

void MultiComboDelegate::updateEditorGeometry(QWidget *editor,
                                       const QStyleOptionViewItem &option,
                                       const QModelIndex &) const {
    editor->setGeometry(option.rect);
}
