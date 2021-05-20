#pragma once

#include <QMainWindow>
#include <QDateTime>

#include "CircuitBoardInspection.h"
#include "Statistics.h"

#include "ThreadForRealTimeGrab.h"
#include "ThreadForListenExternalSignal.h"

#include "ui_Window.h"

#include <list>

class WindowClass : public QMainWindow
{
	Q_OBJECT

public:
	WindowClass(MyCamera *myCamera, MysqlSetting *mysql_conn, QMainWindow *parent = Q_NULLPTR);
	~WindowClass();

private:
	Ui::mainWindowClass ui;

	MyCamera *myCamera;
	MysqlSetting *mysql_conn;
	QSerialPort *m_serial;

	QTimer *m_serialTimer = NULL;
	std::string lastPortGetChar = "";
	//char targetChar = 'A';

	bool RS232Init();	// 打开串口
	bool RS232Close();	// 关闭串口

	std::string curRunModel = "";

	int mCameraWidth = 0;	// 经过最大区域标定后的宽度
	int mCameraHeight = 0;	// 经过最大区域标定后帧高度
	cv::Mat matForMutiTemp;	// 保存图像用于多模板保存
	bool canGetDoubleClickedSignal = false;

	std::unordered_map<std::string, HSVLimit> HSVColorForGetTrueTarget;
	std::vector<std::vector<int>> landMarkWithId;	
	std::vector<std::vector<int>> vectParamForModel;
	std::vector<std::vector<std::string>> lengthPixel;
	std::vector<std::vector<int>> trueTargetPartAxis;	// 用于存放真正目标物的坐标信息
	std::vector<std::vector<int>> posProcessParam;	// 保存位置检测像素信息
	std::vector<std::vector<int>> plugDirectMaskMsg;	// 保存插头方向检测mask信息
	std::vector<cv::Mat> maskForTargetBlock;
	// std::vector<int> multiTempIdx;	// 用于记录各个多模板序号到几了
	std::map<int, std::list<cv::Mat>> multiTempMat;	// <block索引, 该block索引的多模板>

	bool realTimeGrab = false;
	ThreadForRealTimeGrab *m_realTimeGrabThread;
	ThreadForListenExternalSignal *m_listenExternalSignal;
	ImageProcess *imageProcess;

	cv::Mat normTemp;

	void updateDisplay(cv::Mat &dstMaxRoi, std::vector<int> &res);

	int totalCounts = 0;
	int OKCounts = 0;
	int NGCounts = 0;

	//std::vector<std::vector<std::string>> thisLog;

	void insertIntoTableLog(std::string thisLog);

	int getDoubleClickedBlock(int x, int y);

private slots:
	void onActionClose();
	void onActionEnlarge();
	void onActionRun();
	void onActionImageSetting();

	void dealSignalLoadModel(QString);

	void dealSignalGrabFrameOver();

	void dealSignalGetExternalSignal();

	void onButtonRun();

	void onActionStatistics();

	void portReadyRead();

	void onPortSendSingleChar();

protected:
	//void closeEvent(QCloseEvent *event);

	void mouseDoubleClickEvent(QMouseEvent *);
};
