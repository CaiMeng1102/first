#include "TableDialog.h"
#include <QMessageBox>
#include <QPainter>
#include <QDebug>
#include <QTimer>

TableDialog::TableDialog(const QJsonObject& configObject, QWidget* parent)
	: QWidget(parent), configObject(configObject)
{
	this->setStyleSheet(" border: 1px solid gray;");
	columnConfig = configObject.value("arry").toArray();

	// 初始化新增的成员变量
	m_currentSelectedRow = -1;
	m_selectionBackgroundColor = QColor("#E1F5FE"); // 默认选中背景色

	// 初始化列类型映射
	for (int col = 0; col < columnConfig.size(); ++col) {
		QJsonObject colInfo = columnConfig[col].toObject();
		m_columnTypes[col] = colInfo["type"].toString();
		if (colInfo.contains("alignment")) {
			m_columnAlgens[col] = colInfo["alignment"].toString();
		}
		else {
			m_columnAlgens[col] = "";
		}
	}
	setupModel(columnConfig);
	setupUI(columnConfig);
	if (configObject.value("lineClick").toBool()) {
		// 注意：不设置SelectRows模式，使用NoSelection避免与自定义背景色冲突
		// tableView的选择模式在setupUI中设置为NoSelection
		connect(tableView, &QTableView::clicked, this, &TableDialog::onRowClicked);
		connect(tableView, &QTableView::clicked, this, &TableDialog::onCellClicked);
	}

}

void TableDialog::handleSelectAll(bool checked)
{
	for (int row = 0; row < model->rowCount(); ++row) {
		QStandardItem* item = model->item(row, 0);
		item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
	}
	emit cellupdate(-1, 0, "true");
}

void TableDialog::setupModel(const QJsonArray& config) {
	model = new QStandardItemModel(this);
	model->setColumnCount(config.count());

	// 设置表头
	QStringList headers;
	for (const QJsonValue& col : config) {
		auto obj = col.toObject();
		headers << obj.value("name").toString();
	}
	model->setHorizontalHeaderLabels(headers);
}

void TableDialog::setupUI(const QJsonArray& config) {


	tableView = new QTableView(this);
	tableView->verticalHeader()->hide();
	//tableView->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: #D9EDF8; color: black;}");
	tableView->horizontalHeader()->setStyleSheet(
		"QHeaderView::section:first { border-left: 1px solid #ccc; }"  // 首列特殊处理
		"QHeaderView::section {"
		"    background-color: #D9EDF8;"
		"   border: none;"  // 移除默认边框
		"   border-right: 1px solid #C1C1C1;"  // 添加细边框
		"   border-bottom: 1px solid #C1C1C1;"
		"}"
		"QHeaderView::section:pressed {"
		"    background-color: #D9EDF8;"
		"    color: black;"
		"}"
		"QHeaderView {"
		"    border: none;"          // 移除边框
		"    margin: 0px;"
		"    padding: 0px;"
		"}"
		"QTableView {"
		"    margin: 0px;"
		"    padding: 0px;"
		"   selection-background-color: #E1FBFB;"  // 选中背景色（蓝色）
		"   border: none; margin: 0px; padding: 0px;"
		"   gridline-color: #ccc;"
		"}"
		"QTableView::item {"
		//"    border-left: none;"  // 关键设置
		"    border: 1px solid #ccc;"
		//"    border-bottom: 1px solid #ccc;"
		"}"
		//"QTableView::item:focus { border: none; }"
	);

	tableView->setStyleSheet(
		"QTableView {"
		"background-color: white;"
		"    selection-background-color: transparent;"  // 禁用默认选中背景色，使用自定义方式
		"    selection-color: black;"
		"}"
	);

	// 设置选择模式为NoSelection，避免Qt默认选择行为干扰自定义背景色
	tableView->setSelectionMode(QAbstractItemView::NoSelection);
	tableView->setFocusPolicy(Qt::ClickFocus);
	tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
	tableView->setModel(model);
	tableView->horizontalHeader()->setStretchLastSection(true);
	tableView->horizontalHeader()->setOffset(0);
	tableView->horizontalHeader()->setMinimumSectionSize(0);
	tableView->horizontalHeader()->setDefaultSectionSize(tableView->horizontalHeader()->defaultSectionSize());

	//tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	//tableView->setSelectionBehavior(QAbstractItemView::SelectItems);  // 仅选中单元格，不影响表头

	// 根据配置设置列宽和委托
	for (int col = 0; col < config.count(); col++) {
		QJsonObject colInfo = config[col].toObject();
		QString type = colInfo["type"].toString();
		int width = 100;
		width = colInfo["width"].toInt();
		// 设置默认列宽
		if (type == "LineSelect") {
			// 设置自定义header
			CheckBoxHeaderViewTD* header = new CheckBoxHeaderViewTD(Qt::Horizontal, tableView);
			header->setAlignment(Qt::AlignmentFlag::AlignCenter);
			tableView->setHorizontalHeader(header);
			tableView->horizontalHeader()->setOffset(0);
			tableView->horizontalHeader()->setMinimumSectionSize(0);
			tableView->horizontalHeader()->setDefaultSectionSize(tableView->horizontalHeader()->defaultSectionSize());
			header->setModel(model);
			// 连接全选信号
			connect(header, &CheckBoxHeaderViewTD::selectAllRequested,
				this, &TableDialog::handleSelectAll);

			// 设置第一列为复选框列
			CheckBoxDelegate* checkDelegate = new CheckBoxDelegate(this);
			checkDelegate->setAlignment(Qt::AlignmentFlag::AlignCenter);
			tableView->setItemDelegateForColumn(col, checkDelegate);

			// 连接信号
			connect(checkDelegate, &CheckBoxDelegate::checkStateChanged, this, &TableDialog::onCheckBoxChanged);
			//tableView->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: #D9EDF8; color: black;}");
			tableView->horizontalHeader()->setStyleSheet(
				"QHeaderView::section:first { border-left: 1px solid #ccc; }"  // 首列特殊处理
				"QHeaderView::section {"
				"    background-color: #D9EDF8;"
				"   border: none;"  // 移除默认边框
				"   border-right: 1px solid #C1C1C1;"  // 添加细边框
				"   border-bottom: 1px solid #C1C1C1;"
				"}"
				"QHeaderView::section:pressed {"
				"    background-color: #D9EDF8;"
				"    color: black;"
				"}"
				"QHeaderView {"
				"    border: none;"          // 移除边框
				"    margin: 0px;"
				"    padding: 0px;"
				"}"
				"QTableView {"
				"    margin: 0px;"
				"    padding: 0px;"
				"   selection-background-color: #E1FBFB;"  // 选中背景色（蓝色）
				"   border: none; margin: 0px; padding: 0px;"
				"   gridline-color: #ccc;"
				"}"
				"QTableView::item {"
				//"    border-left: none;"  // 关键设置
				"    border: 1px solid #ccc;"
				//"    border-bottom: 1px solid #ccc;"
				"}"
				//"QTableView::item:focus { border: none; }"
			);


		}
		else if (type == "ReadOnly") {
			//            width = 80;
			//            tableView->setItemDelegateForColumn(col, new ReadOnlyDelegate(this));
		}
		// 设置委托
		else if (type == "Combobox") {
			// 获取选项列表
			QStringList options;
			if (colInfo.contains("options")) {
				QJsonArray jsonOptions = colInfo["options"].toArray();
				for (const QJsonValue& val : jsonOptions) {
					options << val.toString();
				}
			}
			if (colInfo["enableMulti"].toBool()) {
				MultiComboDelegate* delegate = new MultiComboDelegate(options, this);
				tableView->setItemDelegateForColumn(col, delegate);
				connect(delegate, &MultiComboDelegate::valueChanged,
					[this](const QModelIndex& index, const QString& newVal) {
						emit cellupdate(index.row(), index.column(), newVal);
					});
			}
			else {
				ComboDelegate* delegate = new ComboDelegate(options, this);
				tableView->setItemDelegateForColumn(col, delegate);
				connect(delegate, &ComboDelegate::valueChanged,
					[this](const QModelIndex& index, const QString& newVal) {
						emit cellupdate(index.row(), index.column(), newVal);
					});
			}
			//  2025.6.18 dongliyang
			//  添加联动下拉框类型的联动通知
			if (colInfo.contains("dependentColumn")) {
				int dependentCol = colInfo["dependentColumn"].toInt();
				ComboDelegate* delegate = qobject_cast<ComboDelegate*>(tableView->itemDelegateForColumn(col));
				if (delegate) {
					connect(delegate, &ComboDelegate::valueChanged,
						[this, col, dependentCol](const QModelIndex& index, const QString& newVal) {
							updateDependentColumn(index.row(), dependentCol, newVal);
						});
				}
			}
		}
		else if (type == "Delete" || type == "Click") {
			QIcon deleteIcon(":/icon/ico/删除.png");
			ButtonDelegate* deleteDelegate = new ButtonDelegate(deleteIcon, this);
			deleteDelegate->setIconSize(QSize(24, 24));
			deleteDelegate->setColors(
				QColor("#87CEEB"),
				QColor("#D3D3D3"),
				QColor("#1296db"),
				QColor("#808080")
			);

			if (type == "Delete") {
				connect(deleteDelegate, &ButtonDelegate::buttonClicked,
					this, &TableDialog::handleDelete);
			}
			else {
				connect(deleteDelegate, &ButtonDelegate::buttonClicked,
					this, &TableDialog::handleAction);
			}

			tableView->setItemDelegateForColumn(col, deleteDelegate);
			buttonDelegates.insert(col, deleteDelegate);
		}
		else if (type == "Switch") {
			SwitchButtonDelegate* switchDelegate = new SwitchButtonDelegate(this);

			tableView->setItemDelegateForColumn(col, switchDelegate);

			connect(switchDelegate, &SwitchButtonDelegate::stateChanged,
				[this](const QModelIndex& index, bool state) {
					//                qDebug() << "Row" << row << "switch state changed to:" << state;
					emit cellupdate(index.row(), index.column(), state == true ? "true" : "false");
				});
		}
		else if (type == "Attribute") {
			AttributeDelegate* attDelegate = new AttributeDelegate(this);

			tableView->setItemDelegateForColumn(col, attDelegate);

			connect(attDelegate, &AttributeDelegate::valueChanged,
				[this](const QModelIndex& index, const QString& newVal) {
					emit cellupdate(index.row(), index.column(), newVal);
				});
		}
		else if (type == "Applicability") {
			AttributeDelegate* appDelegate = new AttributeDelegate(this);

			tableView->setItemDelegateForColumn(col, appDelegate);

			connect(appDelegate, &AttributeDelegate::valueChanged,
				[this](const QModelIndex& index, const QString& newVal) {
					emit cellupdate(index.row(), index.column(), newVal);
				});
		}
		else if (type == "ColorDialog") {
			ColorDelegate* colorDelegate = new ColorDelegate(this);

			// 设置颜色方块大小（类似复选框）
			colorDelegate->setColorBoxSize(16);

			tableView->setItemDelegateForColumn(col, colorDelegate);

			// 确保列宽不小于最小宽度
			int minWidth = colorDelegate->getMinimumColumnWidth();
			if (width < minWidth) {
				width = minWidth;
			}

			connect(colorDelegate, &ColorDelegate::colorClicked,
				this, &TableDialog::onColorClicked);
		}
		tableView->setColumnWidth(col, width);

	}
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);  // 左、上、右、下边距
	layout->setSpacing(0);  // 控件间距
	layout->setMargin(0);
	layout->addWidget(tableView);
	setLayout(layout);
}


void TableDialog::updateDependentColumn(int row, int col, const QString& parentValue) {
	if (row < 0 || col < 0 || col >= model->columnCount()) return;

	// 根据父列值获取子列选项
	QStringList options;
	if (m_dependencyMap.contains(parentValue)) {
		QJsonArray jsonOptions = m_dependencyMap[parentValue];
		for (const QJsonValue& val : jsonOptions) {
			options << val.toString();
		}
	}

	// 更新子列的ComboBox数据
	setComboBoxData(row, col, options);

	// 自动触发子列的数据更新事件
	QStandardItem* item = model->item(row, col);
	if (item) {
		// 若无选项，清空单元格值
		if (options.isEmpty()) {
			item->setText(""); // 置空当前值
			emit cellupdate(row, col, ""); // 通知外部数据变更
		}
		else {
			emit cellupdate(row, col, item->text());
		}
	}


}

void TableDialog::initData(const QJsonArray& jsonData) {

	model->setRowCount(jsonData.size());

	for (int row = 0; row < jsonData.size(); row++) {
		QJsonObject rowobj = jsonData[row].toObject();
		bool rowenable = true;
		if (rowobj.contains("enable")) {
			rowenable = rowobj.value("enable").toBool();
		}
		m_RowEnables.insert(row, rowenable);
		QJsonArray rowData = rowobj["rowdata"].toArray();
		// 数据验证：列数匹配检查
		if (rowData.size() != columnConfig.size()) {
			//            qWarning() << "Row" << row << "data count mismatch! Expected:"
			//                      << columnConfig.size() << "Actual:" << rowData.size();
			return;
		}

		for (int col = 0; col < rowData.size(); col++) {

			QJsonObject cellData = rowData[col].toObject();
			QJsonObject colInfo = columnConfig[col].toObject();

			QString type = colInfo["type"].toString();
			QString alignment = "";
			if (colInfo.contains("alignment")) {
				alignment = colInfo["alignment"].toString();
			}
			else {
				alignment = "center";
			}
			QString value = cellData["value"].toString(); // 获取对应列数据
			QStandardItem* item = new QStandardItem();

			// 根据列类型处理数据
			if (type == "LineSelect") {
				item->setCheckable(true);
				//初次加载数据默认都是选中
				item->setCheckState(Qt::Checked);
				//                item->setCheckState(value.toLower() == "true" ? Qt::Checked : Qt::Unchecked);
								//item->setTextAlignment(Qt::AlignCenter);
				item->setEditable(false);
			}
			else if (type == "WriteAble") {
				item->setText(value);
				if (alignment == "left") {
					item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
				}
				else if (alignment == "right") {
					item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
				}
				else {
					item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
				}
			}
			else if (type == "Combobox") {
				// 验证下拉选项合法性
				QStringList options;
				if (colInfo.contains("options")) {
					QJsonArray jsonOptions = colInfo["options"].toArray();
					for (const QJsonValue& val : jsonOptions) {
						options << val.toString();
					}
				}

				if (options.contains(value)) {
					item->setText(value);
				}
				else {
					item->setText(options.value(-1));
					//                    qWarning() << "Invalid value for combobox column" << col
					//                              << ":" << value;
				}
				//item->setTextAlignment(Qt::AlignCenter);
				if (alignment == "left") {
					item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
				}
				else if (alignment == "right") {
					item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
				}
				else {
					item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
				}

			}
			else if (type == "Switch") {
				bool value1 = cellData["value"].toBool();
				if (value1) {
					item->setData(true, Qt::CheckStateRole); // 打开状态
				}
				else {
					item->setData(false, Qt::CheckStateRole); // 打开状态
				}
				item->setEditable(false);

			}
			else if (type == "ReadOnly") {
				item->setText(value);
				item->setEditable(false);
				//item->setTextAlignment(Qt::AlignCenter);
				if (alignment == "left") {
					item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
				}
				else if (alignment == "right") {
					item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
				}
				else {
					item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
				}
				// 添加字体颜色显示
				// 检查是否有color字段
				if (cellData.contains("color")) {
					QString colorStr = cellData["color"].toString();
					// 尝试解析颜色值
					QColor color(colorStr);
					if (color.isValid()) {
						item->setForeground(color);
					}
				}
				else {
					// 默认黑色
					item->setForeground(Qt::black);
				}
			}
			// Delete/Click列不需要数据
			else if (type == "Delete" || type == "Click") {
				bool enable = cellData["enable"].toBool();
				item->setEditable(false);
				item->setData(enable, Qt::UserRole);
			}
			else if (type == "Attribute") {
				int attrid = cellData["id"].toInt();
				QString attrtype = cellData["type"].toString();
				QString value = cellData["value"].toString();
				if (attrtype == "整数" || attrtype == "浮点数" || attrtype == "文本") {
					item->setData(AttributeDelegate::TextEditor, AttributeDelegate::DelegateTypeRole);
				}
				else if (attrtype == "单选") {
					item->setData(AttributeDelegate::ComboBoxEditor, AttributeDelegate::DelegateTypeRole);
				}
				else if (attrtype == "多选") {
					item->setData(AttributeDelegate::MultiComboEditor, AttributeDelegate::DelegateTypeRole);
				}
				else {
					item->setData(AttributeDelegate::TextEditor, AttributeDelegate::DelegateTypeRole);
				}
				QStringList list;
				getAttributeValues(list, 0, attrid);
				// 设置下拉框选项初始值
				item->setData(list, AttributeDelegate::ComboItemsRole);
				// 设置当前选中值
				item->setData(value, Qt::EditRole);
				//item->setTextAlignment(Qt::AlignCenter);
				if (alignment == "left") {
					item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
				}
				else if (alignment == "right") {
					item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
				}
				else {
					item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
				}

			}
			else if (type == "Applicability") {
				int conditionid = cellData["id"].toInt();
				QString conditiontype = cellData["type"].toString();
				QString value = cellData["value"].toString();
				if (conditiontype == "整数" || conditiontype == "浮点数" || conditiontype == "文本") {
					item->setData(AttributeDelegate::TextEditor, AttributeDelegate::DelegateTypeRole);
				}
				else if (conditiontype == "单选") {
					item->setData(AttributeDelegate::ComboBoxEditor, AttributeDelegate::DelegateTypeRole);
				}
				else if (conditiontype == "多选") {
					item->setData(AttributeDelegate::MultiComboEditor, AttributeDelegate::DelegateTypeRole);
				}
				else {
					item->setData(AttributeDelegate::TextEditor, AttributeDelegate::DelegateTypeRole);
				}
				QStringList list;
				getConditionValues(list, conditionid);
				// 设置下拉框选项初始值
				item->setData(list, AttributeDelegate::ComboItemsRole);
				// 设置当前选中值
				item->setData(value, Qt::EditRole);
				//item->setTextAlignment(Qt::AlignCenter);
				if (alignment == "left") {
					item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
				}
				else if (alignment == "right") {
					item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
				}
				else {
					item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
				}

			}
			else if (type == "ColorDialog") {
				item->setText("");
				item->setEditable(false);


				if (cellData.contains("color")) {
					QString colorStr = cellData["color"].toString();
					QColor color(colorStr);
					if (color.isValid()) {
						item->setData(color, Qt::UserRole + 1);
					}
				}
				else {
					// 默认白色
					item->setData(QColor(Qt::white), Qt::UserRole + 1);
				}

				if (alignment == "left") {
					item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
				}
				else if (alignment == "right") {
					item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
				}
				else {
					item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
				}
			}
			model->setItem(row, col, item);
		}
	}

	initDataEnable(jsonData);
	connect(model, &QStandardItemModel::dataChanged,
		this, &TableDialog::onDataChanged);
}

void TableDialog::updateData(const QJsonArray& jsonData)
{

	model->setRowCount(0);  // 清空所有行，但列结构不变
	//model->clear();
	disconnect(model, &QStandardItemModel::dataChanged,
		this, &TableDialog::onDataChanged);
	initData(jsonData);
}

void TableDialog::initDataEnable(const QJsonArray& jsonData)
{
	model->setRowCount(jsonData.size());

	for (int row = 0; row < jsonData.size(); row++) {
		QJsonObject rowobj = jsonData[row].toObject();
		bool rowenable = true;
		if (rowobj.contains("enable")) {
			rowenable = rowobj.value("enable").toBool();
		}
		m_RowEnables.insert(row, rowenable);
		QJsonArray rowData = rowobj["rowdata"].toArray();
		// 数据验证：列数匹配检查
		if (rowData.size() != columnConfig.size()) {
			//            qWarning() << "Row" << row << "data count mismatch! Expected:"
			//                      << columnConfig.size() << "Actual:" << rowData.size();
			return;
		}
		for (int col = 0; col < rowData.size(); col++) {
			QJsonObject cellData = rowData[col].toObject();
			QJsonObject colInfo = columnConfig[col].toObject();

			QString type = colInfo["type"].toString();

			if (type == "LineSelect") {
				bool value = cellData["value"].toBool(); // 获取对应列数据
				QStandardItem* item = model->item(row, col);
				item->setCheckState(value == true ? Qt::Checked : Qt::Unchecked);
				onCheckBoxChanged(item->index(), value);

			}

		}
	}
}

void TableDialog::sltaddRow()
{
	int row = model->rowCount();
	for (int col = 0; col < m_columnTypes.size(); col++) {
		QString type = m_columnTypes.value(col);
		QStandardItem* item = new QStandardItem();

		QString alignment = m_columnAlgens.value(col);

		// 根据列类型处理数据
		if (type == "LineSelect") {
			item->setCheckable(true);
			//初次加载数据默认都是选中
			item->setCheckState(Qt::Unchecked);
			//                item->setCheckState(value.toLower() == "true" ? Qt::Checked : Qt::Unchecked);
			//            item->setTextAlignment(Qt::AlignCenter);
			item->setEditable(false);
		}
		else if (type == "WriteAble") {
			item->setText("");
			//item->setTextAlignment(Qt::AlignCenter);
			if (alignment == "left") {
				item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			}
			else if (alignment == "right") {
				item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			}
			else {
				item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
			}

		}
		else if (type == "Combobox") {
			item->setText("");
			//item->setTextAlignment(Qt::AlignCenter);
			if (alignment == "left") {
				item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			}
			else if (alignment == "right") {
				item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			}
			else {
				item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
			}
		}
		else if (type == "Switch") {
			item->setData(false, Qt::CheckStateRole); // 打开状态
			//item->setEditable(false);

		}
		else if (type == "ReadOnly") {
			item->setText("");
			//item->setTextAlignment(Qt::AlignCenter);
			if (alignment == "left") {
				item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			}
			else if (alignment == "right") {
				item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			}
			else {
				item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
			}
		}
		// Delete/Click列不需要数据
		else if (type == "Delete" || type == "Click") {
			item->setEditable(false);
			item->setData(true, Qt::UserRole);
		}
		else if (type == "ColorDialog") {
			item->setText("");
			item->setEditable(false);
			// 设置默认白色（使用自定义角色）
			item->setData(QColor(Qt::white), Qt::UserRole + 1);
			if (alignment == "left") {
				item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			}
			else if (alignment == "right") {
				item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			}
			else {
				item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
			}
		}
		model->setItem(row, col, item);
	}

}

void TableDialog::handleAction(const QModelIndex& index) {
	int row = index.row();
	//    bool checked = model->item(row, 0)->checkState() == Qt::Checked;
	//    QString editText = model->item(row, 1)->text();
	//    QString comboText = model->item(row, 2)->text();

	//    QMessageBox::information(this, "行数据",
	//        QString("第%1行:\n选中状态: %2\n编辑内容: %3\n下拉选项: %4")
	//            .arg(row + 1)
	//            .arg(checked ? "已选中" : "未选中")
	//            .arg(editText)
	//            .arg(comboText));
}

void TableDialog::handleDelete(const QModelIndex& index) {
	int row = index.row();
	model->removeRow(row);
	emit rowDelete(row);
}

// 新增槽函数实现
void TableDialog::onRowClicked(const QModelIndex& index) {
	if (!index.isValid()) return;

	int clickedRow = index.row();

	// 使用QTimer延迟执行，确保在Qt默认处理完成后再设置自定义背景色
	QTimer::singleShot(0, [this, clickedRow]() {
		setCustomRowSelection(clickedRow);
		});

	// 2024.6.19 dongliyang 
	/////start
	// 当前的行数据处理设计只能存储单行委托数据，为实现联动效果，切换行时获取父类的实际参数
	//QStandardItem* parentItem = model->item(clickedRow, 1);
	//if (parentItem) {
	//	QString parentValue = parentItem->text();
	//	updateDependentColumn(clickedRow, 2, parentValue);
	//}
	////////////////////end
	emit rowClicked(clickedRow);

	//    // 获取完整行数据
	//    QStringList rowData;
	//    for (int col = 0; col < model->columnCount(); ++col) {
	//        QStandardItem* item = model->item(clickedRow, col);
	//        if (item) {
	//            if (item->isCheckable()) {
	//                rowData << (item->checkState() == Qt::Checked ? "选中" : "未选中");
	//            } else {
	//                rowData << item->text();
	//            }
	//        }
	//    }

	//    // 示例：显示提示信息
	//    QMessageBox::information(this, "行点击事件",
	//        QString("点击第 %1 行\n数据：%2")
	//            .arg(clickedRow + 1)
	//            .arg(rowData.join(" | ")));
}

void TableDialog::onCellClicked(const QModelIndex& index) {
	if (!index.isValid()) {
		return;
	}

	int row = index.row();
	int column = index.column();

	// 发出信号，传递点击的行和列
	emit cellClicked(row, column);
}

// 数据变化处理函数
void TableDialog::onDataChanged(const QModelIndex& topLeft,
	const QModelIndex& bottomRight) {
	//这里只是对可编辑数据的改变进行
	for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
		for (int col = topLeft.column(); col <= bottomRight.column(); ++col) {
			if (m_columnTypes.value(col) == "WriteAble" || m_columnTypes.value(col) == "ReadOnly") {
				QStandardItem* item = model->item(row, col);
				QString newValue = item->text();
				emit cellupdate(row, col, newValue);
			}
		}
	}
}

void TableDialog::onCheckBoxChanged(const QModelIndex& index, bool checked)
{
	emit cellupdate(index.row(), index.column(), checked ? "true" : "false");

	//判断当前行 为不允许编辑的行
	if (!m_RowEnables.value(index.row())) {
		//取消选中的情况
		if (!checked) {
			//更改当前行的  每一列为不可编辑
//           row model->takeRow(index.row())
			for (int i = 0; i < m_columnTypes.size(); i++) {
				QStandardItem* item = model->item(index.row(), i);
				QString type = m_columnTypes.value(i);
				// 根据列类型处理数据
				if (type == "LineSelect") {

				}
				else if (type == "WriteAble") {
					Qt::ItemFlags flags = item->flags();
					flags &= ~Qt::ItemIsEditable;  // 按位取反后与运算，移除可编辑标志
					item->setFlags(flags);
				}
				else if (type == "Combobox") {
					Qt::ItemFlags flags = item->flags();
					flags &= ~Qt::ItemIsEditable;  // 按位取反后与运算，移除可编辑标志
					item->setFlags(flags);
				}
				else if (type == "Switch") {
					Qt::ItemFlags flags = item->flags();
					flags &= ~Qt::ItemIsEditable;  // 按位取反后与运算，移除可编辑标志
					item->setFlags(flags);
				}
				else if (type == "ReadOnly") {
					Qt::ItemFlags flags = item->flags();
					flags &= ~Qt::ItemIsEditable;  // 按位取反后与运算，移除可编辑标志
					item->setFlags(flags);
				}
				// Delete/Click列不需要数据
				else if (type == "Delete" || type == "Click") {
					//                    item->setData(false,Qt::UserRole);
				}
			}
		}
		else {
			for (int i = 0; i < m_columnTypes.size(); i++) {
				QStandardItem* item = model->item(index.row(), i);
				QString type = m_columnTypes.value(i);
				// 根据列类型处理数据
				if (type == "LineSelect") {

				}
				else if (type == "WriteAble") {
					item->setFlags(item->flags() | Qt::ItemIsEditable);  // 添加可编辑标志
				}
				else if (type == "Combobox") {
					item->setFlags(item->flags() | Qt::ItemIsEditable);  // 添加可编辑标志
				}
				else if (type == "Switch") {
					item->setFlags(item->flags() | Qt::ItemIsEditable);  // 添加可编辑标志
					item->setEditable(false);
				}
				else if (type == "ReadOnly") {
				}
				// Delete/Click列不需要数据
				else if (type == "Delete" || type == "Click") {

				}
			}
		}
	}

}

void TableDialog::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	QStyleOption styleOpt;
	styleOpt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &styleOpt, &painter, this);

}

// 在 tablewindow.cpp 中添加实现
QString TableDialog::getCellData(int row, int col) {
	// 参数有效性检查
	if (row < 0 || row >= model->rowCount() ||
		col < 0 || col >= model->columnCount()) {
		//        qWarning() << "Invalid cell position:" << row << col;
		return "";
	}

	QStandardItem* item = model->item(row, col);
	QJsonObject colInfo = columnConfig[col].toObject();
	QString type = colInfo["type"].toString();

	// 根据列类型处理数据
	if (type == "LineSelect") {
		return item->checkState() == Qt::Checked ? "true" : "false";
	}
	return item->text();
}

QStringList TableDialog::getRowData(int row) {
	QStringList rowData;
	if (row < 0 || row >= model->rowCount()) {
		//        qWarning() << "Invalid row number:" << row;
		return rowData;
	}

	for (int col = 0; col < model->columnCount(); ++col) {
		rowData << getCellData(row, col);
	}
	return rowData;
}

QJsonArray TableDialog::currentDataToJson() {
	QJsonArray jsonData;
	for (int row = 0; row < model->rowCount(); ++row) {
		QJsonArray jsonRow;
		for (int col = 0; col < model->columnCount(); ++col) {
			QJsonObject cell;
			QJsonObject colConfig = columnConfig[col].toObject();
			QString type = colConfig["type"].toString();

			// 特殊列处理
			if (type == "LineSelect") {
				cell["value"] = model->item(row, col)->checkState() == Qt::Checked;
			}
			else if (type == "Delete" || type == "Click") {
				cell["enable"] = model->item(row, col)->data(Qt::UserRole).toBool();
			}
			else if (type == "ColorDialog") {
				QColor color = model->item(row, col)->data(Qt::UserRole + 1).value<QColor>();
				cell["color"] = color.name(); // 将颜色存储为十六进制字符串
				cell["value"] = ""; // ColorDialog类型没有文本值
			}
			else {
				cell["value"] = model->item(row, col)->text();

				// 下拉列存储选项
				if (type == "Combobox" && colConfig.contains("options")) {
					cell["options"] = colConfig["options"];
				}
			}
			jsonRow.append(cell);
		}
		jsonData.append(jsonRow);
	}
	return jsonData;
}

QList<int> TableDialog::currentSelectRow() {
	QList<int> selectedRows;
	const int checkColumn = 0; // 假设复选框在第一列

	if (!model) {
		qDebug() << "TableDialog::currentSelectRow() - 模型为空";
		return selectedRows;
	}

	for (int row = 0; row < model->rowCount(); ++row) {
		QStandardItem* item = model->item(row, checkColumn);
		if (item && item->checkState() == Qt::Checked) {
			selectedRows << row;
		}
	}
	return selectedRows;
}

void TableDialog::clearCheck() {

	if (m_columnTypes.value(0) == "LineSelect") {
		for (int row = 0; row < model->rowCount(); ++row) {
			QStandardItem* item = model->item(row, 0);
			item->setCheckState(Qt::Unchecked);
		}
	}
}
void TableDialog::headVisible() {
	tableView->horizontalHeader()->setVisible(false);
}

QTableView* TableDialog::getTableView()
{
	return tableView;
}

QJsonArray TableDialog::GetTableData()
{

	QJsonArray resultArray;

	if (!model) {
		qDebug() << "TableDialog::GetTableData() - 模型为空";
		return resultArray;
	}

	for (int row = 0; row <= model->rowCount(); row++) {
		QJsonObject rowObject;

		for (int col = 0; col < model->columnCount(); col++) {
			QJsonObject colInfo = columnConfig[col].toObject();
			QString columnName = colInfo["name"].toString();
			QString columnType = colInfo["type"].toString();

			/*if (columnType == "LineSelect" || columnType == "Click") {
				continue;
			}*/

			QStandardItem* item = model->item(row, col);
			if (!item) {
				continue;
			}

			QString cellValue;

			if (columnType == "Switch") {
				bool switchState = item->data(Qt::CheckStateRole).toBool();
				cellValue = switchState ? "开" : "关";
			}
			else if (columnType == "Combobox") {
				cellValue = item->text();
			}
			else if (columnType == "WriteAble" || columnType == "ReadOnly") {
				cellValue = item->text();
			}
			else if (columnType == "Delete") {
				cellValue = "delete";
			}
			else {
				cellValue = item->text();
			}

			if (!columnName.isEmpty()) {
				rowObject[columnName] = cellValue;
			}
		}

		if (!rowObject.isEmpty()) {
			resultArray.append(rowObject);
		}
	}
	qDebug() << "得到的json" << resultArray;
	return resultArray;
}

void TableDialog::moveSelectedRowUp()
{
	//// 获取当前选中的行
	//QModelIndexList selectedIndexes = tableView->selectionModel()->selectedIndexes();
	//qDebug() << "dang qian" << selectedIndexes;
	//if (selectedIndexes.isEmpty()) return;

	//// 假设我们只处理第一个选中的行
	//int selectedRow = selectedIndexes.first().row();

	//// 确保选中的行不是第一行
	//if (selectedRow <= 0) return;

	//// 获取模型
	//QStandardItemModel* model = static_cast<QStandardItemModel*>(tableView->model());

	//// 创建一个临时列表来保存选中行的数据
	//QList<QStandardItem*> selectedRowItems;
	//for (int col = 0; col < model->columnCount(); ++col) {
	//	selectedRowItems.append(model->takeRow(selectedRow).takeFirst());
	//}

	//// 插入到上一行
	//model->insertRow(selectedRow - 1, selectedRowItems);

	//// 删除原来的行
	//model->removeRow(selectedRow + 1); // 注意这里加1是因为我们刚刚插入了一行

	//// 更新选中行
	//tableView->setCurrentIndex(model->index(selectedRow - 1, selectedIndexes.first().column()));

// 2025.6.19 dongliyang
	// 原代码问题为移动时会修改选中行以及修改选中行之后的第一列数据

	QModelIndexList selectedIndexes = tableView->selectionModel()->selectedRows();
	if (selectedIndexes.isEmpty()) return;

	// 使用持久化索引避免动态变化
	QVector<QPersistentModelIndex> persistentIndexes;
	for (const QModelIndex& index : selectedIndexes) {
		persistentIndexes.append(QPersistentModelIndex(index));
	}

	tableView->setUpdatesEnabled(false);
	model->layoutAboutToBeChanged();

	QList<QList<QStandardItem*>> itemsList;
	QList<int> newRows;

	// 按行号升序处理（从顶部开始操作）
	std::sort(persistentIndexes.begin(), persistentIndexes.end(),
		[](const QPersistentModelIndex& a, const QPersistentModelIndex& b) {
			return a.row() < b.row();
		});

	for (int i = 0; i < persistentIndexes.size(); ++i) {
		int row = persistentIndexes[i].row();
		if (row <= 0) continue;  // 顶部行无法上移

		// 取出当前行并保存
		itemsList.append(model->takeRow(row));
		newRows.append(row - 1);  // 目标位置在上方一行
	}

	// 批量插入到新位置
	for (int i = 0; i < itemsList.size(); ++i) {
		model->insertRow(newRows[i], itemsList[i]);
	}

	model->layoutChanged();
	tableView->setUpdatesEnabled(true);

	// 重新选中移动后的行
	QItemSelection selection;
	for (int row : newRows) {
		selection.select(model->index(row, 0),
			model->index(row, model->columnCount() - 1));
	}
	tableView->selectionModel()->select(selection, QItemSelectionModel::Select);
}


void TableDialog::moveSelectedRowDown() {
	// dongliyang 2025.6.20
	QModelIndexList selectedIndexes = tableView->selectionModel()->selectedRows();
	if (selectedIndexes.isEmpty()) return;

	// 使用持久化索引避免动态变化
	QVector<QPersistentModelIndex> persistentIndexes;
	for (const QModelIndex& index : selectedIndexes) {
		persistentIndexes.append(QPersistentModelIndex(index));
	}

	tableView->setUpdatesEnabled(false);
	model->layoutAboutToBeChanged();

	// 按新位置重新插入行
	QList<QList<QStandardItem*>> itemsList;
	QList<int> newRows;

	// 反向遍历（从最后一行开始移动）
	for (int i = persistentIndexes.size() - 1; i >= 0; --i) {
		int row = persistentIndexes[i].row();
		if (row >= model->rowCount() - 1) continue;

		itemsList.prepend(model->takeRow(row));   // 取出并缓存
		newRows.append(row + 1);                  // 记录新位置
	}

	// 批量插入到新位置
	for (int i = 0; i < itemsList.size(); ++i) {
		model->insertRow(newRows[i], itemsList[i]);
	}

	model->layoutChanged();
	tableView->setUpdatesEnabled(true);

	// 重新选中
	QItemSelection selection;
	for (int row : newRows) {
		selection.select(model->index(row, 0), model->index(row, model->columnCount() - 1));
	}
	tableView->selectionModel()->select(selection, QItemSelectionModel::Select);
}

void TableDialog::setComboBoxData(int row, int col, const QStringList& options)
{
	// 确保行和列在有效范围内
	if (row < 0 || row >= model->rowCount() || col < 0 || col >= model->columnCount()) {
		qDebug() << "Invalid row or column index";
		return;
	}

	// 获取列信息
	QJsonObject colInfo = columnConfig[col].toObject();
	QString type = colInfo["type"].toString();

	// 检查是否为 Combobox 类型
	if (type != "Combobox") {
		qDebug() << "Cell is not of type Combobox";
		return;
	}

	// 获取单元格的 item
	QStandardItem* item = model->item(row, col);
	if (!item) {
		qDebug() << "Item is null";
		return;
	}

	// 创建或获取 ComboBoxDelegate
	ComboDelegate* delegate = qobject_cast<ComboDelegate*>(tableView->itemDelegateForColumn(col));
	if (!delegate) {
		// 如果没有委托，创建一个新的
		delegate = new ComboDelegate(options, this);
		tableView->setItemDelegateForColumn(col, delegate);
	}
	else {
		// 如果委托已经存在，更新其选项
		delegate->setOptions(options);
	}

	// 确保单元格显示正确的数据
	if (options.contains(item->text())) {
		item->setText(item->text());
	}
	else {
		// 如果当前数据不在选项中，设置为第一个选项
		if (!options.isEmpty()) {
			item->setText(options.first());
		}
	}
}

void TableDialog::disableHorizontalSlider()
{
	tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void TableDialog::selectFirstRow()
{
	//确保模型中有行数据
	if (model->rowCount() > 0) {
		// 设置选中第一行
		QModelIndex firstIndex = model->index(0, 0); // 第一行第一列的索引

		// 选中整行（如果配置了行选择模式）
		tableView->selectionModel()->select(
			firstIndex,
			QItemSelectionModel::Select | QItemSelectionModel::Rows
		);
		// 滚动到第一行确保可见
		//tableView->scrollTo(firstIndex);

		// 手动触发点击信号（模拟用户点击）
		//emit tableView->clicked(firstIndex);
		onRowClicked(firstIndex);
		//tableView->setFocus();
	}
}

void TableDialog::getAttributeValues(QStringList& list, int type, int attrId)
{

	// 构造查询SQL
	QString sql = "SELECT * FROM attribute_value WHERE attribute_id = %1";
	sql = sql.arg(attrId);
	QVariant result = gAPP->GetIO()->GetDataBySql(sql);
	if (!result.isValid()) {
		qDebug() << "Failed to execute fuzzy query";
		return;
	}

	QVariantList resultList;
	if (result.type() == QVariant::Map) {
		resultList.append(result);
	}
	else if (result.type() == QVariant::List) {
		resultList = result.toList();
	}

	for (const QVariant& row : resultList) {
		QVariantMap map = row.toMap();
		list.append(map["value"].toString());
	}
}

void  TableDialog::setDependencyMap(const QJsonObject& jsonOptions) {
	//2025.6.18 dongliyang
	auto it = jsonOptions.constBegin();
	auto end = jsonOptions.constEnd();
	while (it != end) {
		QString key = it.key();
		QJsonValue value = it.value();
		//当前未考虑不同列有相同的键，有需求可添加行列的拼接标识
		m_dependencyMap.insert(key, value.toArray());
		++it;
	}

}
void TableDialog::getConditionValues(QStringList& list, int conditionId)
{

	// 构造查询SQL
	QString sql = "SELECT * FROM applicability_condition_value WHERE codition_id = %1";
	sql = sql.arg(conditionId);
	QVariant result = gAPP->GetIO()->GetDataBySql(sql);
	if (!result.isValid()) {
		qDebug() << "Failed to execute fuzzy query";
		return;
	}

	QVariantList resultList;
	if (result.type() == QVariant::Map) {
		resultList.append(result);
	}
	else if (result.type() == QVariant::List) {
		resultList = result.toList();
	}

	for (const QVariant& row : resultList) {
		QVariantMap map = row.toMap();
		list.append(map["condition_value"].toString());
	}
}

void TableDialog::setCustomRowSelection(int row)
{
	// 首先清除之前的选中状态
	clearCustomRowSelection();

	if (row < 0 || row >= model->rowCount()) {
		return;
	}

	m_currentSelectedRow = row;

	qDebug() << "Setting custom row selection for row:" << row;

	// 遍历当前行的所有列
	for (int col = 0; col < model->columnCount(); ++col) {
		QStandardItem* item = model->item(row, col);
		if (!item) continue;

		// 获取当前单元格的背景色
		QColor currentBgColor = getCellBackgroundColor(row, col);

		// 存储原始背景色
		QPair<int, int> cellKey(row, col);
		m_originalBackgroundColors[cellKey] = currentBgColor;

		qDebug() << "Cell (" << row << "," << col << ") current color:" << currentBgColor;

		// 检查背景色是否为白色 (#ffffff 或 white)
		bool isWhiteBackground = (currentBgColor == QColor(Qt::white) ||
			currentBgColor == QColor("#ffffff") ||
			currentBgColor == QColor(255, 255, 255) ||
			!currentBgColor.isValid()); // 无效颜色视为默认白色

		qDebug() << "Is white background:" << isWhiteBackground;

		// 如果是白色背景，则设置为选中背景色
		if (isWhiteBackground) {
			setCellBackgroundColor(row, col, m_selectionBackgroundColor);
			qDebug() << "Set cell (" << row << "," << col << ") to selection color:" << m_selectionBackgroundColor;
		}
		// 如果不是白色背景，保持原色不变（不做任何操作）
	}

	// 强制刷新视图
	tableView->viewport()->update();
}

void TableDialog::clearCustomRowSelection()
{
	if (m_currentSelectedRow < 0) {
		return;
	}

	// 恢复所有单元格的原始背景色
	for (auto it = m_originalBackgroundColors.begin(); it != m_originalBackgroundColors.end(); ++it) {
		QPair<int, int> cellKey = it.key();
		QColor originalColor = it.value();

		// 只恢复当前选中行的颜色
		if (cellKey.first == m_currentSelectedRow) {
			setCellBackgroundColor(cellKey.first, cellKey.second, originalColor);
		}
	}

	// 清除存储的背景色信息
	m_originalBackgroundColors.clear();
	m_currentSelectedRow = -1;
}

QColor TableDialog::getCellBackgroundColor(int row, int col)
{
	if (row < 0 || row >= model->rowCount() || col < 0 || col >= model->columnCount()) {
		return QColor(); // 返回无效颜色
	}

	QStandardItem* item = model->item(row, col);
	if (!item) {
		return QColor(Qt::white); // 默认白色
	}

	// 获取单元格的背景色
	QVariant bgColorVariant = item->data(Qt::BackgroundRole);
	if (bgColorVariant.isValid()) {
		QColor color = bgColorVariant.value<QColor>();
		qDebug() << "Found background color for cell (" << row << "," << col << "):" << color;
		return color;
	}

	// 如果没有设置背景色，检查是否有背景刷子
	QVariant bgBrushVariant = item->data(Qt::BackgroundColorRole);
	if (bgBrushVariant.isValid()) {
		QColor color = bgBrushVariant.value<QColor>();
		qDebug() << "Found background brush color for cell (" << row << "," << col << "):" << color;
		return color;
	}

	qDebug() << "No background color found for cell (" << row << "," << col << "), using white";
	return QColor(Qt::white); // 默认白色
}

void TableDialog::setCellBackgroundColor(int row, int col, const QColor& color)
{
	if (row < 0 || row >= model->rowCount() || col < 0 || col >= model->columnCount()) {
		return;
	}

	QStandardItem* item = model->item(row, col);
	if (!item) {
		return;
	}

	// 设置单元格背景色
	if (color.isValid()) {
		item->setData(color, Qt::BackgroundRole);
		qDebug() << "Set background color for cell (" << row << "," << col << ") to:" << color;
	}
	else {
		// 如果颜色无效，移除背景色设置（恢复默认）
		item->setData(QVariant(), Qt::BackgroundRole);
		qDebug() << "Cleared background color for cell (" << row << "," << col << ")";
	}
}

void TableDialog::setSelectionBackgroundColor(const QColor& color)
{
	m_selectionBackgroundColor = color;
}

void TableDialog::setTestBackgroundColors()
{
	// 为测试目的设置一些单元格为非白色背景
	if (model->rowCount() > 0) {
		// 设置第一行第二列为红色背景
		if (model->columnCount() > 1) {
			setCellBackgroundColor(0, 1, QColor("#FF0000"));
		}
		// 设置第一行第三列为绿色背景
		if (model->columnCount() > 2) {
			setCellBackgroundColor(0, 2, QColor("#00FF00"));
		}
	}

	if (model->rowCount() > 1) {
		// 设置第二行第一列为黄色背景
		if (model->columnCount() > 0) {
			setCellBackgroundColor(1, 0, QColor("#FFFF00"));
		}
	}

	qDebug() << "Test background colors set";
}

void TableDialog::onColorClicked(const QModelIndex& index)
{
	if (!index.isValid()) return;

	int row = index.row();
	int col = index.column();

	// 只发出颜色点击信号，不弹出颜色对话框
	// 颜色选择对话框由外部类（如DLG_Element_Library_Management）处理
	emit colorCellClicked(row, col);
}

void TableDialog::setCellColor(int row, int col, const QColor& color)
{
	if (row < 0 || row >= model->rowCount() || col < 0 || col >= model->columnCount()) {
		return;
	}

	QStandardItem* item = model->item(row, col);
	if (!item) return;

	// 检查列类型是否为ColorDialog
	QString type = m_columnTypes.value(col);
	if (type == "ColorDialog") {
		// 使用自定义角色存储颜色，ColorDelegate会读取这个颜色并绘制颜色方块
		// 这里不使用BackgroundRole，避免影响整个单元格的背景色
		item->setData(color, Qt::UserRole + 1);

		// 强制刷新视图以显示新颜色
		tableView->viewport()->update();
	}
}

QColor TableDialog::getCellColor(int row, int col) const
{
	if (row < 0 || row >= model->rowCount() || col < 0 || col >= model->columnCount()) {
		return QColor(); // 返回无效颜色
	}

	QStandardItem* item = model->item(row, col);
	if (!item) {
		return QColor(Qt::white); // 默认白色
	}

	// 检查列类型是否为ColorDialog
	QString type = m_columnTypes.value(col);
	if (type == "ColorDialog") {
		QVariant colorData = item->data(Qt::UserRole + 1);
		if (colorData.isValid() && colorData.canConvert<QColor>()) {
			return colorData.value<QColor>();
		}
	}

	return QColor(Qt::white); // 默认白色
}