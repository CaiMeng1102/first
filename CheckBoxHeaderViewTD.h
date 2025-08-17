#ifndef CHECKBOXHEADERVIEWTD_H
#define CHECKBOXHEADERVIEWTD_H

#include <QHeaderView>
#include <QMouseEvent>
class CheckBoxHeaderViewTD : public QHeaderView {
    Q_OBJECT
public:
    explicit CheckBoxHeaderViewTD(Qt::Orientation orientation, QWidget *parent = nullptr);
    void setModel(QAbstractItemModel *model);
    void setAlignment(Qt::AlignmentFlag align);

signals:
    void selectAllRequested(bool checked);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    Qt::CheckState getCheckState() const;
    QRect getCheckRect(const QRect &sectionRect) const;
    Qt::AlignmentFlag m_align = Qt::AlignmentFlag::AlignLeft;

};

#endif // CHECKBOXHEADERVIEWTD_H
