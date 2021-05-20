#pragma execution_character_set("utf-8")
#include "Statistics.h"

Statistics::Statistics(MysqlSetting *mysql_conn)
: mysql_conn(mysql_conn)
{
	ui.setupUi(this);

	myModelName = mysql_conn->tab2Vector("model", 4);
	updateMyModelNameOnCombobox();
	if (myModelName.size() > 0)
		updateMsgByModelName(myModelName[0][1]);

	connect(ui.comboBoxName, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxName(int)));
}

void Statistics::onComboBoxName(int idx) {
	updateMsgByModelName(ui.comboBoxName->currentText().toStdString());
}

void Statistics::updateMsgByModelName(std::string name) {
	std::vector<std::vector<std::string>> tmpQuery = mysql_conn->tab2Vector("log", name, 3);
	//for (int i = 0; i < tmpQuery.size(); ++i) {
	//	for (int j = 0; j < tmpQuery[i].size(); ++j) {
	//		std::cout << tmpQuery[i][j] << " ";
	//	}
	//	std::cout << std::endl;
	//}
	ui.tableWidgetMsg->clear();
	ui.tableWidgetMsg->setHorizontalHeaderLabels(QStringList() << QString::fromUtf8("ʱ��")
		<< QString::fromUtf8("�ͺ�") << QString::fromUtf8("��Ϣ"));
	ui.tableWidgetMsg->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	if (tmpQuery.size() > 0) {
		ui.tableWidgetMsg->setRowCount(tmpQuery.size());
		for (int i = 0; i < tmpQuery.size(); ++i) {
			for (int j = 0; j < tmpQuery[i].size(); ++j) {
				ui.tableWidgetMsg->setItem(i, j, new QTableWidgetItem(QString::fromStdString(tmpQuery[i][j])));
				ui.tableWidgetMsg->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);	// ���õ�Ԫ�����
			}
		}
	}
	else {
		QMessageBox::warning(NULL, "����", "δ��ѯ���������ݣ�", QMessageBox::Yes);
	}
}

void Statistics::updateMyModelNameOnCombobox() {
	for (int i = 0; i < myModelName.size(); ++i) {
		ui.comboBoxName->addItem(QString::fromStdString(myModelName[i][1]));
	}
	ui.comboBoxName->addItem("test");
}

Statistics::~Statistics() {

}