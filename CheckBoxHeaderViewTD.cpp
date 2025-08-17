#include "CheckBoxHeaderViewTD.h"
#include <QPainter>
#include <QApplication>
#include <QStyle>

CheckBoxHeaderViewTD::CheckBoxHeaderViewTD(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent) {}

void CheckBoxHeaderViewTD::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const {
    // 先绘制默认的header
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();
    // 只在第一列绘制复选框
    if (logicalIndex == 0) {

        Qt::CheckState state = getCheckState();

        QStyleOptionButton option;
        option.rect = getCheckRect(rect);

        option.state = QStyle::State_Enabled;

        if (state == Qt::Checked) {
            option.state |= QStyle::State_On;
        } else if (state == Qt::PartiallyChecked) {
            option.state |= QStyle::State_NoChange;
        } else {
            option.state |= QStyle::State_Off;
        }

        QApplication::style()->drawControl(QStyle::CE_CheckBox, &option, painter);
    }
}

void CheckBoxHeaderViewTD::mousePressEvent(QMouseEvent *event) {

    int logicalIndex = logicalIndexAt(event->pos());
    if (logicalIndex == 0) {
        const int sectionPos = sectionViewportPosition(logicalIndex);
        const int sectionSize = this->sectionSize(logicalIndex);
        QRect sectionRect(sectionPos, 0, sectionSize, height());
        QRect checkRect = getCheckRect(sectionRect);

        if (checkRect.contains(event->pos())) {
            Qt::CheckState current = getCheckState();
            bool selectAll = (current != Qt::Checked);
            emit selectAllRequested(selectAll);
            updateSection(logicalIndex);
        }
    }
}


Qt::CheckState CheckBoxHeaderViewTD::getCheckState() const {
    const QAbstractItemModel *m = model();
    if (!m || m->rowCount() == 0) return Qt::Unchecked;

    int checkedCount = 0;
    const int rowCount = m->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        Qt::CheckState state = static_cast<Qt::CheckState>(
            m->data(m->index(row, 0), Qt::CheckStateRole).toInt());
        if (state == Qt::Checked) ++checkedCount;
    }

    if (checkedCount == 0) return Qt::Unchecked;
    if (checkedCount == rowCount) return Qt::Checked;
    return Qt::PartiallyChecked;
}

QRect CheckBoxHeaderViewTD::getCheckRect(const QRect& rect) const {
    QStyleOptionButton option;
    option.initFrom(this);
    QRect checkBoxRect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option, this);

    // 根据对齐方式调整位置
    switch (m_align) {
    case Qt::AlignmentFlag::AlignLeft:
        checkBoxRect.moveLeft(rect.left() + 4);  // 左侧保留4px边距
        break;
    case Qt::AlignmentFlag::AlignRight:
        checkBoxRect.moveRight(rect.right() - 4); // 右侧保留4px边距
        break;
    default:  // 默认居中
        checkBoxRect.moveCenter(rect.center());
    }

    // 垂直方向始终居中
    checkBoxRect.moveTop(rect.top() + (rect.height() - checkBoxRect.height()) / 2);

    return checkBoxRect;
}

// 在CheckBoxHeaderView中添加模型变化监听
void CheckBoxHeaderViewTD::setModel(QAbstractItemModel *model) {
    QHeaderView::setModel(model);
    connect(model, &QAbstractItemModel::dataChanged, this, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight){
        if (topLeft.column() <= 0 && bottomRight.column() >= 0) {
            updateSection(0);
        }
    });

    connect(model, &QAbstractItemModel::rowsInserted, this, [this](){
        updateSection(0);
    });

    connect(model, &QAbstractItemModel::rowsRemoved, this, [this](){
        updateSection(0);
    });
}

void CheckBoxHeaderViewTD::setAlignment(Qt::AlignmentFlag align)
{
    m_align = align;
    updateSection(0);
}
