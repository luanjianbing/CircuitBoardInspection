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
		ON_GET_IMAGE = 0,			// 初始状态，只可以获取单张图像
		ON_GET_CONTINUOUS_IMAGE,	// 获取连续图像
		ON_LOAD_OR_CREATE_MODEL,	// 允许选择是加载已有模型还是新建模型信息
		ON_LOAD_MODEL,				// 加载已有模型
		ON_CREATE_NEW_MODEL,		// 新建模型
		ON_MAX_AREA_MARK,			// 最大范围标记
		//ON_COLOR_MARK,
		ON_PART_AREA_MARK,			// 局部标记
		ON_PART_AREA_MARK_SCALE,	// 局部放大情况下
		ON_PART_PARAM_SET,	
		ON_CREATE_NEW_MODEL_FINISH,	// 新建模型结束

		ON_SET_TABLE_PARAM,			// 设置表格的所有参数
		ON_SET_BLOCK_PARAM,			// 设置每个框的具体参数
	};

	STEP curState = ON_GET_IMAGE;
	QString newModelName = "";	// 新建模型名称

	bool isOnLandMark = false;	// 是否处于标记状态
	bool isOnPartROIDraw = false;	// 用于标记是否是局部ROI坐标还是整体最大范围的ROI坐标

	QTimer *timerForContinuousFrame = NULL;

	std::vector<std::vector<int>> landMark;	// 保存坐标信息
	cv::Mat matForDraw;	// 用于作图的图像
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	cv::Size frameSize;	// 获取图像大小
	float adaptRatio = 0.0;	// 记录实际缩放的比率
	int offsetY = 0;	// 由于是居中显示，所以实际在Y轴上有一个偏移量
	void landMarkSave();
	void landMarkRun(cv::Mat &mat);	// 送进去画框
	cv::Rect partRect = cv::Rect(0, 0, 0, 0);	// 用于放大显示
	float partScale = 1.5;	// 长宽比按照
	cv::Mat matForDrawPart;	// 局部放大图像
	int partStartX = 0, partStartY = 0, partEndX = 0, partEndY = 0, trueStartX = 0, trueStartY = 0;

	std::vector<std::vector<int>> vectParamForNewModel;
	// [1]区域编号 : (int)
	// [2]区域类型 : 扎带(0) / 胶布(1) / 插头(2)
	// [3]颜色检测 : 否(0) / 是(1)
	// [4]颜色规格值 :	1.扎带(花结束带[灰(0)、白(1)、粉(2)] / T18R[白(3)、黑(4)、黄(5)])	// 0~5	6种
	//					2.胶布(红(6)、白(7)、黑(8)、黄(9)、绿(10)、灰(11))		// 6~11	6种
	//					3.插头(黑(12)、白(13)、粉(14)、蓝(15)、灰(16)、绿(17)、黄(18))	// 12~18	7种
	// [5]长度限制检测(指胶布本身长度) : 否(0) / 是(1)
	// [6]长度规格值 : (int) 单位为mm > 0
	// [7]长度偏差值 : (int) 单位为mm > 0
	// [8]位置检测 : 否(0) / 是(1)
	// [9]起始ID :	从起始ID至当前ID的距离，(int)
	// [10]位置规格值 : (int) 单位为mm > 0
	// [11]位置偏差值 : (int) 单位为mm > 0
	// [12]有无检测 : 否(0) / 是(1)
	// [13]类型检测 : 否(0) / 是(1)
	// [14]方向检测 : 否(0) / 是(1)
	int curSetBlock = -1;	// 当前设置第几个block的参数
	QButtonGroup *btnGroupHaveOrNot;
	QButtonGroup *btnGroupColor;
	QButtonGroup *btnGroupCate;
	QButtonGroup *btnGroupDirection;
	QButtonGroup *btnGroupPos;
	QButtonGroup *btnGroupLength;

	std::vector<std::vector<std::string>> myModelsName;	// 读取已经创建的模型名
	std::unordered_map<std::string, HSVLimit> HSVColorForGetTrueTarget;	// 读取已有颜色信息

	//ImageProcess *m_ImageProcessTest;
	ProcessDisplay *m_ProcessDisplay;

	std::vector<std::vector<std::string>> lengthProcessParam;	// 保存长度检测处理后的像素信息(标准对应像素值， 偏差像素值，角度)（不检测长度两个参数为0）

	std::vector<std::vector<int>> trueTargetPartAxis;	// 用于存放真正目标物的坐标信息
	std::vector<std::vector<int>> posProcessParam;	// 保存位置检测像素信息

	cv::Mat matForColorTest;	// 用于提取颜色的总体大图（包含外部白边界）
	cv::Mat matForColorTestDstColorHSV;	// 用于保存具体的颜色目标区域图
	cv::Mat grayForColorTest;	// matForColorTest转化成的灰度图，将此图拉伸，用于替换hsvForColorTest的第三个通道
	cv::Mat hsvForColorTest;	// matForColorTest转化成的HSV图
	cv::Mat maskForColorTest;	// 针对hsvForColorTest各个通道范围的限制
	cv::Mat displayForColorTest;	// 用于显示到局部分割效果的图
	bool isOnColorMark = false;	// 是否处于颜色提取阶段
	bool isOnTargetColorAverage = false;	// 是否处于计算目标颜色平均值阶段
	cv::Point colorMarkStartPoint, colorMarkEndPoint;	// 用于标记提取颜色的最大区域
	void updateColorMark(std::string &curColor);
	void adjustMatChannel(cv::Mat& src, int chl);
	int curHMean = 0;
	int curSMean = 0;
	int curVMean = 0;

	std::vector<std::vector<int>> plugDirectMaskMsg;	// 保存插头方向检测mask信息

private:
	void updateStepOnPageCreateModel();

	void initNewModelSettingParam();	// 标记完新model的区域之后，初始化显示表格

	void updateDisplayOnTableModelMsg(std::vector<std::vector<int>> &vect);
	void updateDisplayOnPageParam(std::vector<std::vector<int>> &vect, int curIdx);

	void updateHaveModel();	// 更新显示已有的模型

	//// 根据每个block的类型，首先做一次相关的算法检测，并显示
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
