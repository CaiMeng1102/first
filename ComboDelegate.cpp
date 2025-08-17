#include "ComboDelegate.h"
#include <QComboBox>

ComboDelegate::ComboDelegate(const QStringList &options, QObject *parent)
    : QStyledItemDelegate(parent), m_options(options) {

}

QWidget* ComboDelegate::createEditor(QWidget *parent,
                                   const QStyleOptionViewItem &/*option*/,
                                   const QModelIndex &index) const {

    QComboBox *editor = new QComboBox(parent);
    editor->addItems(m_options); // 使用传入的选项

    ComboDelegate *nonConstThis = const_cast<ComboDelegate*>(this);
    // 连接信号时直接传递新值
    connect(editor, QOverload<int>::of(&QComboBox::currentIndexChanged),
               [nonConstThis, editor, index](int) {
           emit nonConstThis->valueChanged(index, editor->currentText());
       });

    return editor;
}

void ComboDelegate::setEditorData(QWidget *editor,
                                const QModelIndex &index) const {
    QComboBox *combo = static_cast<QComboBox*>(editor);
    combo->setCurrentText(index.data(Qt::DisplayRole).toString());
}

void ComboDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                               const QModelIndex &index) const {
    QComboBox *combo = static_cast<QComboBox*>(editor);
    model->setData(index, combo->currentText());
}

void ComboDelegate::setOptions(const QStringList& options)
{
    m_options = options;
}
