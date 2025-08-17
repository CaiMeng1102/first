#include "RowWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontMetrics>
#include <QLineEdit>

RowWidget::RowWidget(int id, int rowHeight, QWidget* parent,
                              RowType type, const QString& label,
                              const QList<QPair<int, QString>>& comboItems,
                              const QStringList& defaultValues)
    : QWidget(parent), m_id(id), m_rowHeight(rowHeight),
      m_type(type), m_comboItems(comboItems), m_defaultValues(defaultValues)
{
    setFixedHeight(m_rowHeight);
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    // 标签列
    m_label = new QLabel(label, this);
    m_label->setFixedHeight(m_rowHeight);
    m_label->setFixedWidth(60);
    m_label->setMargin(0);
    m_label->setStyleSheet("background-color: rgb(217, 237, 248);border: 1px solid #C1C1C1;");
    
    mainLayout->addWidget(m_label);
    // 内容列

    // 删除按钮
    // TableRowWidget.cpp 构造函数部分
    m_deleteButton = new QPushButton(this);
    m_deleteButton->setIcon(QIcon(":/icon/ico/删除.png"));
    const int contentHeight = m_rowHeight - 4;

    // 设置按钮和图标尺寸
    m_deleteButton->setFixedSize(contentHeight, contentHeight);  // contentHeight根据行高计算
    m_deleteButton->setIconSize(QSize(contentHeight-4, contentHeight-4));  // 图标略小于按钮

    // 移除按钮边框和内边距
    m_deleteButton->setStyleSheet(
        "QPushButton {"
        "  border: 1px solid #C1C1C1;"
        "  padding: 0px; "
        "  background: transparent;"
        "}"
        "QPushButton:hover {"
        "  background-color: #ffe0e0;"
        "}"
    );


    connect(m_deleteButton, &QPushButton::clicked, [this](){
        emit deleteRequested(m_id);
    });
    mainLayout->addWidget(m_deleteButton);
    createContentWidget();
    updateComponentHeights();

}


void RowWidget::createContentWidget()
{
    if(m_contentWidget) {
        layout()->removeWidget(m_contentWidget);
        delete m_contentWidget;
    }

    m_contentWidget = new QWidget(this);
    QHBoxLayout* contentLayout = new QHBoxLayout(m_contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    switch(m_type) {
    case RowType::Dropdown:
        setupDropdown();
        break;
    case RowType::Text:
        setupTextInput();
        break;
    case RowType::MultiDropdown:
        setupMultiDropdown();
        break;
    }

    dynamic_cast<QHBoxLayout*>(layout())->insertWidget(1, m_contentWidget);
    updateComponentHeights();
}

void RowWidget::updateComponentHeights()
{
    const int contentHeight = m_rowHeight;

    // 标签
    m_label->setFixedHeight(contentHeight);
    m_label->setAlignment(Qt::AlignVCenter);

    // 内容组件
    m_contentWidget->setFixedHeight(contentHeight);
    if(m_combo) m_combo->setFixedHeight(contentHeight);
    if(m_text) m_text->setFixedHeight(contentHeight);
    if(m_multiCombo) m_multiCombo->setItemHeight(contentHeight);

     // 删除按钮设置
     m_deleteButton->setFixedSize(contentHeight, contentHeight);
     m_deleteButton->setIconSize(QSize(contentHeight, contentHeight));
    // 字体大小自适应
    const int fontSize = qMax(12, (contentHeight-8)/3);
    setStyleSheet(QString(
        "QLabel, QComboBox, QLineEdit { font-size: %1px; }"
    ).arg(fontSize));
}

void RowWidget::setupDropdown()
{
    m_combo = new QComboBox(m_contentWidget);
    m_combo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    for (const auto& item : m_comboItems) {
        m_combo->addItem(item.second, item.first);
    }
    m_combo->setCurrentIndex(-1);  // 不选中任何项

    if (!m_defaultValues.isEmpty()) {
        m_combo->setCurrentText(m_defaultValues.first());
    }
    connect(m_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &RowWidget::handleComboChange);
    m_contentWidget->layout()->addWidget(m_combo);
}

void RowWidget::setupTextInput()
{
    m_text = new QLineEdit(m_defaultValues.join("|"), m_contentWidget);
    m_text->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    connect(m_text, &QLineEdit::textChanged,
            this, &RowWidget::handleTextChange);
    m_contentWidget->layout()->addWidget(m_text);
}

void RowWidget::setupMultiDropdown()
{
    m_multiCombo = new CheckableComboBox(m_contentWidget);
    for (const auto& item : m_comboItems) {
        m_multiCombo->addItem(item.first, item.second,
                             m_defaultValues.contains(QString::number(item.first)));
    }

    QList<int> ids;
    for (const QString& str : m_defaultValues) {
        bool ok;
        int num = str.toInt(&ok);
        if (ok) {
            ids.append(num);
        }
    }
    m_multiCombo->setCheckedIds(ids);
    connect(m_multiCombo, &CheckableComboBox::checkedChanged,
            this, &RowWidget::handleMultiSelectionChange);
    m_contentWidget->layout()->addWidget(m_multiCombo);
}

void RowWidget::setType(RowType newType)
{
    if (m_type == newType) return;
    m_type = newType;
    createContentWidget();  // 重新创建内容部件
    emit valueChanged(m_id, m_type, currentOptionIds(), {currentValue()});
}

void RowWidget::setLabel(const QString& text)
{
    m_label->setText(text);
    updateGeometry();
}

QString RowWidget::currentValue() const
{
    switch(m_type) {
    case RowType::Dropdown:
        return m_combo ? m_combo->currentText() : "";
    case RowType::Text:
        return m_text ? m_text->text() : "";
    case RowType::MultiDropdown:
        return m_multiCombo ? m_multiCombo->checkedTexts().join("|") : "";
    }
    return "";
}

QList<int> RowWidget::currentOptionIds() const
{
    switch(m_type) {
    case RowType::Dropdown:
        return m_combo ? QList<int>{m_combo->currentData().toInt()} : QList<int>();
    case RowType::MultiDropdown:
        return m_multiCombo ? m_multiCombo->checkedIds() : QList<int>();
    default:
        return QList<int>();
    }
}

void RowWidget::setRowHeight(int height)
{
    m_rowHeight = qMax(20, height);
    setFixedHeight(m_rowHeight);
    updateComponentHeights();
    updateGeometry();
}

// 信号处理 ================================================

void RowWidget::handleComboChange(int index)
{
    Q_UNUSED(index)
    emit valueChanged(m_id,
                    RowType::Dropdown,
                    {m_combo->currentData().toInt()},
                    {m_combo->currentText()});
}

void RowWidget::handleTextChange(const QString& text)
{
    emit valueChanged(m_id,
                    RowType::Text,
                    QList<int>(),
                    {text});
}

void RowWidget::handleMultiSelectionChange()
{
    if(!m_multiCombo) return;
    emit valueChanged(m_id,
                     RowType::MultiDropdown,
                     m_multiCombo->checkedIds(),
                     m_multiCombo->checkedTexts());
}

// 辅助方法 ================================================

void RowWidget::clearContentWidget()
{
    if(m_contentWidget) {
        layout()->removeWidget(m_contentWidget);
        delete m_contentWidget;
        m_contentWidget = nullptr;
        m_combo = nullptr;
        m_text = nullptr;
        m_multiCombo = nullptr;
    }
}
