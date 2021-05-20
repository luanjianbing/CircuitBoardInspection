#pragma once

#include <QtWidgets/QWidget>
#include "ui_CircuitBoardInspection.h"

//#include <unordered_map>

//#include "common.h"
#include "MyCamera.h"
#include "MysqlSetting.h"
//#include "ImageProcess.h"
#include "ProcessDisplay.h"
#include <io.h>

#include <QTimer>
#include <QMouseEvent>
#include <QPen>
#include <QPainter>
#include <QEvent>
#include <QComboBox>
#include <QButtonGroup>

class CircuitBoardInspection : public QWidget
{
    Q_OBJECT

public:
    CircuitBoardInspection(MyCamera *myCamera, MysqlSetting *mysql_conn, QWidget *parent = Q_NULLPTR);
	~CircuitBoardInspection();

private:
    Ui::CircuitBoardInspectionClass ui;

	MyCamera *myCamera;
	MysqlSetting *mysql_conn;

	enum STEP {
		ON_GET_IMAGE = 0,			// ��ʼ״̬��ֻ���Ի�ȡ����ͼ��
		ON_GET_CONTINUOUS_IMAGE,	// ��ȡ����ͼ��
		ON_LOAD_OR_CREATE_MODEL,	// ����ѡ���Ǽ�������ģ�ͻ����½�ģ����Ϣ
		ON_LOAD_MODEL,				// ��������ģ��
		ON_CREATE_NEW_MODEL,		// �½�ģ��
		ON_MAX_AREA_MARK,			// ���Χ���
		//ON_COLOR_MARK,
		ON_PART_AREA_MARK,			// �ֲ����
		ON_PART_AREA_MARK_SCALE,	// �ֲ��Ŵ������
		ON_PART_PARAM_SET,	
		ON_CREATE_NEW_MODEL_FINISH,	// �½�ģ�ͽ���

		ON_SET_TABLE_PARAM,			// ���ñ������в���
		ON_SET_BLOCK_PARAM,			// ����ÿ����ľ������
	};

	STEP curState = ON_GET_IMAGE;
	QString newModelName = "";	// �½�ģ������

	bool isOnLandMark = false;	// �Ƿ��ڱ��״̬
	bool isOnPartROIDraw = false;	// ���ڱ���Ƿ��Ǿֲ�ROI���껹���������Χ��ROI����

	QTimer *timerForContinuousFrame = NULL;

	std::vector<std::vector<int>> landMark;	// ����������Ϣ
	cv::Mat matForDraw;	// ������ͼ��ͼ��
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	cv::Size frameSize;	// ��ȡͼ���С
	float adaptRatio = 0.0;	// ��¼ʵ�����ŵı���
	int offsetY = 0;	// �����Ǿ�����ʾ������ʵ����Y������һ��ƫ����
	void landMarkSave();
	void landMarkRun(cv::Mat &mat);	// �ͽ�ȥ����
	cv::Rect partRect = cv::Rect(0, 0, 0, 0);	// ���ڷŴ���ʾ
	float partScale = 1.5;	// ����Ȱ���
	cv::Mat matForDrawPart;	// �ֲ��Ŵ�ͼ��
	int partStartX = 0, partStartY = 0, partEndX = 0, partEndY = 0, trueStartX = 0, trueStartY = 0;

	std::vector<std::vector<int>> vectParamForNewModel;
	// [1]������ : (int)
	// [2]�������� : ����(0) / ����(1) / ��ͷ(2)
	// [3]��ɫ��� : ��(0) / ��(1)
	// [4]��ɫ���ֵ :	1.����(��������[��(0)����(1)����(2)] / T18R[��(3)����(4)����(5)])	// 0~5	6��
	//					2.����(��(6)����(7)����(8)����(9)����(10)����(11))		// 6~11	6��
	//					3.��ͷ(��(12)����(13)����(14)����(15)����(16)����(17)����(18))	// 12~18	7��
	// [5]�������Ƽ��(ָ����������) : ��(0) / ��(1)
	// [6]���ȹ��ֵ : (int) ��λΪmm > 0
	// [7]����ƫ��ֵ : (int) ��λΪmm > 0
	// [8]λ�ü�� : ��(0) / ��(1)
	// [9]��ʼID :	����ʼID����ǰID�ľ��룬(int)
	// [10]λ�ù��ֵ : (int) ��λΪmm > 0
	// [11]λ��ƫ��ֵ : (int) ��λΪmm > 0
	// [12]���޼�� : ��(0) / ��(1)
	// [13]���ͼ�� : ��(0) / ��(1)
	// [14]������ : ��(0) / ��(1)
	int curSetBlock = -1;	// ��ǰ���õڼ���block�Ĳ���
	QButtonGroup *btnGroupHaveOrNot;
	QButtonGroup *btnGroupColor;
	QButtonGroup *btnGroupCate;
	QButtonGroup *btnGroupDirection;
	QButtonGroup *btnGroupPos;
	QButtonGroup *btnGroupLength;

	std::vector<std::vector<std::string>> myModelsName;	// ��ȡ�Ѿ�������ģ����
	std::unordered_map<std::string, HSVLimit> HSVColorForGetTrueTarget;	// ��ȡ������ɫ��Ϣ

	//ImageProcess *m_ImageProcessTest;
	ProcessDisplay *m_ProcessDisplay;

	std::vector<std::vector<std::string>> lengthProcessParam;	// ���泤�ȼ�⴦����������Ϣ(��׼��Ӧ����ֵ�� ƫ������ֵ���Ƕ�)������ⳤ����������Ϊ0��

	std::vector<std::vector<int>> trueTargetPartAxis;	// ���ڴ������Ŀ�����������Ϣ
	std::vector<std::vector<int>> posProcessParam;	// ����λ�ü��������Ϣ

	cv::Mat matForColorTest;	// ������ȡ��ɫ�������ͼ�������ⲿ�ױ߽磩
	cv::Mat matForColorTestDstColorHSV;	// ���ڱ���������ɫĿ������ͼ
	cv::Mat grayForColorTest;	// matForColorTestת���ɵĻҶ�ͼ������ͼ���죬�����滻hsvForColorTest�ĵ�����ͨ��
	cv::Mat hsvForColorTest;	// matForColorTestת���ɵ�HSVͼ
	cv::Mat maskForColorTest;	// ���hsvForColorTest����ͨ����Χ������
	cv::Mat displayForColorTest;	// ������ʾ���ֲ��ָ�Ч����ͼ
	bool isOnColorMark = false;	// �Ƿ�����ɫ��ȡ�׶�
	bool isOnTargetColorAverage = false;	// �Ƿ��ڼ���Ŀ����ɫƽ��ֵ�׶�
	cv::Point colorMarkStartPoint, colorMarkEndPoint;	// ���ڱ����ȡ��ɫ���������
	void updateColorMark(std::string &curColor);
	void adjustMatChannel(cv::Mat& src, int chl);
	int curHMean = 0;
	int curSMean = 0;
	int curVMean = 0;

	std::vector<std::vector<int>> plugDirectMaskMsg;	// �����ͷ������mask��Ϣ

private:
	void updateStepOnPageCreateModel();

	void initNewModelSettingParam();	// �������model������֮�󣬳�ʼ����ʾ���

	void updateDisplayOnTableModelMsg(std::vector<std::vector<int>> &vect);
	void updateDisplayOnPageParam(std::vector<std::vector<int>> &vect, int curIdx);

	void updateHaveModel();	// ������ʾ���е�ģ��

	//// ����ÿ��block�����ͣ�������һ����ص��㷨��⣬����ʾ
	//void updateAlgorithmDisplay(int cateIdx, int detectIdx);

private slots:
	void onButtonSingleImage();
	void onButtonContinuousImage();
	void onButtonLoadModel();
	void onButtonCreateNewModel();
	void onButtonStartCircleROI();

	void onTableWidgetModelMsg(int cRow, int cCol);

	void onComboBoxAreaCate(int);
	void onBtnGroupHaveOrNot(int);
	void onBtnGroupColor(int);
	void onBtnGroupCate(int);
	void onBtnGroupDirection(int);
	void onBtnGroupPos(int);
	void onBtnGroupLength(int);

	void onComboBoxDetectionColorNormal(int);
	void onComboBoxStartID(int);
	void onLineEditDetectionPosNormal();
	void onLineEditDetectionPosOffset();
	void onLineEditDetectionLengthNormal();
	void onLineEditDetectionLengthOffset();

	void onButtonFinishCreateModel();

	void onLandMarkChangeFromProcessDisplay(int, int, int, int, int);

	void onLengthParamModify(int, int, int, double);

	void onButtonConfirmMsg();

	void onTrueTargetPartAxisModify(int, int, int, int, int, int, int);

	void onTwoTargetDistance(int, int, int);

	void onComboBoxHaveColor(int);
	void onButtonModifyColor();
	void onCheckBoxColorMark(int);

	void onHorizontalSliderHMin(int);
	void onHorizontalSliderHMax(int);
	void onHorizontalSliderSMin(int);
	void onHorizontalSliderSMax(int);
	void onHorizontalSliderVMin(int);
	void onHorizontalSliderVMax(int);

	void onLineEditHOffset();
	void onLineEditSOffset();
	void onLineEditVOffset();

protected:
	void mousePressEvent(QMouseEvent* ev);
	void mouseReleaseEvent(QMouseEvent* ev);
	void mouseMoveEvent(QMouseEvent *ev);
	bool eventFilter(QObject *, QEvent *);

signals:
	void signalLoadModel(QString);
};
