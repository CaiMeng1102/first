#include "MultiSelectComboBox.h"

MultiSelectComboBox::MultiSelectComboBox(QWidget *parent)
    : QComboBox(parent)
{
    listWidget = new QListWidget(this);
    setEditable(true);
    lineEdit()->setReadOnly(true);
    setModel(listWidget->model());
    setView(listWidget);

    // 设置无选择模式，只显示复选框
    listWidget->setSelectionMode(QAbstractItemView::NoSelection);

    connect(listWidget, &QListWidget::itemChanged,
            this, &MultiSelectComboBox::onItemChanged);
}

QStringList MultiSelectComboBox::checkedItems() const {
    QStringList items;
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        if (item->checkState() == Qt::Checked)
            items << item->text();
    }
    return items;
}

void MultiSelectComboBox::setCheckedItems(const QStringList &items) {
    listWidget->blockSignals(true);

    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        item->setCheckState(items.contains(item->text()) ?
                           Qt::Checked : Qt::Unchecked);
    }

    listWidget->blockSignals(false);
    updateText();
}

void MultiSelectComboBox::addItem(const QString &text) {
    QListWidgetItem *item = new QListWidgetItem(text, listWidget);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setCheckState(Qt::Unchecked);
}

void MultiSelectComboBox::onItemChanged(QListWidgetItem *) {
    updateText();
    emit selectionChanged();
}

void MultiSelectComboBox::updateText() {
    lineEdit()->setText(checkedItems().join("|"));
}
