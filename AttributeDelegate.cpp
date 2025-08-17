#include "AttributeDelegate.h"
#include <QLineEdit>
#include "MultiSelectComboBox.h"

AttributeDelegate::AttributeDelegate(QObject * parent)
    : QStyledItemDelegate(parent) {
}

QWidget* AttributeDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const {
    QWidget* editor = nullptr;
    int type = index.data(DelegateTypeRole).toInt();

    switch (type) {
    case TextEditor:
        editor = QStyledItemDelegate::createEditor(parent, option, index);
      /*  if (auto lineEdit = qobject_cast<QLineEdit*>(editor)) {
            connect(lineEdit, &QLineEdit::textChanged,
                [this, index](const QString& text) {
                    emit const_cast<AttributeDelegate*>(this)->valueChanged(index, text);
                });
        }*/
        break;
    case ComboBoxEditor: {
        auto combo = new QComboBox(parent);
        combo->addItems(index.data(ComboItemsRole).toStringList());
        //connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        //    [this, combo, index](int) {
        //        emit const_cast<AttributeDelegate*>(this)->valueChanged(index, combo->currentText());
        //    });
        editor = combo;
        break;
    }case MultiComboEditor: {
        MultiSelectComboBox* combo = new MultiSelectComboBox(parent);
        QStringList list = index.data(ComboItemsRole).toStringList();
        for (auto str :list) {
            combo->addItem(str);
        }
        combo->setCheckedItems(index.data(Qt::EditRole).toString().split("|"));
        editor = combo;
        break;
    }
    default:
        editor = QStyledItemDelegate::createEditor(parent, option, index);
    }
    return editor;
}

void AttributeDelegate::setEditorData(QWidget* editor,
    const QModelIndex& index) const {
    if (auto combo = qobject_cast<QComboBox*>(editor)) {
        combo->setCurrentText(index.data(Qt::EditRole).toString());
    }
    else if(auto combo = qobject_cast<MultiSelectComboBox*>(editor)){
        combo->setCheckedItems(index.data(Qt::EditRole).toString().split("|"));
    }
    else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void AttributeDelegate::setModelData(QWidget* editor,
    QAbstractItemModel* model,
    const QModelIndex& index) const {
    if (auto combo = qobject_cast<QComboBox*>(editor)) {
        model->setData(index, combo->currentText(), Qt::EditRole);
        emit const_cast<AttributeDelegate*>(this)->valueChanged(index, combo->currentText());
    }
    else if(auto combo = qobject_cast<MultiSelectComboBox*>(editor)){
        const QString newValue = combo->checkedItems().join("|");
        model->setData(index, newValue, Qt::EditRole);
        emit const_cast<AttributeDelegate*>(this)->valueChanged(index, newValue);
    }
    else {
        QStyledItemDelegate::setModelData(editor, model, index);
        emit const_cast<AttributeDelegate*>(this)->valueChanged(index, index.data(Qt::EditRole).toString());

    }
}