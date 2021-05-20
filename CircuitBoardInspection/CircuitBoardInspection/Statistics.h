#pragma once
#include <QtWidgets/QWidget>
#include "ui_Statistics.h"

#include <QComboBox>
#include <QMessageBox>
#include <QHeaderView>

#include "MysqlSetting.h"

class Statistics :public QWidget {
	Q_OBJECT

public:
	Statistics(MysqlSetting *mysql_conn);
	~Statistics();
private:
	Ui::StatisticClass ui;

	MysqlSetting *mysql_conn;

	std::vector<std::vector<std::string>> myModelName;
	void updateMyModelNameOnCombobox();

	void updateMsgByModelName(std::string name);
private slots:
	void onComboBoxName(int);
};