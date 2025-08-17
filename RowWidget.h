#ifndef TABLEROWWIDGET_H
#define TABLEROWWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include "CheckableComboBox.h"
#include <QLabel>

enum class RowType { Dropdown, Text, MultiDropdown };

class RowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RowWidget(int id,
                           int rowHeight = 40,
                           QWidget* parent = nullptr,
                           RowType type = RowType::Text,
                           const QString& label = "",
                           const QList<QPair<int, QString>>& comboItems = {},
                           const QStringList& defaultValues = {});

    int id() const { return m_id; }
    RowType type() const { return m_type; }
    void setType(RowType newType);
    void setLabel(const QString& text);
    QString currentValue() const;
    QList<int> currentOptionIds() const;
    void setRowHeight(int height);
    int rowHeight() const { return m_rowHeight; }

    void clearContentWidget();
signals:
    void valueChanged(int rowId,
                     RowType type,
                     const QList<int>& optionIds,
                     const QStringList& optionTexts);
    void deleteRequested(int rowId);

private slots:
    void handleComboChange(int index);
    void handleTextChange(const QString& text);
    void handleMultiSelectionChange();

private:
    void createContentWidget();
    void updateComponentHeights();
    void setupDropdown();
    void setupTextInput();
    void setupMultiDropdown();

    int m_id;
    int m_rowHeight;
    RowType m_type;
    QLabel* m_label;
    QWidget* m_contentWidget = nullptr;
    QComboBox* m_combo= nullptr;
    QLineEdit* m_text= nullptr;
    CheckableComboBox* m_multiCombo= nullptr;
    QPushButton* m_deleteButton = nullptr;
    QList<QPair<int, QString>> m_comboItems;
    QStringList m_defaultValues;
};

#endif // TABLEROWWIDGET_H
