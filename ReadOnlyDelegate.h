#ifndef READONLYDELEGATE_H
#define READONLYDELEGATE_H

#include <QStyledItemDelegate>

class ReadOnlyDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit ReadOnlyDelegate(QObject *parent = nullptr);

    QWidget* createEditor(QWidget *parent,
                        const QStyleOptionViewItem &option,
                        const QModelIndex &index) const override;
};

#endif // READONLYDELEGATE_H
