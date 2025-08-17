#ifndef TABLEDIALOG_H
#define TABLEDIALOG_H

#include <QObject>
#include <QWidget>
#include <QStandardItemModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QTableView>
#include "ButtonDelegate.h"
#include <QHeaderView>
#include "ComboDelegate.h"
#include <QVBoxLayout>
#include "CheckBoxHeaderViewTD.h"
#include "ReadOnlyDelegate.h"
#include <QList>
#include "SwitchButtonDelegate.h"
#include "MultiComboDelegate.h"
#include "CheckBoxDelegate.h"
#include "AttributeDelegate.h"
#include "ColorDelegate.h"
#include<CAD/APP.h>
#include <QVariantMap>
#include "global.h"

//enum columnType{
//   LineSelect,
//   ReadOnly,
//   WriteAble,
//   Combobox,
//   Delete
//};


class TableDialog : public QWidget
{
	Q_OBJECT
public:
	//构造函数传入QJsonObject 表头格式 json格式demoHeaderstr
	explicit TableDialog(const QJsonObject& configObject, QWidget* parent = nullptr);
	//数据初始化加载方法  json 格式为demoDataJsonstr
	void initData(const QJsonArray& jsonData);
	void updateData(const QJsonArray& jsonData);
	//添加行方法
	void sltaddRow();
	//获取当前行列对应的单元格数据
	QString getCellData(int row, int col);
	//获取当前行的数据
	QStringList getRowData(int row);
	//获取当前表格的所有数据 返回的为二维json数组
	QJsonArray currentDataToJson();
	QJsonArray GetTableData();
	//获取当前选中行
	QList<int> currentSelectRow();
	//清除当前选中的行
	void clearCheck();
	void headVisible();
	//设置选中第一行s
	void selectFirstRow();
	QTableView* getTableView();
	void moveSelectedRowUp();
	void moveSelectedRowDown();// 下移当前选中行
	void setComboBoxData(int row, int col, const QStringList& options);
	void disableHorizontalSlider();
	void getAttributeValues(QStringList& list, int type, int attrId);
	void getConditionValues(QStringList& list, int conditionId);
	//! 设置联动数据  
   //! 用于刷新子列数据
	void setDependencyMap(const QJsonObject& jsonOptions);
	//! 设置行选中时的自定义背景色
	//! 保持非白色背景列不变，其他列设置为选中背景色
	void setCustomRowSelection(int row);
	//! 清除所有行的自定义背景色
	void clearCustomRowSelection();
	//! 获取指定单元格的背景色
	QColor getCellBackgroundColor(int row, int col);
	//! 设置指定单元格的背景色
	void setCellBackgroundColor(int row, int col, const QColor& color);
	//! 设置选中时的背景色
	void setSelectionBackgroundColor(const QColor& color);
	//! 测试函数：设置一些单元格为非白色背景用于测试
	void setTestBackgroundColors();
	//! 设置单元格的颜色（用于ColorDialog类型）
	//void setCellColor(int row, int col, const QColor& color);

signals:
	// 行点击信号
	void rowClicked(int row);
	//单元格值改变信号
	void cellupdate(int row, int col, QString value);
	//行删除信号
	void rowDelete(int row);
	//单元格点击信号
	void cellClicked(int row, int column);
	//颜色单元格点击信号
	void colorCellClicked(int row, int column);

public slots:
	// 设置指定单元格的颜色方块颜色（用于ColorDialog类型）
	// 该方法只设置颜色方块的背景色，不弹出颜色选择对话框
	void setCellColor(int row, int col, const QColor& color);

	// 获取指定单元格的颜色方块颜色
	QColor getCellColor(int row, int col) const;

private slots:
	//操作按钮点击
	void handleAction(const QModelIndex& index);
	//删除按钮点击
	void handleDelete(const QModelIndex& index);
	//全选功能实现
	void handleSelectAll(bool checked);
	//行点击事件
	void onRowClicked(const QModelIndex& index);
	//数据变化函数
	void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	void onCheckBoxChanged(const QModelIndex& index, bool checked);
	void onCellClicked(const QModelIndex& index);
	// 更新联动列数据
	void updateDependentColumn(int row, int col, const QString& parentValue);
	//颜色委托点击处理
	void onColorClicked(const QModelIndex& index);

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	void setupModel(const QJsonArray& config);
	void setupUI(const QJsonArray& config);
	void initDataEnable(const QJsonArray& jsonData);

	QTableView* tableView;
	QStandardItemModel* model;
	QJsonArray columnConfig;
	QJsonObject configObject;
	QMap<int, ButtonDelegate*> buttonDelegates; // 存储按钮列的委托
	QMap<int, QString> m_columnTypes; // 列索引 -> 列类型
	QMap<int, QString> m_columnAlgens; // 列索引 -> 列位置（居中、居左、居右）
	QMap<int, bool> m_RowEnables; // 行索引 -> 行可编辑
	QMap<QString, QJsonArray> m_dependencyMap; // 联动下拉框数据  //当前未考虑有相同键的冲突，有需求可添加行列的拼接标识

	int m_currentSelectedRow; // 当前选中的行
	QColor m_selectionBackgroundColor; // 选中时的背景色
	QMap<QPair<int, int>, QColor> m_originalBackgroundColors; // 存储原始背景色 (row, col) -> color

private:
	//demoData的数据顺序要根据demoJsonstr的列顺序
	//enum Alignment { Left, Center, Right };

	QString demoHeaderstr = R"(
                            {
                            "lineClick": false,
                            "arry": [
                                {
                                    "name": "",
                                    "type": "LineSelect",
                                    "width":30
                                },
                                {
                                    "name": "开关",
                                    "type": "Switch",
                                     "width":50
                                },
                                {
                                    "name": "编辑",
                                    "type": "WriteAble",
                                    "width":80
                                },
                                {
                                    "name": "下拉",
                                    "type": "Combobox",
                                    "options": [
                                        "选项A",
                                        "选项B",
                                        "选项C"
                                    ],
                                    "enableMulti":false,
                                   "width":80
                                },
                                {
                                    "name": "只读",
                                    "type": "ReadOnly",
                                       "width":80
                                },
                                {
                                    "name": "颜色",
                                    "type": "ColorDialog",
                                       "width":80
                                },
                                {
                                    "name": "操作",
                                    "type": "Click",
                                       "width":50

                                },
                                {
                                    "name": "删除",
                                    "type": "Delete",
                                   "width":50
                                }
                            ]
                        }
   )";

	QString demoDataJsonstr = R"([
                              {
                                  "enable":false,
                                  "rowdata":[
                                
                                  {"value": "张三"},
                                  {"value": "选项A"},
                                  {"value": "正常"},
                                  {"color": "#FF0000"},
                                  {"value": "", "enable": true},
                                 
                                  ]
                              },
                              {
                              "rowdata":[
                                
                                  {"value": "张三"},
                                  {"value": "选项A"},
                                  {"value": "正常"},
                                  {"color": "#00FF00"},
                                  {"value": "", "enable": true},
                                 
                              ]
                          }
                          ]
                          )";

};

#endif // TABLEDIALOG_H
