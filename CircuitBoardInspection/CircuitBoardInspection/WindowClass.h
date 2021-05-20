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

	bool RS232Init();	// �򿪴���
	bool RS232Close();	// �رմ���

	std::string curRunModel = "";

	int mCameraWidth = 0;	// �����������궨��Ŀ��
	int mCameraHeight = 0;	// �����������궨��֡�߶�
	cv::Mat matForMutiTemp;	// ����ͼ�����ڶ�ģ�屣��
	bool canGetDoubleClickedSignal = false;

	std::unordered_map<std::string, HSVLimit> HSVColorForGetTrueTarget;
	std::vector<std::vector<int>> landMarkWithId;	
	std::vector<std::vector<int>> vectParamForModel;
	std::vector<std::vector<std::string>> lengthPixel;
	std::vector<std::vector<int>> trueTargetPartAxis;	// ���ڴ������Ŀ�����������Ϣ
	std::vector<std::vector<int>> posProcessParam;	// ����λ�ü��������Ϣ
	std::vector<std::vector<int>> plugDirectMaskMsg;	// �����ͷ������mask��Ϣ
	std::vector<cv::Mat> maskForTargetBlock;
	// std::vector<int> multiTempIdx;	// ���ڼ�¼������ģ����ŵ�����
	std::map<int, std::list<cv::Mat>> multiTempMat;	// <block����, ��block�����Ķ�ģ��>

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
