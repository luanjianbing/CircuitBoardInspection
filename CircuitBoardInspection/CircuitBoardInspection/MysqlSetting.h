#pragma once
#include <winsock.h>
#include <mysql.h>
//#include <iostream>
#include <string>
//#include <vector>
#include <queue>
#include <map>
//#include <unordered_map>

#include "common.h"

#define HOST_ "localhost"
#define USER_ "root"
#define PASSWORD_ "111111"
#define DATABASE_ "circuitboarddatabase"

struct conn_info {
	char *host;
	char *user;
	char *password;
	char *database;
};


class MysqlSetting {
public:
	MysqlSetting();
	~MysqlSetting();

	bool mysqlInit();

	// ��ѯ�������
	int queryRowCnt(const char *table);

	// ��ѯ����moldId�ĸ���
	int queryIdRowCnt(const char *table, int moldId);

	// table all data
	void showAllData(const char *tableName, std::vector<std::string> &colName);

	// ��ѯĳ������ select (dstColName) from (table) where (queryColName) = (queryName);
	char *queryData(const char *table, const char *dstColName, const char *queryColName, const char *queryName);

	// ��ѯ�������ݣ���������������ɵ�map	select (Colname1), (Colname2) from table;
	std::map<std::string, std::string> queryAllData(const char *table, const char *Colname1, const char *Colname2);

	// ��ѯ�������ݣ���������������ɵ�vector	select * from (table)
	std::vector<std::vector<std::string>> tab2Vector(const char * table, const int colNum);
	std::vector<std::vector<std::string>> tab2Vector(const char * table, std::string name, int colNum);

	// ��ѯ��񷵻�vector order by id
	std::vector<std::vector<std::string>> tab2VectorOrderById(const char * table, const int colNum);

	// ��ѯ����moldid�����ݴ���vector
	std::vector<std::vector<std::string>> tabId2Vector(const char * table, int moldId, const int colNum);

	// ��ĩβ�������� insert into (table) values (val);
	void insertData(const char *table, const char *val);

	// ���±��е�ĳ������ update (table) set (dstColName) = (val) where (queryColName) = (queryName) 
	void updateData(const char *table, const char *val, const char *dstColName, const char *queryColName, const char *queryName, bool numOrStr);

	// ���ķ���moldid�Ĳ���
	void updateIdData(const char *table, const char *val, const char *dstColName, const char *queryColName, const char *queryName, bool numOrStr, int moldId);

	// ��ѯ����������������ط���һ��vector����
	void queryAllColName(std::vector<std::string> &vec_colName, const char *table);

	// ɾ�������������ݲ��������vector��ֵ
	void detAndNewData(const char *table, std::vector<std::vector<std::string>> vect_chg);
	void detAndNewData(const char *table, std::vector<std::vector<int>> vect_chg);
	void detAndNewDataWithID(const char *table, std::vector<std::vector<int>> vect_chg);
	void detAndNewDataWithID(const char *table, std::vector<std::vector<std::string>> vect_chg);

	// ɾ������moldid�����ݴ���vector
	void detMoldIdAndNewData(const char *table, std::vector<std::vector<std::string>> vect_chg, int moldId);

	void detStepIdAndNewData(const char *table, std::vector<std::vector<std::string>> vect_chg, int moldId);

	// ������־��Ϣ
	void insertLogData(const char *table, std::string time, std::string res, std::string mesg);
	void insertLogData(const char *table, std::vector<std::string> _thisLog);

	// ����manual name ��Ϣ
	void detManualNameAndNewData(const char *table, std::vector<std::vector<std::string>> vect_chg);

	void createNewTabelPARAM(const char *table);
	void createNewTabelBLOCK(const char *table);
	void createNewTabelLengthPixelParam(const char *table);
	void createNewTabelTrueTargetBlock(const char *table);
	void createNewTabelPosPixelParam(const char *table);
	void createNewTabelPlugDirectMaskMsg(const char *table);
	// ��ȡHSV��ɫ��Ϣ
	void readColorHSVParam(std::unordered_map<std::string, HSVLimit> &m_HSVColor);
	// ����HSV��ɫ��Ϣ
	void modifyColorHSVParam(std::unordered_map<std::string, HSVLimit> &m_HSVColor);

private:
	MYSQL *mysql_conn;
	conn_info info;

	// connect
	MYSQL *mysqlConnect(conn_info con);

	// operation cmd
	void mysqlOperation(char *sql_op);

	// for show
	void mysqlShow(MYSQL_RES *res, std::vector<std::string> &colName);
	MYSQL_RES *mysqlSelect(const char *table, MYSQL_RES *res);

	// disconnect from sql
	void mysqlDisconnect();
};