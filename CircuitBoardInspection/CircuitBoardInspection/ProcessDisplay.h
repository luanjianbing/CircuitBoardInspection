#pragma once

#include "common.h"

#include <QtWidgets/QWidget>
#include <QMessageBox>
#include "ui_ProcessDisplay.h"

#include "ImageProcess.h"




class ProcessDisplay : public QWidget
{
	Q_OBJECT

public:
	ProcessDisplay(QWidget *parent = Q_NULLPTR);
	~ProcessDisplay();

	void setBlockMsg(std::vector<std::vector<int>> &landMarkRun);
	void setMatMsg(cv::Mat norm);
	void setParamMsg(std::vector<std::vector<int>> &vectParamForNewModel);

	void updateDisplayByBlockIdx(int blockIdx);

	void updateAlgorithmDisplay(int cateIdx, int colorIdx, int detectIdx);

	void setHSVColorForGetTrueTarget(std::unordered_map<std::string, HSVLimit> &HSVColorForGetTrueTarget);
	void setCurModelName(std::string modelName);

private:
	Ui::ProcessResultDisplayClass ui;

	ImageProcess *m_ImageProcessTest;

	cv::Mat matTemplate;
	int matTemplateSizeX = 0;
	int matTemplateSizeY = 0;

	std::vector<std::vector<int>> block;

	bool isOnModify = false;	// 记录是否处在编辑坐标状态

	std::vector<std::vector<int>> param;

	// std::unordered_map<std::string, HSVLimit> HSVColorForGetTrueTarget;

signals:
	void signalLandMarkChange(int idx, int x1, int y1, int x2, int y2);
	void signalLengthParamModify(int, int, int, double);

	void signalTrueTargetPartAxis(int, int, int, int, int, int, int);

	void signalTwoTargetDistance(int, int, int);

private slots :
	void onButtonModify();
	void onButtonSure();
};