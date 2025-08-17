#ifndef MULTICOMBOBOX_H
#define MULTICOMBOBOX_H

#include <QComboBox>
#include <QListWidget>
#include <QHash>
#include "CheckBoxDelegateRW.h"

class CheckableComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit CheckableComboBox(QWidget* parent = nullptr);

    void addItem(int id, const QString& text, bool checked = false);
    void setCheckedIds(const QList<int>& ids);
    QList<int> checkedIds() const;
    QStringList checkedTexts() const;
    void setItemHeight(int height);
    void setItems(const QMap<int, QString>& items);

signals:
    void checkedChanged(const QList<int>& ids, const QStringList& texts);

protected:
    void showPopup() override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void updateDisplayText();
    void handleItemClicked(const QModelIndex& index);

private:
    QListWidget* m_listWidget;
    QLineEdit* m_displayText;
    QHash<int, QString> m_idToText;
    QHash<QString, int> m_textToId;
    CheckBoxDelegateRW* m_delegate;
    int m_itemHeight = 28;
    void setStyle();
};

#endif // MULTICOMBOBOX_H
