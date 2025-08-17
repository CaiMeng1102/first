#include "CheckableComboBox.h"
#include <QLineEdit>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>



CheckableComboBox::CheckableComboBox(QWidget* parent)
    : QComboBox(parent), m_delegate(new CheckBoxDelegateRW(this))
{
    setEditable(true);
    m_displayText = lineEdit();
    m_displayText->setReadOnly(true);
    m_displayText->installEventFilter(this);

    m_listWidget = new QListWidget(this);
    m_listWidget->setItemDelegate(m_delegate);
    m_listWidget->setSelectionMode(QAbstractItemView::NoSelection);

    setModel(m_listWidget->model());
    setView(m_listWidget);

    connect(m_listWidget, &QListWidget::clicked,
            this, &CheckableComboBox::handleItemClicked);

    setStyleSheet(
        "QComboBox {"
        "    border: 1px solid gray;"          // 1像素黑色边框
        "    background: white;"
        "    padding-left: 6px;"
        "    padding-right: 20px;"              // 为下拉按钮预留空间
        "}"
        "QComboBox::drop-down {"
        "    background: transparent;"          // 下拉按钮背景透明
        "    border: none;"                     // 无边框
        "    width: 20px;"                      // 按钮宽度
        "}"
        "QComboBox::down-arrow {"
        "    image: url(:/icon/ico/treeopen.png);"// 自定义箭头图标路径
        "    width: 12px;"
        "    height: 12px;"
        "}"
        "QListWidget {"
        "    border: 1px solid gray;"
        "    background-color: white;"
        "}"
        "QListWidget::item {"
        "    padding: 2px;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: #E1F5FE;"
        "}"
    );
}

void CheckableComboBox::addItem(int id, const QString& text, bool checked)
{
    m_idToText.insert(id, text);
    m_textToId.insert(text, id);

    QListWidgetItem* item = new QListWidgetItem(text);
    item->setData(Qt::UserRole, id);
    item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    m_listWidget->addItem(item);
}

void CheckableComboBox::setCheckedIds(const QList<int>& ids)
{
    for (int i = 0; i < m_listWidget->count(); ++i) {
        QListWidgetItem* item = m_listWidget->item(i);
        int itemId = item->data(Qt::UserRole).toInt();
        item->setCheckState(ids.contains(itemId) ? Qt::Checked : Qt::Unchecked);
    }
    updateDisplayText();
}

QList<int> CheckableComboBox::checkedIds() const
{
    QList<int> ids;
    for (int i = 0; i < m_listWidget->count(); ++i) {
        QListWidgetItem* item = m_listWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            ids << item->data(Qt::UserRole).toInt();
        }
    }
    return ids;
}

QStringList CheckableComboBox::checkedTexts() const
{
    QStringList texts;
    for (int i = 0; i < m_listWidget->count(); ++i) {
        QListWidgetItem* item = m_listWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            texts << item->text();
        }
    }
    return texts;
}

void CheckableComboBox::setItemHeight(int height)
{
    m_itemHeight = height;
    m_delegate->setItemSize(QSize(width(), height));
}

void CheckableComboBox::showPopup()
{
    QComboBox::showPopup();
    m_listWidget->setMinimumWidth(width());
    m_delegate->setItemSize(QSize(m_listWidget->width(), m_itemHeight));
}

bool CheckableComboBox::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_displayText && event->type() == QEvent::MouseButtonPress) {
        showPopup();
        return true;
    }
    return QComboBox::eventFilter(watched, event);
}

void CheckableComboBox::updateDisplayText()
{
    m_displayText->setText(checkedTexts().join("|"));
    m_displayText->setCursorPosition(0);
}

void CheckableComboBox::handleItemClicked(const QModelIndex& index)
{
    QListWidgetItem* item = m_listWidget->item(index.row());
    item->setCheckState(item->checkState() == Qt::Checked
                        ? Qt::Unchecked
                        : Qt::Checked);
    updateDisplayText();
    emit checkedChanged(checkedIds(), checkedTexts());
}

void CheckableComboBox::setItems(const QMap<int, QString>& items)
{
    // 清除原有数据
    m_listWidget->clear();
    m_idToText.clear();
    m_textToId.clear();

    // 添加新数据
    QMapIterator<int, QString> it(items);
    while (it.hasNext()) {
        it.next();
        addItem(it.key(), it.value(), false);
    }

    updateDisplayText();
}
//--------修改下拉框显示-------

