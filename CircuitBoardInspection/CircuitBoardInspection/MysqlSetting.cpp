#include "MysqlSetting.h"

MysqlSetting::MysqlSetting() {
	
}

bool MysqlSetting::mysqlInit() {
	try
	{
		info.host = HOST_;
		info.user = USER_;
		info.password = PASSWORD_;
		info.database = DATABASE_;
		mysql_conn = mysqlConnect(info);
	}
	catch (const std::exception&e)
	{
		std::cout << e.what() << std::endl;
		return false;
	}
	return true;
}

/***************************�������ݿ�***************************/
MYSQL *MysqlSetting::mysqlConnect(conn_info con) {
	MYSQL *mysql = mysql_init(NULL);	//mysql��ʼ��������ʵ����һ���µĶ���NULLΪ�µĶ��󣬷���Ϊ��ʼ���Դ����Ķ���)
	if (!mysql_real_connect(mysql, con.host, con.user, con.password, con.database, 3306, NULL, 0)) {	//mysql_real_connect(MYSQL *mysql,const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag)
		std::cout << "Connection MySQL Error" << std::endl;
		exit(1);
	}
	std::cout << "Connection MySQL Success" << std::endl;
	mysql_query(mysql, "set names utf8");		// ��Ҫ����һ��
	return mysql;	//����ʵ�����Ķ���
}

/**************************���ݿ��������**************************/
void MysqlSetting::mysqlOperation(char *sql_op) {
	if (mysql_query(mysql_conn, sql_op)) {		//mysql_query()����Ϊ��mysql���в�������һ������Ϊ�ղ�ʵ�����Ķ��󣬵ڶ���λ��Ҫ���еĲ���
		printf("MySQL query error : %s", mysql_error(mysql_conn));
		getchar();
		exit(1);
	}
}

/***************************�Ͽ����ݿ�******************************/
void MysqlSetting::mysqlDisconnect() {
	mysql_close(mysql_conn);//�Ͽ�����
	std::cout << "Disconnect from MySQL" << std::endl;
}

/****************************��ʾ��������***************************/
void MysqlSetting::mysqlShow(MYSQL_RES *res, std::vector<std::string> &colName) {
	MYSQL_ROW col;
	while ((col = mysql_fetch_row(res)) != NULL)	//	row = mysql_fetch_row(res);�������ݵ�ת��,��Ҫѭ��ת��
	{
		for (int i = 0; i < colName.size(); i++)
			std::cout << col[i] << "  ";	//��ʾ������,ע��˴������ֻ�����У���rowҲֻ��2�У���д���л����
		std::cout << std::endl;
	}
}

/*****************************��ѯ����****************************/
MYSQL_RES *MysqlSetting::mysqlSelect(const char *table, MYSQL_RES *res) {
	char cmd[100];

	strcpy(cmd, "select * from ");
	strcat(cmd, table);		//ƴ��ָ��

	mysql_free_result(res);
	mysqlOperation(cmd);
	return mysql_store_result(mysql_conn);//��ѯ�Ľ��,��mysql_store_result�������mysql_affected_rows()���Է��ز�ѯ����������mysql_use_result()����
}

/****************************��ʾ��������***************************/
void MysqlSetting::showAllData(const char *tableName, std::vector<std::string> &colName) {
	MYSQL_RES *res;
	res = NULL;//Ҫ�ȳ�ʼ��������ᱨ��
	for (std::vector<std::string>::const_iterator iter = colName.begin(); iter != colName.end(); ++iter)
		std::cout << *iter << " ";
	std::cout << std::endl;
	res = mysqlSelect(tableName, res);
	mysqlShow(res, colName);
}

/****************************�����޸�ĳ������***********************/
void MysqlSetting::updateData(const char *table, const char *val, const char *dstColName, const char *queryColName, const char *queryName, bool numOrStr) {
	char cmd[100];

	// bool numOrStr = 0  �޸ĵ�Ϊ������
	// bool numOrStr = 1  �޸ĵ�Ϊ�ַ���

	strcpy(cmd, "update ");
	strcat(cmd, table);
	strcat(cmd, " set ");
	strcat(cmd, dstColName);
	strcat(cmd, " = ");
	if (numOrStr)	strcat(cmd, "'");
	strcat(cmd, val);
	if (numOrStr)	strcat(cmd, "'");
	strcat(cmd, " where ");
	strcat(cmd, queryColName);
	strcat(cmd, " = '");
	strcat(cmd, queryName);
	strcat(cmd, "'");

	mysqlOperation(cmd);
}

void MysqlSetting::updateIdData(const char *table, const char *val, const char *dstColName, const char *queryColName, const char *queryName, bool numOrStr, int moldId) {
	char cmd[100];

	// bool numOrStr = 0  �޸ĵ�Ϊ������
	// bool numOrStr = 1  �޸ĵ�Ϊ�ַ���

	strcpy(cmd, "update ");
	strcat(cmd, table);
	strcat(cmd, " set ");
	strcat(cmd, dstColName);
	strcat(cmd, " = ");
	if (numOrStr)	strcat(cmd, "'");
	strcat(cmd, val);
	if (numOrStr)	strcat(cmd, "'");
	strcat(cmd, " where ");
	strcat(cmd, queryColName);
	strcat(cmd, " = '");
	strcat(cmd, queryName);
	strcat(cmd, "'");
	strcat(cmd, " and mold_id = ");
	strcat(cmd, std::to_string(moldId).c_str());

	mysqlOperation(cmd);
}

/****************************ĳ�ű�ĩβ����һ������***********************/
void MysqlSetting::insertData(const char *table, const char *val) {
	char cmd[100];

	strcpy(cmd, "insert into ");
	strcat(cmd, table);
	strcat(cmd, " values ");
	strcat(cmd, val);

	mysqlOperation(cmd);
}

void MysqlSetting::createNewTabelPARAM(const char *tablename) {
	char cmd[1024];

	strcpy(cmd, "CREATE TABLE ");
	strcat(cmd, tablename);
	strcat(cmd, " (");
	strcat(cmd, "id INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "cate INT NOT NULL DEFAULT 0,");
	strcat(cmd, "color_detection INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "color_norm INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "length_detection INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "length_norm INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "length_offset INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "pos_detection INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "pos_start INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "pos_norm INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "pos_offset INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "have_or_not_detection INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "cate_detection INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "direct_detection INT UNSIGNED NOT NULL DEFAULT 0");
	strcat(cmd, ")ENGINE MYISAM CHARSET utf8");

	mysqlOperation(cmd);
}

void MysqlSetting::createNewTabelBLOCK(const char *table) {
	char cmd[1024];

	strcpy(cmd, "CREATE TABLE ");
	strcat(cmd, table);
	strcat(cmd, " (");
	strcat(cmd, "id INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "x1 INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "y1 INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "x2 INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "y2 INT UNSIGNED NOT NULL DEFAULT 0");
	strcat(cmd, ")ENGINE MYISAM CHARSET utf8");

	mysqlOperation(cmd);
}

void MysqlSetting::createNewTabelLengthPixelParam(const char *table) {
	char cmd[1024];

	strcpy(cmd, "CREATE TABLE ");
	strcat(cmd, table);
	strcat(cmd, " (");
	strcat(cmd, "id INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "length_pixel_norm VARCHAR(20) NOT NULL DEFAULT \"0\",");
	strcat(cmd, "length_pixel_offset VARCHAR(20) NOT NULL DEFAULT \"0\",");
	strcat(cmd, "length_angle VARCHAR(20) NOT NULL DEFAULT \"0.0\"");
	strcat(cmd, ")ENGINE MYISAM CHARSET utf8");

	mysqlOperation(cmd);
}

void MysqlSetting::createNewTabelTrueTargetBlock(const char *table) {
	char cmd[1024];

	strcpy(cmd, "CREATE TABLE ");
	strcat(cmd, table);
	strcat(cmd, " (");
	strcat(cmd, "id INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "x1 INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "y1 INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "x2 INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "y2 INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "center_x INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "center_y INT UNSIGNED NOT NULL DEFAULT 0");
	strcat(cmd, ")ENGINE MYISAM CHARSET utf8");

	mysqlOperation(cmd);
}

void MysqlSetting::createNewTabelPosPixelParam(const char *table) {
	char cmd[1024];

	strcpy(cmd, "CREATE TABLE ");
	strcat(cmd, table);
	strcat(cmd, " (");
	strcat(cmd, "id INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "pos_length_pixel_norm INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "pos_length_pixel_offset INT UNSIGNED NOT NULL DEFAULT 0");
	strcat(cmd, ")ENGINE MYISAM CHARSET utf8");

	mysqlOperation(cmd);
}

void MysqlSetting::createNewTabelPlugDirectMaskMsg(const char *table) {
	char cmd[1024];

	strcpy(cmd, "CREATE TABLE ");
	strcat(cmd, table);
	strcat(cmd, " (");
	strcat(cmd, "id INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "block_idx INT UNSIGNED NOT NULL DEFAULT 0,");
	strcat(cmd, "mask_cate_id INT UNSIGNED NOT NULL DEFAULT 0");
	strcat(cmd, ")ENGINE MYISAM CHARSET utf8");

	mysqlOperation(cmd);
}

// ��ȡHSV��ɫ��Ϣ
void MysqlSetting::readColorHSVParam(std::unordered_map<std::string, HSVLimit> &m_HSVColor) {
	std::vector<std::vector<std::string>> colorHSVParam_str = tab2Vector("hsv_color_param", 7);
	HSVLimit m_hsvLimit;
	for (int i = 0; i < colorHSVParam_str.size(); ++i) {
		m_hsvLimit.hMin = atoi(colorHSVParam_str[i][1].c_str());
		m_hsvLimit.hMax = atoi(colorHSVParam_str[i][2].c_str());
		m_hsvLimit.sMin = atoi(colorHSVParam_str[i][3].c_str());
		m_hsvLimit.sMax = atoi(colorHSVParam_str[i][4].c_str());
		m_hsvLimit.vMin = atoi(colorHSVParam_str[i][5].c_str());
		m_hsvLimit.vMax = atoi(colorHSVParam_str[i][6].c_str());
		if (m_HSVColor.find(colorHSVParam_str[i][0]) != m_HSVColor.end()) {
			m_HSVColor[colorHSVParam_str[i][0]] = m_hsvLimit;
		}
		else {
			m_HSVColor.insert(std::make_pair(colorHSVParam_str[i][0], m_hsvLimit));
		}
	}
}

// ����HSV��ɫ��Ϣ
void MysqlSetting::modifyColorHSVParam(std::unordered_map<std::string, HSVLimit> &m_HSVColor) {
	char cmd[2000];

	strcpy(cmd, "delete from hsv_color_param");

	mysqlOperation(cmd);

	std::vector<std::vector<std::string>> colorHSVParam_str;
	for (auto iter = m_HSVColor.begin(); iter != m_HSVColor.end(); ++iter) {
		std::vector<std::string> _perLine;
		_perLine.push_back(iter->first);
		_perLine.push_back(std::to_string(iter->second.hMin));
		_perLine.push_back(std::to_string(iter->second.hMax));
		_perLine.push_back(std::to_string(iter->second.sMin));
		_perLine.push_back(std::to_string(iter->second.sMax));
		_perLine.push_back(std::to_string(iter->second.vMin));
		_perLine.push_back(std::to_string(iter->second.vMax));
		colorHSVParam_str.push_back(_perLine);
	}

	if (!colorHSVParam_str.empty()) {
		strcpy(cmd, "insert into hsv_color_param");
		strcat(cmd, " values ");
		for (int i = 0; i < colorHSVParam_str.size(); i++) {
			strcat(cmd, "(");
			strcat(cmd, "\"");
			strcat(cmd, colorHSVParam_str[i][0].c_str());
			strcat(cmd, "\"");
			for (int j = 1; j < colorHSVParam_str[i].size(); j++) {
				strcat(cmd, ",");
				strcat(cmd, colorHSVParam_str[i][j].c_str());
			}
			if (i != (colorHSVParam_str.size() - 1))	strcat(cmd, "),");
			else strcat(cmd, ")");
		}
		
		mysqlOperation(cmd);
	}
}

/****************************��ѯĳ������***************************/
char *MysqlSetting::queryData(const char *table, const char *dstColName, const char *queryColName, const char *queryName) {
	char cmd[100];
	MYSQL_RES *res;
	MYSQL_ROW col;
	res = NULL;//Ҫ�ȳ�ʼ��������ᱨ��

	strcpy(cmd, "select ");
	strcat(cmd, dstColName);
	strcat(cmd, " from ");
	strcat(cmd, table);
	strcat(cmd, " where ");
	strcat(cmd, queryColName);
	strcat(cmd, " = ");
	strcat(cmd, "\"");
	strcat(cmd, queryName);
	strcat(cmd, "\"");

	mysql_free_result(res);
	mysqlOperation(cmd);
	res = mysql_store_result(mysql_conn);
	col = mysql_fetch_row(res);

	return col[0];
}
/****************************��ѯ���б�����ݷ��ض�άvector***************************/
std::vector<std::vector<std::string>> MysqlSetting::tab2Vector(const char * table, const int colNum) {
	char cmd[100];

	MYSQL_RES *res;
	MYSQL_ROW col;

	std::vector<std::vector<std::string>> vect;
	std::vector<std::string> _vect;

	res = NULL;//Ҫ�ȳ�ʼ��������ᱨ��

	strcpy(cmd, "select * from ");
	strcat(cmd, table);		//ƴ��ָ��

	mysql_free_result(res);
	mysqlOperation(cmd);
	res = mysql_store_result(mysql_conn);

	while ((col = mysql_fetch_row(res)) != NULL)		// ÿһ�з���
	{
		for (int i = 0; i < colNum; i++) {
			_vect.push_back(col[i]);
		}
		vect.push_back(_vect);
		for (int i = 0; i < colNum; i++) {
			_vect.pop_back();
		}
	}

	return vect;
}

std::vector<std::vector<std::string>> MysqlSetting::tab2Vector(const char * table, std::string name, int colNum) {
	char cmd[100];

	MYSQL_RES *res;
	MYSQL_ROW col;

	std::vector<std::vector<std::string>> vect;
	std::vector<std::string> _vect;

	res = NULL;//Ҫ�ȳ�ʼ��������ᱨ��

	strcpy(cmd, "select * from ");
	strcat(cmd, table);		//ƴ��ָ��
	strcat(cmd, " where model_name = ");		//ƴ��ָ��
	strcat(cmd, "\"");
	strcat(cmd, name.c_str());
	strcat(cmd, "\"");

	mysql_free_result(res);
	mysqlOperation(cmd);
	res = mysql_store_result(mysql_conn);

	while ((col = mysql_fetch_row(res)) != NULL)		// ÿһ�з���
	{
		for (int i = 0; i < colNum; i++) {
			_vect.push_back(col[i]);
		}
		vect.push_back(_vect);
		for (int i = 0; i < colNum; i++) {
			_vect.pop_back();
		}
	}

	return vect;
}

/****************************��ѯ���б�����ݷ��ض�άvector����ID***************************/
std::vector<std::vector<std::string>> MysqlSetting::tab2VectorOrderById(const char * table, const int colNum) {
	char cmd[100];

	MYSQL_RES *res;
	MYSQL_ROW col;

	std::vector<std::vector<std::string>> vect;
	std::vector<std::string> _vect;

	res = NULL;//Ҫ�ȳ�ʼ��������ᱨ��

	strcpy(cmd, "select * from ");
	strcat(cmd, table);		//ƴ��ָ��
	strcat(cmd, " order by id asc");		//ƴ��ָ��

	mysql_free_result(res);
	mysqlOperation(cmd);
	res = mysql_store_result(mysql_conn);

	while ((col = mysql_fetch_row(res)) != NULL)		// ÿһ�з���
	{
		for (int i = 0; i < colNum; i++) {
			_vect.push_back(col[i]);
		}
		vect.push_back(_vect);
		for (int i = 0; i < colNum; i++) {
			_vect.pop_back();
		}
	}

	return vect;
}

std::vector<std::vector<std::string>> MysqlSetting::tabId2Vector(const char * table, int moldId, const int colNum) {
	char cmd[150];

	MYSQL_RES *res;
	MYSQL_ROW col;

	std::vector<std::vector<std::string>> vect;
	std::vector<std::string> _vect;

	res = NULL;//Ҫ�ȳ�ʼ��������ᱨ��

	strcpy(cmd, "select * from ");
	strcat(cmd, table);		//ƴ��ָ��
	strcat(cmd, " where mold_id = ");
	strcat(cmd, std::to_string(moldId).c_str());
	if (colNum == 31 || colNum == 2 || colNum == 3) {
		strcat(cmd, " order by id asc");
	}

	mysql_free_result(res);
	mysqlOperation(cmd);
	res = mysql_store_result(mysql_conn);

	while ((col = mysql_fetch_row(res)) != NULL)		// ÿһ�з���
	{
		for (int i = 0; i < colNum; i++) {
			_vect.push_back(col[i]);
		}
		vect.push_back(_vect);
		for (int i = 0; i < colNum; i++) {
			_vect.pop_back();
		}
	}

	return vect;
}

/****************************��ѯ���з���map***************************/
std::map<std::string, std::string> MysqlSetting::queryAllData(const char *table, const char *Colname1, const char *Colname2) {
	char cmd[100];

	MYSQL_RES *res;
	MYSQL_ROW col;

	std::map<std::string, std::string> m;
	std::queue<std::string> name;
	std::queue<std::string> val;

	res = NULL;//Ҫ�ȳ�ʼ��������ᱨ��

	strcpy(cmd, "select ");
	strcat(cmd, Colname1);
	strcat(cmd, ", ");
	strcat(cmd, Colname2);
	strcat(cmd, " from ");
	strcat(cmd, table);

	mysql_free_result(res);
	mysqlOperation(cmd);
	res = mysql_store_result(mysql_conn);

	while ((col = mysql_fetch_row(res)) != NULL)
	{
		name.push(col[0]);
		val.push(col[1]);
	}

	while (!name.empty()) {
		m[name.front()] = val.front();
		name.pop();
		val.pop();
	}

	return m;
}
/****************************��ѯ������������***************************/
void MysqlSetting::queryAllColName(std::vector<std::string> &vec_colName, const char *table) {
	char cmd[150];

	MYSQL_RES *res;
	MYSQL_ROW col;

	res = NULL;//Ҫ�ȳ�ʼ��������ᱨ��

	strcpy(cmd, "select column_name from information_schema.columns where table_name = ");
	strcat(cmd, table);

	mysql_free_result(res);
	mysqlOperation(cmd);
	res = mysql_store_result(mysql_conn);

	while ((col = mysql_fetch_row(res)) != NULL)
	{
		vec_colName.push_back(col[0]);
	}
}

void MysqlSetting::detManualNameAndNewData(const char *table, std::vector<std::vector<std::string>> vect_chg) {
	char cmd[2000];

	strcpy(cmd, "delete from ");
	strcat(cmd, table);

	mysqlOperation(cmd);

	if (!vect_chg.empty()) {
		strcpy(cmd, "insert into ");
		strcat(cmd, table);
		strcat(cmd, " values ");
		for (int i = 0; i < vect_chg.size(); i++) {
			strcat(cmd, "(");
			strcat(cmd, vect_chg[i][0].c_str());
			strcat(cmd, ",");
			strcat(cmd, "\"");
			strcat(cmd, vect_chg[i][1].c_str());
			strcat(cmd, "\",");
			strcat(cmd, vect_chg[i][2].c_str());
			if (i != (vect_chg.size() - 1))	strcat(cmd, "),");
			else strcat(cmd, ")");
		}
		mysqlOperation(cmd);
	}
}
/****************************ɾ�������������ݲ���ֵ***************************/
void MysqlSetting::detAndNewData(const char *table, std::vector<std::vector<std::string>> vect_chg) {
	char cmd[2000];

	strcpy(cmd, "delete from ");
	strcat(cmd, table);

	mysqlOperation(cmd);

	if (!vect_chg.empty()) {
		strcpy(cmd, "insert into ");
		strcat(cmd, table);
		strcat(cmd, " values ");
		for (int i = 0; i < vect_chg.size(); i++) {
			strcat(cmd, "(");
			//strcat(cmd, std::to_string(i + 1).c_str());
			for (int j = 0; j < vect_chg[i].size(); j++) {
				if(j != 0) strcat(cmd, ",");
				strcat(cmd, "\"");
				strcat(cmd, vect_chg[i][j].c_str());
				strcat(cmd, "\"");
			}
			if (i != (vect_chg.size() - 1))	strcat(cmd, "),");
			else strcat(cmd, ")");
		}
		mysqlOperation(cmd);
	}
}

void MysqlSetting::detAndNewData(const char *table, std::vector<std::vector<int>> vect_chg) {
	char cmd[2000];

	strcpy(cmd, "delete from ");
	strcat(cmd, table);

	mysqlOperation(cmd);

	if (!vect_chg.empty()) {
		strcpy(cmd, "insert into ");
		strcat(cmd, table);
		strcat(cmd, " values ");
		for (int i = 0; i < vect_chg.size(); i++) {
			strcat(cmd, "(");
			//strcat(cmd, std::to_string(i + 1).c_str());
			for (int j = 0; j < vect_chg[i].size(); j++) {
				if (j != 0) strcat(cmd, ",");
				//strcat(cmd, "\"");
				strcat(cmd, std::to_string(vect_chg[i][j]).c_str());
				//strcat(cmd, "\"");
			}
			if (i != (vect_chg.size() - 1))	strcat(cmd, "),");
			else strcat(cmd, ")");
		}
		mysqlOperation(cmd);
	}
}

void MysqlSetting::detAndNewDataWithID(const char *table, std::vector<std::vector<int>> vect_chg) {
	char cmd[2000];

	strcpy(cmd, "delete from ");
	strcat(cmd, table);

	mysqlOperation(cmd);

	if (!vect_chg.empty()) {
		strcpy(cmd, "insert into ");
		strcat(cmd, table);
		strcat(cmd, " values ");
		for (int i = 0; i < vect_chg.size(); i++) {
			strcat(cmd, "(");
			strcat(cmd, std::to_string(i).c_str());
			for (int j = 0; j < vect_chg[i].size(); j++) {
				strcat(cmd, ",");
				//strcat(cmd, "\"");
				strcat(cmd, std::to_string(vect_chg[i][j]).c_str());
				//strcat(cmd, "\"");
			}
			if (i != (vect_chg.size() - 1))	strcat(cmd, "),");
			else strcat(cmd, ")");
		}
		mysqlOperation(cmd);
	}
}

void MysqlSetting::detAndNewDataWithID(const char *table, std::vector<std::vector<std::string>> vect_chg) {
	char cmd[2000];

	strcpy(cmd, "delete from ");
	strcat(cmd, table);

	mysqlOperation(cmd);

	if (!vect_chg.empty()) {
		strcpy(cmd, "insert into ");
		strcat(cmd, table);
		strcat(cmd, " values ");
		for (int i = 0; i < vect_chg.size(); i++) {
			strcat(cmd, "(");
			strcat(cmd, std::to_string(i).c_str());
			for (int j = 0; j < vect_chg[i].size(); j++) {
				strcat(cmd, ",");
				strcat(cmd, "\"");
				strcat(cmd, vect_chg[i][j].c_str());
				strcat(cmd, "\"");
			}
			if (i != (vect_chg.size() - 1))	strcat(cmd, "),");
			else strcat(cmd, ")");
		}
		mysqlOperation(cmd);
	}

}

void MysqlSetting::detStepIdAndNewData(const char *table, std::vector<std::vector<std::string>> vect_chg, int moldId) {
	char cmd[2500];

	strcpy(cmd, "delete from ");
	strcat(cmd, table);
	strcat(cmd, " where mold_id = ");
	strcat(cmd, std::to_string(moldId).c_str());

	mysqlOperation(cmd);

	if (!vect_chg.empty()) {
		strcpy(cmd, "insert into ");
		strcat(cmd, table);
		strcat(cmd, " values ");
		for (int i = 0; i < vect_chg.size(); i++) {
			strcat(cmd, "(");
			strcat(cmd, std::to_string(i + 1).c_str());
			for (int j = 0; j < vect_chg[i].size(); j++) {
				strcat(cmd, ",");
				strcat(cmd, "\"");
				strcat(cmd, vect_chg[i][j].c_str());
				strcat(cmd, "\"");
			}
			if (i != (vect_chg.size() - 1))	strcat(cmd, "),");
			else strcat(cmd, ")");
		}
		mysqlOperation(cmd);
	}
}

void MysqlSetting::detMoldIdAndNewData(const char *table, std::vector<std::vector<std::string>> vect_chg, int moldId) {
	char cmd[2000];

	strcpy(cmd, "delete from ");
	strcat(cmd, table);
	strcat(cmd, " where mold_id = ");
	strcat(cmd, std::to_string(moldId).c_str());

	mysqlOperation(cmd);

	if (!vect_chg.empty()) {
		strcpy(cmd, "insert into ");
		strcat(cmd, table);
		strcat(cmd, " values ");
		for (int i = 0; i < vect_chg.size(); i++) {
			strcat(cmd, "(");
			strcat(cmd, std::to_string(moldId).c_str());
			for (int j = 0; j < vect_chg[i].size(); j++) {
				strcat(cmd, ",");
				strcat(cmd, "\"");
				strcat(cmd, vect_chg[i][j].c_str());
				strcat(cmd, "\"");
			}
			if (i != (vect_chg.size() - 1))	strcat(cmd, "),");
			else strcat(cmd, ")");
		}
		mysqlOperation(cmd);
	}
}

/****************************��ѯ��������***************************/
int MysqlSetting::queryRowCnt(const char *table) {
	char cmd[100];

	MYSQL_RES *res;
	MYSQL_ROW col;

	res = NULL;//Ҫ�ȳ�ʼ��������ᱨ��

	strcpy(cmd, "select count(*) from ");
	strcat(cmd, table);

	mysql_free_result(res);
	mysqlOperation(cmd);
	res = mysql_store_result(mysql_conn);

	col = mysql_fetch_row(res);

	return atoi(col[0]);
}

int MysqlSetting::queryIdRowCnt(const char *table, int moldId) {
	char cmd[100];

	MYSQL_RES *res;
	MYSQL_ROW col;

	res = NULL;//Ҫ�ȳ�ʼ��������ᱨ��

	strcpy(cmd, "select count(*) from ");
	strcat(cmd, table);
	strcat(cmd, " where mold_id = ");
	strcat(cmd, std::to_string(moldId).c_str());

	mysql_free_result(res);
	mysqlOperation(cmd);
	res = mysql_store_result(mysql_conn);

	col = mysql_fetch_row(res);

	return atoi(col[0]);
}

/****************************������־��Ϣ***************************/
void MysqlSetting::insertLogData(const char *table, std::string time, std::string res, std::string mesg) {
	char cmd[200];

	strcpy(cmd, "insert into ");
	strcat(cmd, table);
	strcat(cmd, "(time_occur, result, error_image_message)");
	strcat(cmd, " values ");
	strcat(cmd, "(");
	strcat(cmd, "\"");
	strcat(cmd, time.c_str());
	strcat(cmd, "\"");
	strcat(cmd, ",");
	strcat(cmd, "\"");
	strcat(cmd, res.c_str());
	strcat(cmd, "\"");
	strcat(cmd, ",");
	strcat(cmd, "\"");
	strcat(cmd, mesg.c_str());
	strcat(cmd, "\"");
	strcat(cmd, ")");

	mysqlOperation(cmd);
}

void MysqlSetting::insertLogData(const char *table, std::vector<std::string> _thisLog) {
	char cmd[200];

	strcpy(cmd, "insert into ");
	strcat(cmd, table);
	strcat(cmd, " values ");
	strcat(cmd, "(");
	strcat(cmd, "\"");
	strcat(cmd, _thisLog[0].c_str());
	strcat(cmd, "\"");
	strcat(cmd, ",");
	strcat(cmd, "\"");
	strcat(cmd, _thisLog[1].c_str());
	strcat(cmd, "\"");
	strcat(cmd, ",");
	strcat(cmd, "\"");
	strcat(cmd, _thisLog[2].c_str());
	strcat(cmd, "\"");
	strcat(cmd, ")");

	mysqlOperation(cmd);
}

MysqlSetting::~MysqlSetting() {
	mysqlDisconnect();
}