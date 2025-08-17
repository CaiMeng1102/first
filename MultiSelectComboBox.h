#ifndef MULTISELECTCOMBOBOX_H
#define MULTISELECTCOMBOBOX_H

#include <QComboBox>
#include <QListWidget>
#include <QLineEdit>

class MultiSelectComboBox : public QComboBox {
    Q_OBJECT
public:
    explicit MultiSelectComboBox(QWidget *parent = nullptr);

    QStringList checkedItems() const;
    void setCheckedItems(const QStringList &items);
    void addItem(const QString &text);

signals:
    void selectionChanged();

private slots:
    void onItemChanged(QListWidgetItem *item);

private:
    void updateText();
    QListWidget *listWidget;
};

#endif // MULTISELECTCOMBOBOX_H
