#pragma execution_character_set("utf-8")
#include "CircuitBoardInspection.h"

CircuitBoardInspection::CircuitBoardInspection(MyCamera *myCamera, MysqlSetting *mysql_conn, QWidget *parent)
    : myCamera(myCamera), mysql_conn(mysql_conn), QWidget(parent)
{
    ui.setupUi(this);
	setFixedSize(this->width(), this->height());
	// 加载模型名
	myModelsName = mysql_conn->tab2Vector("model", 7);
	//std::cout << "myModelsName: " << myModelsName.size() << std::endl;
	//if (myModelsName.size() > 0) {
	//	for (int i = 0; i < myModelsName.size(); ++i) {
	//		for (int j = 0; j < myModelsName[i].size(); ++j)
	//			std::cout << myModelsName[i][j] << " ";
	//		std::cout << std::endl;
	//	}
	//}
	// 加载颜色HSV信息
	mysql_conn->readColorHSVParam(HSVColorForGetTrueTarget);
	//for (auto iter = HSVColorForGetTrueTarget.begin(); iter != HSVColorForGetTrueTarget.end(); ++iter) {
	//	std::cout << iter->first << " " << iter->second.hMin << " " << iter->second.hMax << " " << iter->second.sMin
	//		<< " " << iter->second.sMax << " " << iter->second.vMin << " " << iter->second.vMax << std::endl;
	//}
	//mysql_conn->modifyColorHSVParam(HSVColorForGetTrueTarget);
	ui.comboBoxHaveColor->clear();
	for (auto iter = HSVColorForGetTrueTarget.begin(); iter != HSVColorForGetTrueTarget.end(); ++iter) {
		ui.comboBoxHaveColor->addItem(iter->first.c_str());
	}
	ui.comboBoxHaveColor->setCurrentIndex(-1);
	connect(ui.comboBoxHaveColor, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxHaveColor(int)));
	connect(ui.horizontalSliderHMin, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalSliderHMin(int)));
	connect(ui.horizontalSliderHMax, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalSliderHMax(int)));
	connect(ui.horizontalSliderSMin, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalSliderSMin(int)));
	connect(ui.horizontalSliderSMax, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalSliderSMax(int)));
	connect(ui.horizontalSliderVMin, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalSliderVMin(int)));
	connect(ui.horizontalSliderVMax, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalSliderVMax(int)));
	// buttonModifyColor
	connect(ui.buttonModifyColor, SIGNAL(released()), this, SLOT(onButtonModifyColor()));
	// checkBoxColorMark
	connect(ui.checkBoxColorMark, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxColorMark(int)));
	//ui.checkBoxColorMark->setDisabled(true);

	connect(ui.lineEditHOffset, SIGNAL(editingFinished()), this, SLOT(onLineEditHOffset()));
	connect(ui.lineEditSOffset, SIGNAL(editingFinished()), this, SLOT(onLineEditSOffset()));
	connect(ui.lineEditVOffset, SIGNAL(editingFinished()), this, SLOT(onLineEditVOffset()));

	// buttonSingleImage
	connect(ui.buttonSingleImage, SIGNAL(released()), this, SLOT(onButtonSingleImage()));
	// buttonContinuousImage
	connect(ui.buttonContinuousImage, SIGNAL(released()), this, SLOT(onButtonContinuousImage()));
	// buttonLoadModel
	connect(ui.buttonLoadModel, SIGNAL(released()), this, SLOT(onButtonLoadModel()));
	// buttonCreateNewModel
	connect(ui.buttonCreateNewModel, SIGNAL(released()), this, SLOT(onButtonCreateNewModel()));
	// buttonStartCircleROI
	connect(ui.buttonStartCircleROI, SIGNAL(released()), this, SLOT(onButtonStartCircleROI()));

	//m_ImageProcessTest = new ImageProcess();
	m_ProcessDisplay = new ProcessDisplay();
}

void CircuitBoardInspection::onCheckBoxColorMark(int) {
	x1 = 27;
	y1 = 27;
	x2 = 27;
	y2 = 27;
	if (ui.checkBoxColorMark->isChecked()) {
		ui.widgetHSVVal->setEnabled(true);
	}
	//else {
	//	ui.widgetHSVVal->setDisabled(true);
	//}
	if (isOnTargetColorAverage) {
		QMessageBox::information(NULL, "提示", "请先设置标记目标颜色并设置偏差！", QMessageBox::Yes);
		ui.checkBoxColorMark->setChecked(false);
	}
}

void CircuitBoardInspection::onButtonModifyColor() {
	// 保存颜色信息
	ui.labelDisplayColorThrd->clear();
	// 设置黑/白/灰
	for (auto iter = HSVColorForGetTrueTarget.begin(); iter != HSVColorForGetTrueTarget.end(); ++iter) {
		if (iter->first == "Black") {
			iter->second.hMin = 0;
			iter->second.hMax = 180;
			iter->second.sMin = 0;
			iter->second.sMax = 255;
		}
		else if (iter->first == "Gray" || iter->first == "White") {
			iter->second.hMin = 0;
			iter->second.hMax = 180;
		}
		else {
			iter->second.vMin = 0;
			iter->second.vMax = 255;
		}
	}

	mysql_conn->modifyColorHSVParam(HSVColorForGetTrueTarget);
	m_ProcessDisplay->setHSVColorForGetTrueTarget(HSVColorForGetTrueTarget);
	QMessageBox::information(NULL, "提示", "颜色信息更新入库！", QMessageBox::Yes);

	ui.toolBox->setCurrentIndex(1);	// 跳转到功能设置参数一页
	ui.groupBoxGetColor->setDisabled(true);

	m_ProcessDisplay->show();

	curState = ON_PART_AREA_MARK_SCALE;
	updateStepOnPageCreateModel();

	isOnColorMark = false;
}

void CircuitBoardInspection::onComboBoxHaveColor(int idx) {
	x1 = 27;
	y1 = 27;
	x2 = 27;
	y2 = 27;
	std::string curColor = ui.comboBoxHaveColor->currentText().toStdString();
	ui.horizontalSliderHMin->setValue(HSVColorForGetTrueTarget[curColor].hMin);
	ui.horizontalSliderHMax->setValue(HSVColorForGetTrueTarget[curColor].hMax);
	ui.horizontalSliderSMin->setValue(HSVColorForGetTrueTarget[curColor].sMin);
	ui.horizontalSliderSMax->setValue(HSVColorForGetTrueTarget[curColor].sMax);
	ui.horizontalSliderVMin->setValue(HSVColorForGetTrueTarget[curColor].vMin);
	ui.horizontalSliderVMax->setValue(HSVColorForGetTrueTarget[curColor].vMax);
	updateColorMark(curColor);
}

void CircuitBoardInspection::onLineEditHOffset() {
	std::string curColor = ui.comboBoxHaveColor->currentText().toStdString();
	if (curColor != "") {
		int curHOffset = ui.lineEditHOffset->text().toInt();
		int minHVal = (curHMean - curHOffset) > 0 ? (curHMean - curHOffset) : 0;
		int maxHVal = (curHMean + curHOffset) < 180 ? (curHMean + curHOffset) : 180;
		HSVColorForGetTrueTarget[curColor].hMin = minHVal;
		HSVColorForGetTrueTarget[curColor].hMax = maxHVal;
	}
}
void CircuitBoardInspection::onLineEditSOffset() {
	std::string curColor = ui.comboBoxHaveColor->currentText().toStdString();
	if (curColor != "") {
		int curSOffset = ui.lineEditSOffset->text().toInt();
		int minSVal = (curSMean - curSOffset) > 0 ? (curSMean - curSOffset) : 0;
		int maxSVal = (curSMean + curSOffset) < 255 ? (curSMean + curSOffset) : 255;
		HSVColorForGetTrueTarget[curColor].sMin = minSVal;
		HSVColorForGetTrueTarget[curColor].sMax = maxSVal;
	}
}
void CircuitBoardInspection::onLineEditVOffset() {
	std::string curColor = ui.comboBoxHaveColor->currentText().toStdString();
	if (curColor != "") {
		int curVOffset = ui.lineEditVOffset->text().toInt();
		int minVVal = (curVMean - curVOffset) > 0 ? (curVMean - curVOffset) : 0;
		int maxVVal = (curVMean + curVOffset) < 255 ? (curVMean + curVOffset) : 255;
		HSVColorForGetTrueTarget[curColor].vMin = minVVal;
		HSVColorForGetTrueTarget[curColor].vMax = maxVVal;
	}
}

void CircuitBoardInspection::onHorizontalSliderHMin(int val) {
	std::string curColor = ui.comboBoxHaveColor->currentText().toStdString();
	HSVColorForGetTrueTarget[curColor].hMin = val;
	ui.labelhMinVal->setText(QString::fromStdString(std::to_string(val)));
	updateColorMark(curColor);
}
void CircuitBoardInspection::onHorizontalSliderHMax(int val) {
	std::string curColor = ui.comboBoxHaveColor->currentText().toStdString();
	HSVColorForGetTrueTarget[curColor].hMax = val;
	ui.labelhMaxVal->setText(QString::fromStdString(std::to_string(val)));
	updateColorMark(curColor);
}
void CircuitBoardInspection::onHorizontalSliderSMin(int val) {
	std::string curColor = ui.comboBoxHaveColor->currentText().toStdString();
	HSVColorForGetTrueTarget[curColor].sMin = val;
	ui.labelsMinVal->setText(QString::fromStdString(std::to_string(val)));
	updateColorMark(curColor);
}
void CircuitBoardInspection::onHorizontalSliderSMax(int val) {
	std::string curColor = ui.comboBoxHaveColor->currentText().toStdString();
	HSVColorForGetTrueTarget[curColor].sMax = val;
	ui.labelsMaxVal->setText(QString::fromStdString(std::to_string(val)));
	updateColorMark(curColor);
}
void CircuitBoardInspection::onHorizontalSliderVMin(int val) {
	std::string curColor = ui.comboBoxHaveColor->currentText().toStdString();
	HSVColorForGetTrueTarget[curColor].vMin = val;
	ui.labelvMinVal->setText(QString::fromStdString(std::to_string(val)));
	updateColorMark(curColor);
}
void CircuitBoardInspection::onHorizontalSliderVMax(int val) {
	std::string curColor = ui.comboBoxHaveColor->currentText().toStdString();
	HSVColorForGetTrueTarget[curColor].vMax = val;
	ui.labelvMaxVal->setText(QString::fromStdString(std::to_string(val)));
	updateColorMark(curColor);
}

void CircuitBoardInspection::updateColorMark(std::string &curColor) {
	
	if (matForColorTest.empty())
		return;
	cv::cvtColor(matForColorTest, grayForColorTest, cv::COLOR_BGR2GRAY);
	adjustMatChannel(grayForColorTest, 0);
	
	cv::cvtColor(matForColorTest, hsvForColorTest, cv::COLOR_BGR2HSV);
	int nRows = hsvForColorTest.rows;
	int nCols = hsvForColorTest.cols;
	uchar *pDataMat;
	for (int j = 0; j < nRows; j++)
	{
		pDataMat = grayForColorTest.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			hsvForColorTest.at<cv::Vec3b>(j, i)[2] = pDataMat[i];
		}
	}
	if (curColor == "Black") {
		cv::inRange(hsvForColorTest,
			cv::Scalar(0, 0, HSVColorForGetTrueTarget[curColor].vMin),
			cv::Scalar(180, 255, HSVColorForGetTrueTarget[curColor].vMax), maskForColorTest);
	}
	else if (curColor == "White" || curColor == "Gray") {
		cv::inRange(hsvForColorTest,
			cv::Scalar(0, HSVColorForGetTrueTarget[curColor].sMin, HSVColorForGetTrueTarget[curColor].vMin),
			cv::Scalar(180, HSVColorForGetTrueTarget[curColor].sMax, HSVColorForGetTrueTarget[curColor].vMax), maskForColorTest);
	}
	else {
		cv::inRange(hsvForColorTest,
			cv::Scalar(HSVColorForGetTrueTarget[curColor].hMin, HSVColorForGetTrueTarget[curColor].sMin, 0),
			cv::Scalar(HSVColorForGetTrueTarget[curColor].hMax, HSVColorForGetTrueTarget[curColor].sMax, 255), maskForColorTest);
	}
	cv::Mat structureElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));      //创建结构元
	cv::dilate(maskForColorTest, maskForColorTest, structureElement);
	cv::cvtColor(maskForColorTest, maskForColorTest, cv::COLOR_GRAY2BGR);
	//if (curColor == "Black") 
		displayForColorTest = maskForColorTest;
	//else
	//	displayForColorTest = matForColorTest & maskForColorTest;
	//cv::cvtColor(displayForColorTest, displayForColorTest, cv::COLOR_BGR2RGB);
	QImage imgForDrawQImage = QImage(displayForColorTest.data, displayForColorTest.cols, displayForColorTest.rows, displayForColorTest.step, QImage::Format_RGB888);
	ui.labelDisplayColorThrd->setPixmap(QPixmap::fromImage(imgForDrawQImage).scaled(ui.labelDisplayColorThrd->width(), ui.labelDisplayColorThrd->height(), Qt::KeepAspectRatio));
}

CircuitBoardInspection::~CircuitBoardInspection() {
	//myCamera->cameraClose();
	delete btnGroupHaveOrNot;
	delete btnGroupColor;
	delete btnGroupCate;
	delete btnGroupDirection;
	delete btnGroupPos;
	delete btnGroupLength;

	//delete m_ImageProcessTest;
	delete m_ProcessDisplay;
}

// 获取单张图像槽函数
void CircuitBoardInspection::onButtonSingleImage() {
	std::cout << "Get One Frame" << std::endl;
	// 获取单张图像
	/**********************************************************/
	cv::Mat singleMat(myCamera->getCameraResolution(), CV_8UC3);
	myCamera->getOneFrame(singleMat);
	//singleMat = cv::imread("./test/norm.jpg");	// 测试标准图像
	/**********************************************************/
	//cv::imshow("singleMat", singleMat);
	// 显示
	// 对于显示而言，只有BGR才是正常的显色，所以需要转换
	cv::cvtColor(singleMat, singleMat, cv::COLOR_BGR2RGB);
	QImage singleQImage(singleMat.data, singleMat.cols, singleMat.rows, QImage::Format_RGB888);
	ui.labelDisplay->setPixmap(QPixmap::fromImage(singleQImage).
		scaled(ui.labelDisplay->width(), ui.labelDisplay->height(), Qt::KeepAspectRatio));

	// 更新步骤显示
	if (curState != ON_GET_CONTINUOUS_IMAGE) {	// 连续获取图像不准进行下一步
		curState = ON_LOAD_OR_CREATE_MODEL;
		updateStepOnPageCreateModel();
	}
}

// 获取连续图像槽函数
void CircuitBoardInspection::onButtonContinuousImage() {
	// 启动定时器，不停获取单张测试图像
	if (timerForContinuousFrame == NULL) {
		timerForContinuousFrame = new QTimer();
		timerForContinuousFrame->setInterval(20);	// interval for timer
		connect(timerForContinuousFrame, SIGNAL(timeout()), this, SLOT(onButtonSingleImage()));
	}

	if (timerForContinuousFrame->isActive() == false) {	// 启动连续获取图像定时器
		timerForContinuousFrame->start();
		ui.buttonContinuousImage->setText("停止获取图像");

		curState = ON_GET_CONTINUOUS_IMAGE;
	}
	else {	// 停止定时器
		timerForContinuousFrame->stop();
		disconnect(timerForContinuousFrame, SIGNAL(timeout()), this, SLOT(onButtonSingleImage()));
		delete timerForContinuousFrame;
		timerForContinuousFrame = NULL;
		ui.buttonContinuousImage->setText("获取连续测试图像");

		curState = ON_GET_IMAGE;
	}
	updateStepOnPageCreateModel();
}

// 加载模型
void CircuitBoardInspection::onButtonLoadModel() {
	std::cout << "buttonLoadModel clicked" << std::endl;

	// 根据这个名称去数据库中取出block和param，传递给主界面
	emit signalLoadModel(ui.comboBoxHaveModel->currentText());

	curState = ON_LOAD_MODEL;
	updateStepOnPageCreateModel();
}

// 新建模型
void CircuitBoardInspection::onButtonCreateNewModel() {
	std::cout << "buttonCreateNewModel clicked" << std::endl;

	if (ui.lineEditNewModelName->text().isEmpty()) {	// (TODO)这里可以用来规定格式
		QMessageBox::information(NULL, "提示", "<新建模型名称>错误！", QMessageBox::Yes);
	}
	else {	// 没问题，进入创建模型的过程
		// 获取新建模型名称
		newModelName = ui.lineEditNewModelName->text();
		std::cout << "create new model [" << newModelName.toStdString() << "]" << std::endl;

		// 获取一张图像(用于新建)
		/**********************************************/
		myCamera->getOneFrame(matForDraw);
		//matForDraw = cv::imread("./test/norm.jpg");	// 测试标准图像
		frameSize = myCamera->getCameraResolution();
		/**********************************************/
		// 计算图像实际缩放的比率
		float ratioX = static_cast<float>(frameSize.width) / ui.labelDisplay->width();
		float ratioY = static_cast<float>(frameSize.height) / ui.labelDisplay->height();
		adaptRatio = MAX(ratioX, ratioY);	// 真正缩放的比率取小的
		offsetY = static_cast<int>((ui.labelDisplay->height() - (frameSize.height / adaptRatio)) / 2);

		// 创建文件夹用于保存当前设置block的mask
		std::string path = ".\\mask\\" + newModelName.toStdString();
		if (_access(path.c_str(), 0) == -1) {
			std::string cmd = "mkdir " + path;
			system(cmd.c_str());
		}
		else {
			std::cout << path << " has Exist." << std::endl;
		}
		// 创建文件夹用于保存当前设置block的多模板
		std::string tempPath = ".\\multiTemplate\\" + newModelName.toStdString();
		if (_access(tempPath.c_str(), 0) == -1) {
			std::string cmd = "mkdir " + tempPath;
			system(cmd.c_str());
		}
		else {
			std::cout << tempPath << " has Exist." << std::endl;
		}

		m_ProcessDisplay->setCurModelName(newModelName.toStdString());	// 

		curState = ON_CREATE_NEW_MODEL;
		updateStepOnPageCreateModel();

		
	}
}

// 新建模型标记区域
void CircuitBoardInspection::onButtonStartCircleROI() {
	// 开始作图
	// (TODO)左键绘图，右键移动
	if (curState == ON_CREATE_NEW_MODEL) {	// 首先规定最大范围
		QMessageBox::information(NULL, "提示", "请先确定最大范围后再标记局部区域！", QMessageBox::Yes);

		isOnLandMark = true;	// 开始标记
		ui.labelDisplay->installEventFilter(this);

		ui.buttonStartCircleROI->setText("结束标记");

		curState = ON_MAX_AREA_MARK;	// 标记整体区域
		updateStepOnPageCreateModel();
	}
	//else if (curState == ON_PART_AREA_MARK_SCALE){	// 真正记录小区域
	//	QMessageBox::information(NULL, "提示", "区域标定完毕，请设置各区域检测参数！", QMessageBox::Yes);

	//	ui.buttonStartCircleROI->setText("开始");

	//	// 更新状态
	//	curState = ON_SET_TABLE_PARAM;
	//	updateStepOnPageCreateModel();
	//	//std::cout << curState << std::endl;
	//}
}

void CircuitBoardInspection::updateStepOnPageCreateModel() {
	switch (curState)
	{
		case CircuitBoardInspection::ON_GET_IMAGE: {
			break;
		}
		case CircuitBoardInspection::ON_GET_CONTINUOUS_IMAGE: {
			ui.groupBoxLoadOrCreateModel->setEnabled(false);
			break;
		}
		case CircuitBoardInspection::ON_LOAD_OR_CREATE_MODEL: {	// 允许选择是加载已有模型还是新建模型信息
			updateHaveModel();
			ui.groupBoxLoadOrCreateModel->setEnabled(true);
			break;
		}
		case CircuitBoardInspection::ON_LOAD_MODEL: {	// 加载已有模型
			QMessageBox::information(NULL, "提示", "模型加载成功！", QMessageBox::Yes);
			// (TODO)目前只做个简单的功能，直接关闭
			//this->close();
			// 暂时失能，不允许修改表格
			ui.tableWidgetModelMsg->setEnabled(true);
			// 初始化参数设置，并显示在表格中
			initNewModelSettingParam();
			break;
		}
		case CircuitBoardInspection::ON_CREATE_NEW_MODEL: {	// 新建模型
			// 创建mask文件夹


			ui.groupBoxCircleROI->setEnabled(true);
			ui.buttonFinishCreateModel->setEnabled(true);

			ui.groupBoxLoadImage->setEnabled(false);
			ui.groupBoxLoadOrCreateModel->setEnabled(false);

			// 下面是标记每一个框的参数部分，全部初始化失能
			ui.groupBoxDetectHaveOrNot->setDisabled(true);
			ui.groupBoxDetectionColor->setDisabled(true);
			ui.groupBoxDetectionCate->setDisabled(true);
			ui.groupBoxDetectionDirection->setDisabled(true);
			ui.groupBoxDetectionLength->setDisabled(true);
			ui.groupBoxDetectionPos->setDisabled(true);
			break;
		}
		case CircuitBoardInspection::ON_MAX_AREA_MARK: {	// 标记最大范围
			// 具体参数设置
			// 区域类型
			connect(ui.comboBoxAreaCate, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxAreaCate(int)));
			// 有无
			btnGroupHaveOrNot = new QButtonGroup();
			btnGroupHaveOrNot->addButton(ui.radioButtonDetectionHaveOrNotOff, 0);	// set有无检测
			btnGroupHaveOrNot->addButton(ui.radioButtonDetectionHaveOrNotOn, 1);	// unset有无检测
			connect(btnGroupHaveOrNot, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupHaveOrNot(int)));
			// 颜色
			btnGroupColor = new QButtonGroup();
			btnGroupColor->addButton(ui.radioButtonDetectionColorOff, 0);	// set颜色检测
			btnGroupColor->addButton(ui.radioButtonDetectionColorOn, 1);	// unset颜色检测
			connect(btnGroupColor, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupColor(int)));
			connect(ui.comboBoxDetectionColorNormal, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxDetectionColorNormal(int)));
			// 类型
			btnGroupCate = new QButtonGroup();
			btnGroupCate->addButton(ui.radioButtonDetectionCateOff, 0);	// set类型检测
			btnGroupCate->addButton(ui.radioButtonDetectionCateOn, 1);	// unset类型检测
			connect(btnGroupCate, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupCate(int)));
			// 方向
			btnGroupDirection = new QButtonGroup();
			btnGroupDirection->addButton(ui.radioButtonDetectionDirectionOff, 0);	// set方向检测
			btnGroupDirection->addButton(ui.radioButtonDetectionDirectionOn, 1);	// unset方向检测
			connect(btnGroupDirection, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupDirection(int)));
			// 位置
			btnGroupPos = new QButtonGroup();
			btnGroupPos->addButton(ui.radioButtonDetectionPosOff, 0);	// set位置检测
			btnGroupPos->addButton(ui.radioButtonDetectionPosOn, 1);	// unset位置检测
			connect(btnGroupPos, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupPos(int)));
			connect(ui.comboBoxStartID, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxStartID(int)));
			connect(ui.lineEditDetectionPosNormal, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionPosNormal()));
			connect(ui.lineEditDetectionPosOffset, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionPosOffset()));
			// 长度
			btnGroupLength = new QButtonGroup();
			btnGroupLength->addButton(ui.radioButtonDetectionLengthOff, 0);	// set长度检测
			btnGroupLength->addButton(ui.radioButtonDetectionLengthOn, 1);	// unset长度检测
			connect(btnGroupLength, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupLength(int)));
			connect(ui.lineEditDetectionLengthNormal, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionLengthNormal()));
			connect(ui.lineEditDetectionLengthOffset, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionLengthOffset()));

			
			// 响应length修改
			connect(m_ProcessDisplay, SIGNAL(signalLengthParamModify(int, int, int, double)), this, SLOT(onLengthParamModify(int, int, int, double)));
			// 响应block修改
			connect(m_ProcessDisplay, SIGNAL(signalLandMarkChange(int, int, int, int, int)), this, SLOT(onLandMarkChangeFromProcessDisplay(int, int, int, int, int)));
			// 响应细化target选取
			connect(m_ProcessDisplay, SIGNAL(signalTrueTargetPartAxis(int, int, int, int, int, int, int)), this, SLOT(onTrueTargetPartAxisModify(int, int, int, int, int, int, int)));
			// 响应pos像素信息添加
			connect(m_ProcessDisplay, SIGNAL(signalTwoTargetDistance(int, int, int)), this, SLOT(onTwoTargetDistance(int, int, int)));
			break;
		}

		//case CircuitBoardInspection::ON_COLOR_MARK: {
		//	// ui.groupBoxGetColor->setEnabled(true);
		//	break;
		//}
		case CircuitBoardInspection::ON_PART_AREA_MARK: {	// 标记局部

			// 之后的图像都以第一次标记最大范围内为初始图像
			matForDraw = matForDraw(cv::Range(landMark[0][1], landMark[0][3]),
				cv::Range(landMark[0][0], landMark[0][2]));
			frameSize = cv::Size(landMark[0][2] - landMark[0][0], landMark[0][3] - landMark[0][1]);
			// 计算图像实际缩放的比率
			float ratioX = static_cast<float>(frameSize.width) / ui.labelDisplay->width();
			float ratioY = static_cast<float>(frameSize.height) / ui.labelDisplay->height();
			adaptRatio = MAX(ratioX, ratioY);	// 真正缩放的比率取小的
			offsetY = static_cast<int>((ui.labelDisplay->height() - (frameSize.height / adaptRatio)) / 2);

			// 清零主要是为了去掉显示框
			x1 = 27;
			y1 = 27;
			x2 = 27;
			y2 = 27;

			// 传递给后面用于局部显示
			m_ProcessDisplay->setMatMsg(matForDraw);
			// ui.buttonConfirmMsg->setEnabled(true);
			ui.comboBoxAreaCate->setCurrentIndex(-1);

			ui.groupBoxDetectHaveOrNot->setDisabled(true);
			ui.groupBoxDetectionColor->setDisabled(true);
			ui.groupBoxDetectionCate->setDisabled(true);
			ui.groupBoxDetectionDirection->setDisabled(true);
			ui.groupBoxDetectionLength->setDisabled(true);
			ui.groupBoxDetectionPos->setDisabled(true);

			connect(ui.buttonConfirmMsg, SIGNAL(released()), this, SLOT(onButtonConfirmMsg()));
			connect(ui.buttonFinishCreateModel, SIGNAL(released()), this, SLOT(onButtonFinishCreateModel()));
			break;
		}
		case CircuitBoardInspection::ON_PART_AREA_MARK_SCALE: {
			ui.buttonConfirmMsg->setDisabled(true);
			break;
		}
		case CircuitBoardInspection::ON_PART_PARAM_SET: {
			ui.buttonConfirmMsg->setEnabled(true);
			
			
			break;
		}
		case CircuitBoardInspection::ON_CREATE_NEW_MODEL_FINISH: {	// 模型新建结束
			ui.toolBox->setCurrentIndex(0);	// 跳转到功能设置参数一页

			ui.groupBoxDetectHaveOrNot->setEnabled(false);
			ui.groupBoxDetectionColor->setEnabled(false);
			ui.groupBoxDetectionCate->setEnabled(false);
			ui.groupBoxDetectionDirection->setEnabled(false);
			ui.groupBoxDetectionLength->setEnabled(false);

			ui.groupBoxDetectionPos->setEnabled(false);

			ui.tableWidgetModelMsg->clear();
			ui.tableWidgetModelMsg->setEnabled(false);

			ui.groupBoxLoadOrCreateModel->setEnabled(true);
			ui.buttonFinishCreateModel->setEnabled(false);
			// 读取更新已有模型
			updateHaveModel();
			// 关闭画框
			isOnLandMark = false;
			ui.groupBoxCircleROI->setEnabled(false);
			//m_ProcessDisplay->close();
			//delete m_ProcessDisplay;
			QMessageBox::information(NULL, "提示", "创建新模型完毕!", QMessageBox::Yes);

			ui.buttonStartCircleROI->setText("开始");
			break;
		}
		case CircuitBoardInspection::ON_SET_TABLE_PARAM: {	// 区域标记完毕，设置每个区域的功能
			

			//ui.toolBox->setCurrentIndex(1);	// 跳转到功能设置参数一页
			//ui.buttonModifyMsg->setEnabled(true);
			//ui.buttonDeleteMsg->setEnabled(true);
			//ui.buttonDeleteAllMsg->setEnabled(true);
			//ui.tableWidgetModelMsg->setEnabled(true);

			// 初始化参数设置，并显示在表格中
			//initNewModelSettingParam();

			//m_ProcessDisplay->setBlockMsg(landMark);
			//m_ProcessDisplay->setMatMsg(matForDraw);
			//m_ProcessDisplay->setParamMsg(vectParamForNewModel);
			//m_ProcessDisplay->show();
			//// 响应block修改
			//connect(m_ProcessDisplay, SIGNAL(signalLandMarkChange(int, int, int, int, int)), this, SLOT(onLandMarkChangeFromProcessDisplay(int, int, int, int, int)));
			//// 响应length修改
			//connect(m_ProcessDisplay, SIGNAL(signalLengthParamModify(int, int, int)), this, SLOT(onLengthParamModify(int, int, int)));

			//connect(ui.buttonFinishCreateModel, SIGNAL(released()), this, SLOT(onButtonFinishCreateModel()));

			//// 用于存放长度检测的数据
			//lengthProcessParam.resize(landMark.size());
			//for (int i = 0; i < lengthProcessParam.size(); ++i) {
			//	lengthProcessParam[i].resize(2, 0);
			//}

			break;
		}
		case CircuitBoardInspection::ON_SET_BLOCK_PARAM: {	// 具体设置每个框的功能
			//ui.groupBoxDetectHaveOrNot->setEnabled(true);
			//ui.groupBoxDetectionColor->setEnabled(true);
			//ui.groupBoxDetectionCate->setEnabled(true);
			//ui.groupBoxDetectionDirection->setEnabled(true);
			//ui.groupBoxDetectionLength->setEnabled(true);

			//ui.groupBoxDetectionPos->setEnabled(true);

			// 表格显示更新
			//updateDisplayOnTableModelMsg(vectParamForNewModel);	// 根据vect初始化显示表格
			// 上方显示更新
			//updateDisplayOnPageParam(vectParamForNewModel);
			break;
		}
		
		default: {
			break;
		}
	}
}

void CircuitBoardInspection::onButtonConfirmMsg() {
	// 每画一个block，失能功能选项，直到选择该区域类型
	//ui.comboBoxAreaCate->setCurrentIndex(-1);
	ui.groupBoxDetectHaveOrNot->setDisabled(true);
	ui.groupBoxDetectionColor->setDisabled(true);
	ui.groupBoxDetectionCate->setDisabled(true);
	ui.groupBoxDetectionDirection->setDisabled(true);
	ui.groupBoxDetectionLength->setDisabled(true);
	ui.groupBoxDetectionPos->setDisabled(true);

	curState = ON_PART_AREA_MARK_SCALE;	// 模型结束
	updateStepOnPageCreateModel();
}

void CircuitBoardInspection::updateHaveModel() {	// 更新显示已有的模型
	myModelsName.clear();
	myModelsName = mysql_conn->tab2Vector("model", 7);

	ui.comboBoxHaveModel->clear();
	for (int i = 0; i < myModelsName.size(); ++i) {
		ui.comboBoxHaveModel->addItem(QString::fromStdString(myModelsName[i][1]));
	}
}

// 建模结束保存表格
void CircuitBoardInspection::onButtonFinishCreateModel() {
	bool isNewModel = true;
	for (int i = 0; i < myModelsName.size(); ++i) {
		if (myModelsName[i][1] == newModelName.toStdString())
			isNewModel = false;
	}

	if (isNewModel) {
		std::string newParamTabelName = newModelName.toStdString().append("_param_table");
		std::string newBlockTabelName = newModelName.toStdString().append("_block_table");
		std::string newLengthPixelParamTabelName = newModelName.toStdString().append("_length_pixel_table");
		std::string newTrueTargetBlockTabelName = newModelName.toStdString().append("_true_target_block_table");
		std::string newPosPixelParamTabelName = newModelName.toStdString().append("_pos_pixel_table");
		std::string newPlugDirectMaskMsgTableName = newModelName.toStdString().append("_plug_direction_table");

		std::vector<std::string> _newModel(7, "");
		_newModel[0] = std::to_string(myModelsName.size() + 1);
		_newModel[1] = newModelName.toStdString();
		_newModel[2] = newParamTabelName;
		_newModel[3] = newBlockTabelName;
		_newModel[4] = newLengthPixelParamTabelName;
		_newModel[5] = newTrueTargetBlockTabelName;
		_newModel[6] = newPosPixelParamTabelName;
		myModelsName.push_back(_newModel);
		// 插入新表
		mysql_conn->detAndNewData("model", myModelsName);

		// 创建参数新表
		mysql_conn->createNewTabelPARAM(newParamTabelName.c_str());	
		mysql_conn->detAndNewData(newParamTabelName.c_str(), vectParamForNewModel);
		// 创建新block表
		mysql_conn->createNewTabelBLOCK(newBlockTabelName.c_str());
		mysql_conn->detAndNewDataWithID(newBlockTabelName.c_str(), landMark);
		// 创建新LengthParam表
		mysql_conn->createNewTabelLengthPixelParam(newLengthPixelParamTabelName.c_str());
		mysql_conn->detAndNewDataWithID(newLengthPixelParamTabelName.c_str(), lengthProcessParam);
		// 创建新TrueTargetAxis表
		mysql_conn->createNewTabelTrueTargetBlock(newTrueTargetBlockTabelName.c_str());
		mysql_conn->detAndNewDataWithID(newTrueTargetBlockTabelName.c_str(), trueTargetPartAxis);
		// 创建新的pospixel表
		mysql_conn->createNewTabelPosPixelParam(newPosPixelParamTabelName.c_str());
		mysql_conn->detAndNewDataWithID(newPosPixelParamTabelName.c_str(), posProcessParam);
		// 创建新的PlugDirectMaskMsg表
		mysql_conn->createNewTabelPlugDirectMaskMsg(newPlugDirectMaskMsgTableName.c_str());
		mysql_conn->detAndNewDataWithID(newPlugDirectMaskMsgTableName.c_str(), plugDirectMaskMsg);

		curState = ON_CREATE_NEW_MODEL_FINISH;	// 模型结束
		updateStepOnPageCreateModel();
	}
	else {
		QMessageBox::warning(NULL, "警告", "模型名已经存在！", QMessageBox::Yes);
	}
}

void CircuitBoardInspection::mousePressEvent(QMouseEvent* ev) {
	if (curState > ON_CREATE_NEW_MODEL && curState < ON_PART_PARAM_SET) {
		if (ev->buttons() & Qt::LeftButton) {
			std::cout << "Button Left Clicked-[";
			x1 = ev->pos().x() < 27 ? 27 : ev->pos().x();
			y1 = ev->pos().y() < 27 ? 27 : ev->pos().y();
			std::cout << x1 << "," << y1 << "]" << std::endl;
			x2 = x1;
			y2 = y1;
			QCursor cursor;
			cursor.setShape(Qt::ClosedHandCursor);
			QApplication::setOverrideCursor(cursor);


		}
		else if (ev->buttons() & Qt::RightButton) {
			std::cout << "Button Right Clicked." << std::endl;
			// 放大
			if (curState == ON_PART_AREA_MARK) {	// 点击一下放大局部
				// 更新选取的局部放大图像，得到放大显示的矩形框
				partRect.height = static_cast<int>(static_cast<float>(frameSize.height) / partScale);
				partRect.width = partRect.height;
				
				float ratioX = static_cast<float>(partRect.width) / ui.labelDisplay->width();
				float ratioY = static_cast<float>(partRect.height) / ui.labelDisplay->height();
				adaptRatio = MAX(ratioX, ratioY);	// 真正缩放的比率取小的
				offsetY = static_cast<int>((ui.labelDisplay->height() - (partRect.height / adaptRatio)) / 2);

				matForDrawPart = matForDraw(cv::Range(trueStartY, partRect.height), cv::Range(trueStartX, partRect.width));

				
			}
			else if (curState == ON_PART_AREA_MARK_SCALE) {	// 移动
				x1 = 27;
				y1 = 27;
				x2 = 27;
				y2 = 27;
				partStartX = ev->pos().x();
				partStartY = ev->pos().y();
			}
			//else if (curState == ON_PART_AREA_MARK_SCALE) {	// 清零去显示
			//	
			//}
		}
	}
}

void CircuitBoardInspection::mouseReleaseEvent(QMouseEvent* ev) {
	if (curState > ON_CREATE_NEW_MODEL && curState < ON_PART_PARAM_SET) {
		if (ev->button() == Qt::LeftButton) {
			std::cout << "Button Left Released-[";
			x2 = ev->pos().x() > (27 + ui.labelDisplay->width()) ? (27 + ui.labelDisplay->width()) : ev->pos().x();
			y2 = ev->pos().y() > (27 + ui.labelDisplay->height()) ? (27 + ui.labelDisplay->height()) : ev->pos().y();
			std::cout << x2 << "," << y2 << "]" << std::endl;
			// 如果是从右下往左上画框的话，交换两点的坐标
			if (x2 < x1 && y2 < y1) {
				std::cout << "Swap StartPoint and EndPoint." << std::endl;
				std::swap(x1, x2);
				std::swap(y1, y2);
			}
			update();
			QApplication::restoreOverrideCursor();

			// for save cordination
			if (x1 != x2)
			{
				//std::cout << curState << " " << ON_SET_TABLE_PARAM  << std::endl;
				//if (ui.checkBoxColorMark->isChecked()) {
				//	if (ui.comboBoxHaveColor->currentIndex() == -1) {
				//		QMessageBox::information(NULL, "提示", "未选择目标颜色！", QMessageBox::Yes);
				//	}
				//	else {

				//	}
				//	return;
				//}

				if (!isOnColorMark) {
					landMarkSave();
					
					if (curState == ON_MAX_AREA_MARK) {	// 除了第一次是标记一个大区域，接下来都是标记小区域
						//ui.toolBox->setCurrentIndex(1);	// 跳转到功能设置参数一页
						//
						//m_ProcessDisplay->show();

						ui.groupBoxGetColor->setEnabled(true);

						curState = ON_PART_AREA_MARK;
						updateStepOnPageCreateModel();

						isOnColorMark = true;
						// 添加labelDisplayColorThrd进入事件，允许在上面作图
						ui.labelDisplayColorThrd->installEventFilter(this);
						// ui.labelDisplay->removeEventFilter(this);
					}
					else {
						curState = ON_PART_PARAM_SET;	// 设置局部参数
						updateStepOnPageCreateModel();
					}
					std::cout << "landMarkSave: " << landMark.size() << " vectParamForNewModel: " << vectParamForNewModel.size() << std::endl;
				}
				else {
					if (ui.comboBoxHaveColor->currentText() == "") {
						QMessageBox::warning(NULL, "警告", "请先选定目标颜色！", QMessageBox::Yes);
						return;
					}
					else {
						int startX = static_cast<int>((x1 - 27) * adaptRatio);
						int startY = static_cast<int>((y1 - 27 - offsetY) * adaptRatio);
						int endX = static_cast<int>((x2 - 27) * adaptRatio);
						int endY = static_cast<int>((y2 - 27 - offsetY) * adaptRatio);
						if (!ui.checkBoxColorMark->isChecked()) {	// 没有选中，处于标记阶段
							
							if (!isOnTargetColorAverage) {	// 说明正处于标记外部范围的阶段
								isOnTargetColorAverage = true;	// 下一个阶段
								colorMarkStartPoint = cv::Point(startX, startY);	// 记录外部范围
								colorMarkEndPoint = cv::Point(endX, endY);

								matForColorTest = matForDrawPart(cv::Range(startY, endY), cv::Range(startX, endX));
								cv::Mat matForColorTestDisplay;
								cv::cvtColor(matForColorTest, matForColorTestDisplay, cv::COLOR_BGR2RGB);
								QImage imgForDrawQImage = QImage(matForColorTestDisplay.data, matForColorTestDisplay.cols, matForColorTestDisplay.rows, matForColorTestDisplay.step, QImage::Format_RGB888);
								ui.labelDisplayColorThrd->setPixmap(QPixmap::fromImage(imgForDrawQImage).scaled(ui.labelDisplayColorThrd->width(), ui.labelDisplayColorThrd->height(), Qt::KeepAspectRatio));
								// 拉伸变换
								cv::cvtColor(matForColorTest, grayForColorTest, cv::COLOR_BGR2GRAY);
								adjustMatChannel(grayForColorTest, 0);
								cv::cvtColor(matForColorTest, hsvForColorTest, cv::COLOR_BGR2HSV);
								int nRows = hsvForColorTest.rows;
								int nCols = hsvForColorTest.cols;
								uchar *pDataMat;
								for (int j = 0; j < nRows; j++)
								{
									pDataMat = grayForColorTest.ptr<uchar>(j);
									for (int i = 0; i < nCols; i++) {
										hsvForColorTest.at<cv::Vec3b>(j, i)[2] = pDataMat[i];
									}
								}
							}
							else {	// 处于计算目标颜色具体平均值的阶段
								isOnTargetColorAverage = false;

								int relaStartX = startX - colorMarkStartPoint.x;
								int relaStartY = startY - colorMarkStartPoint.y;
								int relaEndX = endX - colorMarkStartPoint.x;
								int relaEndY = endY - colorMarkStartPoint.y;

								cv::Mat matForColorTestTargetColor = matForColorTest(cv::Range(relaStartY, relaEndY), cv::Range(relaStartX, relaEndX));
								cv::Mat matForColorTestDisplay;
								cv::cvtColor(matForColorTestTargetColor, matForColorTestDisplay, cv::COLOR_BGR2RGB);
								QImage imgForDrawQImage = QImage(matForColorTestDisplay.data, matForColorTestDisplay.cols, matForColorTestDisplay.rows, matForColorTestDisplay.step, QImage::Format_RGB888);
								ui.labelDisplayColorThrd->setPixmap(QPixmap::fromImage(imgForDrawQImage).scaled(ui.labelDisplayColorThrd->width(), ui.labelDisplayColorThrd->height(), Qt::KeepAspectRatio));

								matForColorTestDstColorHSV = hsvForColorTest(cv::Range(relaStartY, relaEndY), cv::Range(relaStartX, relaEndX));
								cv::Mat means, stddev;
								cv::meanStdDev(matForColorTestDstColorHSV, means, stddev);
								curHMean = static_cast<int>(means.at<double>(0));
								curSMean = static_cast<int>(means.at<double>(1));
								curVMean = static_cast<int>(means.at<double>(2));
								ui.labelHAverage->setText(QString::fromStdString(std::to_string(curHMean)));
								ui.labelSAverage->setText(QString::fromStdString(std::to_string(curSMean)));
								ui.labelVAverage->setText(QString::fromStdString(std::to_string(curVMean)));

								//matForColorTestDstColor = matForDrawPart(cv::Range(startY, endY), cv::Range(startX, endX));
								//cv::Mat matForColorTestDstColorDisplay;
								//cv::cvtColor(matForColorTest, matForColorTestDstColorDisplay, cv::COLOR_BGR2RGB);
								//QImage imgForDrawQImage = QImage(matForColorTestDstColorDisplay.data, matForColorTestDstColorDisplay.cols, matForColorTestDstColorDisplay.rows, matForColorTestDstColorDisplay.step, QImage::Format_RGB888);
								//ui.labelDisplayColorThrd->setPixmap(QPixmap::fromImage(imgForDrawQImage).scaled(ui.labelDisplayColorThrd->width(), ui.labelDisplayColorThrd->height(), Qt::KeepAspectRatio));
							}
						}
						else {	// 处于测试阶段
							if (isOnTargetColorAverage) isOnTargetColorAverage = false;	// 保证下一次一定是从标记最大的区域开始

							matForColorTest = matForDrawPart(cv::Range(startY, endY), cv::Range(startX, endX));

							std::string curColor = ui.comboBoxHaveColor->currentText().toStdString();
							updateColorMark(curColor);
						}
						//if (!ui.checkBoxColorMark->isChecked()) {
						//	cv::cvtColor(matForColorTest, grayForColorTest, cv::COLOR_BGR2GRAY);
						//	adjustMatChannel(grayForColorTest, 0);
						//	cv::cvtColor(matForColorTest, hsvForColorTest, cv::COLOR_BGR2HSV);
						//	int nRows = hsvForColorTest.rows;
						//	int nCols = hsvForColorTest.cols;
						//	uchar *pDataMat;
						//	for (int j = 0; j < nRows; j++)
						//	{
						//		pDataMat = grayForColorTest.ptr<uchar>(j);
						//		for (int i = 0; i < nCols; i++) {
						//			hsvForColorTest.at<cv::Vec3b>(j, i)[2] = pDataMat[i];
						//		}
						//	}
						//	//std::vector<cv::Mat> mv;
						//	//cv::split(hsvForColorTest, mv);
						//	cv::Mat means, stddev;
						//	cv::meanStdDev(hsvForColorTest, means, stddev);
						//	curHMean = static_cast<int>(means.at<double>(0));
						//	curSMean = static_cast<int>(means.at<double>(1));
						//	curVMean = static_cast<int>(means.at<double>(2));
						//	ui.labelHAverage->setText(QString::fromStdString(std::to_string(curHMean)));
						//	ui.labelSAverage->setText(QString::fromStdString(std::to_string(curSMean)));
						//	ui.labelVAverage->setText(QString::fromStdString(std::to_string(curVMean)));
						//}
						//else {
						//	std::string curColor = ui.comboBoxHaveColor->currentText().toStdString();
						//	updateColorMark(curColor);
						//}
					}
					//int startX = static_cast<int>((x1 - 27) * adaptRatio);
					//int startY = static_cast<int>((y1 - 27 - offsetY) * adaptRatio);
					//int endX = static_cast<int>((x2 - 27) * adaptRatio);
					//int endY = static_cast<int>((y2 - 27 - offsetY) * adaptRatio);

					//matForColorTest = matForDrawPart(cv::Range(startY, endY), cv::Range(startX, endX));
					//QImage imgForDrawQImage = QImage(matForColorTest.data, matForColorTest.cols, matForColorTest.rows, matForColorTest.step, QImage::Format_RGB888);
					//ui.labelDisplayColorThrd->setPixmap(QPixmap::fromImage(imgForDrawQImage).scaled(ui.labelDisplayColorThrd->width(), ui.labelDisplayColorThrd->height(), Qt::KeepAspectRatio));
					
					//cv::cvtColor(matForColorTest, matForColorTest, cv::COLOR_BGR2RGB);
				}
				
			}
		}
		else if (ev->button() == Qt::RightButton) {
			if (curState == ON_PART_AREA_MARK) {
				curState = ON_PART_AREA_MARK_SCALE;
				//curState = ON_COLOR_MARK;
				updateStepOnPageCreateModel();
			}
			
		}
	}
}

void CircuitBoardInspection::mouseMoveEvent(QMouseEvent *ev) {
	if (curState > ON_CREATE_NEW_MODEL && curState < ON_PART_PARAM_SET) {
		if (ev->buttons() & Qt::LeftButton) {
			x2 = ev->pos().x();	// (TODO)需要限制坐标
			y2 = ev->pos().y();
			update();
		}
		else if (ev->buttons() & Qt::RightButton) {
			if (curState == ON_PART_AREA_MARK_SCALE) {
				partEndX = ev->pos().x();
				partEndY = ev->pos().y();

				trueStartX += (partEndX - partStartX);
				trueStartY += (partEndY - partStartY);
				if (trueStartX <= 0) trueStartX = 0;
				if (trueStartY <= 0) trueStartY = 0;
				if ((partRect.width + trueStartX) >= frameSize.width) trueStartX = frameSize.width - partRect.width - 1;
				if ((partRect.height + trueStartY) >= frameSize.height) trueStartY = frameSize.height - partRect.height - 1;

				cv::Mat matForDraw_cp;
				matForDraw.copyTo(matForDraw_cp);	// 拷贝图片
				landMarkRun(matForDraw_cp);
				matForDrawPart = matForDraw_cp(cv::Range(trueStartY, partRect.height + trueStartY), cv::Range(trueStartX, partRect.width + trueStartX));
				//matForDrawPart = matForDraw(cv::Rect(trueStartX, trueStartY, partRect.width, partRect.height));
				update();
			}
		}
	}
}

bool CircuitBoardInspection::eventFilter(QObject *obj, QEvent *event) {
	if (isOnLandMark) {
		if (obj == ui.labelDisplay && event->type() == QEvent::Paint) {
			QPainter painter(ui.labelDisplay);
			painter.setPen(QPen(Qt::red, 1));

			ui.labelDisplay->clear();
			cv::Mat mat_cp;
			if(curState >= ON_PART_AREA_MARK_SCALE)	// 局部已经标记过了
				matForDrawPart.copyTo(mat_cp);	// 拷贝图片
			else {
				matForDraw.copyTo(mat_cp);	// 拷贝图片
				// 将图片送进去画完框出来(这里是保存过的框)
				landMarkRun(mat_cp);
			}

			if (isOnTargetColorAverage) {	// 颜色标定的时候用于画最外层框
				cv::rectangle(mat_cp, colorMarkStartPoint,
					colorMarkEndPoint, CvScalar(0, 255, 0), 8, cv::LINE_8, 0);
			}

			//// 将图片送进去画完框出来(这里是实时的框)（这里算出来的图像实际上只能往右下去，因为没有考虑其他情况）
			int startX = static_cast<int>((x1 - 27) * adaptRatio );
			int startY = static_cast<int>((y1 - 27 - offsetY) * adaptRatio);
			int endX = static_cast<int>((x2 - 27) * adaptRatio);
			int endY = static_cast<int>((y2 - 27 - offsetY) * adaptRatio);

			cv::rectangle(mat_cp, cv::Point(startX, startY), cv::Point(endX, endY), cv::Scalar(0, 255, 0), 4);
			cv::cvtColor(mat_cp, mat_cp, cv::COLOR_BGR2RGB);
			QImage imgForDrawQImage = QImage(mat_cp.data, mat_cp.cols, mat_cp.rows, mat_cp.step, QImage::Format_RGB888);
			ui.labelDisplay->setPixmap(QPixmap::fromImage(imgForDrawQImage).scaled(ui.labelDisplay->width(), ui.labelDisplay->height(), Qt::KeepAspectRatio));

			painter.end();
		}
	}
	return QWidget::eventFilter(obj, event);
}

void CircuitBoardInspection::landMarkSave() {
	std::vector<int> _landmark = { x1 - 27, y1 - 27, x2 - 27, y2 - 27 };	// 首先得到的坐标是相对于所点击控件的坐标

	_landmark[0] = static_cast<int>(_landmark[0] * adaptRatio + trueStartX);
	_landmark[1] = static_cast<int>((_landmark[1] - offsetY) * adaptRatio + trueStartY);
	_landmark[2] = static_cast<int>(_landmark[2] * adaptRatio + trueStartX);
	_landmark[3] = static_cast<int>((_landmark[3] - offsetY) * adaptRatio + trueStartY);

	landMark.push_back(_landmark);

	// 压入一行参数
	std::vector<int> _vectParamForNewModel(14, 0);
	_vectParamForNewModel[0] = vectParamForNewModel.size();
	_vectParamForNewModel[1] = -1;
	vectParamForNewModel.push_back(_vectParamForNewModel);
	// 显示更新最后一个
	updateDisplayOnPageParam(vectParamForNewModel, vectParamForNewModel.size() - 1);

	// 在这之前已经传递了一个用于显示的matForDraw过去
	m_ProcessDisplay->setBlockMsg(landMark);	// 传递新的坐标信息，实际上每次只使用了最后一个
	if (landMark.size() > 1)	// 第一个不取，只显示后面的局部信息
		m_ProcessDisplay->updateDisplayByBlockIdx(landMark.size() - 1);
}

void CircuitBoardInspection::landMarkRun(cv::Mat &mat) {	// 送进去画框
	if (curState >= ON_PART_AREA_MARK) {
		for (int i = 1; i < landMark.size(); i++) {
			cv::rectangle(mat, cv::Point(landMark[i][0], landMark[i][1]),
				cv::Point(landMark[i][2], landMark[i][3]), CvScalar(0, 255, 0), 8, cv::LINE_8, 0);
		}
	}
	else {
		for (int i = 0; i < landMark.size(); i++) {
			cv::rectangle(mat, cv::Point(landMark[i][0], landMark[i][1]),
				cv::Point(landMark[i][2], landMark[i][3]), CvScalar(0, 255, 0), 8, cv::LINE_8, 0);
		}
	}
	
}

// 初始化显示表格内容
void CircuitBoardInspection::initNewModelSettingParam() {
	//std::string curModel = ui.comboBoxHaveModel->currentText().toStdString();
	std::vector<std::vector<int>> vectParamReadByName;
	std::vector<std::vector<int>> landMarkReadByName;

	int curModelIdx = ui.comboBoxHaveModel->currentIndex();
	// 取出坐标
	std::vector<std::vector<std::string>> landMarkReadByName_str = mysql_conn->tab2Vector(myModelsName[curModelIdx][3].c_str(), 5);
	for (int i = 0; i < landMarkReadByName_str.size(); ++i) {
		std::vector<int> tmp;
		for (int j = 1; j < 5; ++j) {
			tmp.push_back(atoi(landMarkReadByName_str[i][j].c_str()));
		}
		landMarkReadByName.push_back(tmp);
	}
	std::cout << "Get Load Model Block." << std::endl;
	// 取出param
	std::vector<std::vector<std::string>> vectParamReadByName_str = mysql_conn->tab2Vector(myModelsName[curModelIdx][2].c_str(), 14);
	for (int i = 0; i < vectParamReadByName_str.size(); ++i) {
		std::vector<int> tmp;
		for (int j = 0; j < 14; ++j) {
			tmp.push_back(atoi(vectParamReadByName_str[i][j].c_str()));
		}
		vectParamReadByName.push_back(tmp);
	}
	std::cout << "Get Load Model Param." << std::endl;
	// 表格显示
	ui.tableWidgetModelMsg->setRowCount(vectParamReadByName.size() - 1);	// 初始化表格行数
	ui.tableWidgetModelMsg->setEditTriggers(QAbstractItemView::NoEditTriggers);	// 初始化不可编辑
	updateDisplayOnTableModelMsg(vectParamReadByName);


	//vectParamForNewModel.resize(landMark.size());
	//for (int i = 0; i < vectParamForNewModel.size(); ++i)
	//	vectParamForNewModel[i].resize(14, 0);	// 14列
	//for (int i = 0; i < vectParamForNewModel.size(); ++i)
	//	vectParamForNewModel[i][0] = i;	// 初始化区域编号

	//ui.tableWidgetModelMsg->setRowCount(vectParamForNewModel.size() - 1);	// 初始化表格行数
	//ui.tableWidgetModelMsg->setEditTriggers(QAbstractItemView::NoEditTriggers);	// 初始化不可编辑
	
	//updateDisplayOnTableModelMsg(vectParamForNewModel);	// 根据vect初始化显示表格

	//// 响应表格点击事件，获取行信息，然后开放上面的具体参数，开始设置
	//connect(ui.tableWidgetModelMsg, SIGNAL(cellClicked(int, int)), this, SLOT(onTableWidgetModelMsg(int, int)));

	//// 具体参数设置
	//// 区域类型
	//connect(ui.comboBoxAreaCate, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxAreaCate(int)));
	//// 有无
	//btnGroupHaveOrNot = new QButtonGroup();
	//btnGroupHaveOrNot->addButton(ui.radioButtonDetectionHaveOrNotOff, 0);	// set有无检测
	//btnGroupHaveOrNot->addButton(ui.radioButtonDetectionHaveOrNotOn, 1);	// unset有无检测
	//connect(btnGroupHaveOrNot, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupHaveOrNot(int)));
	//// 颜色
	//btnGroupColor = new QButtonGroup();
	//btnGroupColor->addButton(ui.radioButtonDetectionColorOff, 0);	// set颜色检测
	//btnGroupColor->addButton(ui.radioButtonDetectionColorOn, 1);	// unset颜色检测
	//connect(btnGroupColor, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupColor(int)));
	//connect(ui.comboBoxDetectionColorNormal, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxDetectionColorNormal(int)));
	//// 类型
	//btnGroupCate = new QButtonGroup();
	//btnGroupCate->addButton(ui.radioButtonDetectionCateOff, 0);	// set类型检测
	//btnGroupCate->addButton(ui.radioButtonDetectionCateOn, 1);	// unset类型检测
	//connect(btnGroupCate, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupCate(int)));
	//// 方向
	//btnGroupDirection = new QButtonGroup();
	//btnGroupDirection->addButton(ui.radioButtonDetectionDirectionOff, 0);	// set方向检测
	//btnGroupDirection->addButton(ui.radioButtonDetectionDirectionOn, 1);	// unset方向检测
	//connect(btnGroupDirection, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupDirection(int)));
	//// 位置
	//btnGroupPos = new QButtonGroup();
	//btnGroupPos->addButton(ui.radioButtonDetectionPosOff, 0);	// set位置检测
	//btnGroupPos->addButton(ui.radioButtonDetectionPosOn, 1);	// unset位置检测
	//connect(btnGroupPos, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupPos(int)));
	//connect(ui.comboBoxStartID, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxStartID(int)));
	//connect(ui.lineEditDetectionPosNormal, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionPosNormal()));
	//connect(ui.lineEditDetectionPosOffset, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionPosOffset()));
	//// 长度
	//btnGroupLength = new QButtonGroup();
	//btnGroupLength->addButton(ui.radioButtonDetectionLengthOff, 0);	// set位置检测
	//btnGroupLength->addButton(ui.radioButtonDetectionLengthOn, 1);	// unset位置检测
	//connect(btnGroupLength, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupLength(int)));
	//connect(ui.lineEditDetectionLengthNormal, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionLengthNormal()));
	//connect(ui.lineEditDetectionLengthOffset, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionLengthOffset()));
}

void CircuitBoardInspection::onComboBoxAreaCate(int idx) {
	disconnect(ui.comboBoxDetectionColorNormal, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxDetectionColorNormal(int)));
	vectParamForNewModel.back()[1] = idx;

	ui.comboBoxDetectionColorNormal->clear();
	// 插入可选颜色
	switch (idx)
	{
	case 0: {	// 花结束带
		ui.comboBoxDetectionColorNormal->addItem("灰");
		ui.comboBoxDetectionColorNormal->addItem("白");
		ui.comboBoxDetectionColorNormal->addItem("粉");
		break;
	}
	case 1: {	// T18R
		ui.comboBoxDetectionColorNormal->addItem("白");
		ui.comboBoxDetectionColorNormal->addItem("黑");
		ui.comboBoxDetectionColorNormal->addItem("黄");
		break;
	}
	case 2: {	// 胶布
		ui.comboBoxDetectionColorNormal->addItem("红");
		ui.comboBoxDetectionColorNormal->addItem("白");
		ui.comboBoxDetectionColorNormal->addItem("黑");
		ui.comboBoxDetectionColorNormal->addItem("黄");
		ui.comboBoxDetectionColorNormal->addItem("绿");
		ui.comboBoxDetectionColorNormal->addItem("灰");
		break;
	}
	case 3: {	// 插头
		ui.comboBoxDetectionColorNormal->addItem("黑");
		ui.comboBoxDetectionColorNormal->addItem("白");
		ui.comboBoxDetectionColorNormal->addItem("粉");
		ui.comboBoxDetectionColorNormal->addItem("蓝");
		ui.comboBoxDetectionColorNormal->addItem("灰");
		ui.comboBoxDetectionColorNormal->addItem("绿");
		ui.comboBoxDetectionColorNormal->addItem("黄");
		break;
	}
	default:
		break;
	}
	ui.comboBoxDetectionColorNormal->setCurrentIndex(-1);

	// 插头方向
	if (idx == 3) {
		ui.comboBoxPlugMask->setEnabled(true);
		ui.comboBoxPlugMask->setCurrentIndex(-1);
	}
	else {
		ui.comboBoxPlugMask->setDisabled(true);
		ui.comboBoxPlugMask->setCurrentIndex(-1);
	}
	
	//ui.groupBoxDetectHaveOrNot->setEnabled(true);
	//ui.groupBoxDetectionColor->setEnabled(true);
	//ui.groupBoxDetectionCate->setEnabled(true);
	//ui.groupBoxDetectionDirection->setEnabled(true);
	//ui.groupBoxDetectionLength->setEnabled(true);
	//ui.groupBoxDetectionPos->setEnabled(true);
	
	ui.groupBoxDetectionColor->setEnabled(true);
	ui.widgetDetectionColor->setDisabled(true);
	connect(ui.comboBoxDetectionColorNormal, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxDetectionColorNormal(int)));
}

void CircuitBoardInspection::onComboBoxDetectionColorNormal(int idx) {
	//std::cout << idx << " " << vectParamForNewModel[curSetBlock + 1][3] << std::endl;
	//vectParamForNewModel[curSetBlock + 1][3] = idx;
	if (idx != -1) {
		ui.groupBoxDetectHaveOrNot->setEnabled(true);
		//ui.groupBoxDetectionColor->setEnabled(true);
		ui.groupBoxDetectionCate->setEnabled(true);
		ui.groupBoxDetectionDirection->setEnabled(true);
		ui.groupBoxDetectionLength->setEnabled(true);
		ui.groupBoxDetectionPos->setEnabled(true);
		ui.widgetDetectionColor->setEnabled(true);

		vectParamForNewModel.back()[3] = idx;
		std::cout << "Color: " << idx << std::endl;
		// 每当选定颜色的时候，根据当前类型ID与颜色ID，进一步细化目标区域信息
		m_ProcessDisplay->setParamMsg(vectParamForNewModel);	// 刷新参数
		m_ProcessDisplay->updateAlgorithmDisplay(vectParamForNewModel.back()[1], vectParamForNewModel.back()[3], -1);	// (型号，颜色, -1表明只缩小目标区域)
	}
	else {
		ui.groupBoxDetectHaveOrNot->setDisabled(true);
		//ui.groupBoxDetectionColor->setDisabled(true);
		ui.groupBoxDetectionCate->setDisabled(true);
		ui.groupBoxDetectionDirection->setDisabled(true);
		ui.groupBoxDetectionLength->setDisabled(true);
		ui.groupBoxDetectionPos->setDisabled(true);
	}
}

void CircuitBoardInspection::onComboBoxStartID(int startIdx) {
	//if (startIdx != -1)
		//vectParamForNewModel[curSetBlock + 1][8] = ui.comboBoxStartID->itemText(startIdx).toInt();
	std::cout << "onComboBoxStartID: " << startIdx << std::endl;
	vectParamForNewModel.back()[8] = ui.comboBoxStartID->itemText(startIdx).toInt();
}

void CircuitBoardInspection::onLineEditDetectionPosNormal() {
	//vectParamForNewModel[curSetBlock + 1][9] = ui.lineEditDetectionPosNormal->text().toInt();
	vectParamForNewModel.back()[9] = ui.lineEditDetectionPosNormal->text().toInt();
}

void CircuitBoardInspection::onLineEditDetectionPosOffset() {
	//vectParamForNewModel[curSetBlock + 1][10] = ui.lineEditDetectionPosOffset->text().toInt();
	vectParamForNewModel.back()[10] = ui.lineEditDetectionPosOffset->text().toInt();
}
 
void CircuitBoardInspection::onLineEditDetectionLengthNormal() {
	//vectParamForNewModel[curSetBlock + 1][5] = ui.lineEditDetectionLengthNormal->text().toInt();
	vectParamForNewModel.back()[5] = ui.lineEditDetectionLengthNormal->text().toInt();
	//std::cout << vectParamForNewModel.back()[5] << std::endl;
}

void CircuitBoardInspection::onLineEditDetectionLengthOffset() {
	//vectParamForNewModel[curSetBlock + 1][6] = ui.lineEditDetectionLengthOffset->text().toInt();
	vectParamForNewModel.back()[6] = ui.lineEditDetectionLengthOffset->text().toInt();
}

void CircuitBoardInspection::onBtnGroupHaveOrNot(int idx) {	// 有无检测
	//std::cout << "onBtnGroupHaveOrNot: " << idx << std::endl;
	//vectParamForNewModel[curSetBlock + 1][11] = idx;
	vectParamForNewModel.back()[11] = idx;
}

void CircuitBoardInspection::onBtnGroupColor(int idx) {	// 颜色检测
	//std::cout << "onBtnGroupColor: " << idx << std::endl;
	//vectParamForNewModel[curSetBlock + 1][2] = idx;
	vectParamForNewModel.back()[2] = idx;
}

void CircuitBoardInspection::onBtnGroupCate(int idx) {	// 类型检测
	//std::cout << "onBtnGroupCate: " << idx << std::endl;
	//vectParamForNewModel[curSetBlock + 1][12] = idx;
	if (idx == 1) {
		if (vectParamForNewModel.back()[1] != 0 && vectParamForNewModel.back()[1] != 1) {	// 类型检测只针对扎带
			ui.radioButtonDetectionCateOff->setChecked(true);
			QMessageBox::warning(NULL, "警告", "类型检测仅针对扎带！", QMessageBox::Yes);
			return;
		}
		else {
			vectParamForNewModel.back()[12] = idx;
		}
	}
	else {
		vectParamForNewModel.back()[12] = idx;
	}
	
}

void CircuitBoardInspection::onBtnGroupDirection(int idx) {	// 方向检测
	//std::cout << "onBtnGroupDirection: " << idx << std::endl;
	//vectParamForNewModel[curSetBlock + 1][13] = idx;
	//vectParamForNewModel.back()[13] = idx;
	if (idx == 1) {
		if (vectParamForNewModel.back()[1] == 3) {	// 如果是插头方向检测的话
			// 必须要选择mask类型
			if (ui.comboBoxPlugMask->currentIndex() != -1) {	// 
				plugDirectMaskMsg.push_back({ vectParamForNewModel.back()[0],  ui.comboBoxPlugMask->currentIndex() });
				vectParamForNewModel.back()[13] = idx;
			}
			else {	// 没有选择mask
				ui.radioButtonDetectionDirectionOff->setChecked(true);
				QMessageBox::warning(NULL, "警告", "插头方向检测请选择Mask类型！", QMessageBox::Yes);
				return;
			}
		}
		else {
			vectParamForNewModel.back()[13] = idx;
		}
	}
	else {
		vectParamForNewModel.back()[13] = idx;
	}
}

void CircuitBoardInspection::onBtnGroupPos(int idx) {	// 位置检测
	//std::cout << "onBtnGroupPos: " << idx << std::endl;
	//vectParamForNewModel[curSetBlock + 1][7] = idx;
	if (idx == 1) {
		if (vectParamForNewModel.back()[9] == 0 || vectParamForNewModel.back()[10] == 0) {
			ui.radioButtonDetectionPosOff->setChecked(true);
			QMessageBox::warning(NULL, "警告", "请先填写参数！", QMessageBox::Yes);
			return;
		}
		else if (ui.comboBoxStartID->currentIndex() == -1) {
			ui.radioButtonDetectionPosOff->setChecked(true);
			QMessageBox::warning(NULL, "警告", "请选择起始ID！", QMessageBox::Yes);
			return;
		}
		else {
			vectParamForNewModel.back()[7] = idx;
			m_ProcessDisplay->setParamMsg(vectParamForNewModel);	// 刷新参数
			m_ProcessDisplay->updateAlgorithmDisplay(vectParamForNewModel.back()[1], vectParamForNewModel.back()[3], 2);	// (型号，颜色, 位置检测)
		}
	}
	else {
		vectParamForNewModel.back()[7] = idx;
	}
}

void CircuitBoardInspection::onBtnGroupLength(int idx) {	// 长度检测
	//std::cout << "onBtnGroupLength: " << idx << std::endl;
	//vectParamForNewModel[curSetBlock + 1][4] = idx;
	//vectParamForNewModel.back()[4] = idx;
	if (idx == 1) {
		if (vectParamForNewModel.back()[5] == 0 || vectParamForNewModel.back()[6] == 0) {
			ui.radioButtonDetectionLengthOff->setChecked(true);
			QMessageBox::warning(NULL, "警告", "请先填写参数！", QMessageBox::Yes);
			return;
		}
		else {
			if (vectParamForNewModel.back()[1] != 2){	// 只有胶布可以进行长度检测
				ui.radioButtonDetectionLengthOff->setChecked(true);
				QMessageBox::warning(NULL, "警告", "长度检测仅针对胶布！", QMessageBox::Yes);
				return;
			}
			else {
				vectParamForNewModel.back()[4] = idx;
				// 已经填入参数，可以开始预检测显示效果
				m_ProcessDisplay->setParamMsg(vectParamForNewModel);	// 刷新参数
				m_ProcessDisplay->updateAlgorithmDisplay(vectParamForNewModel.back()[1], vectParamForNewModel.back()[3], 1);	// (型号，颜色, 长度检测)
			}
		}
	}
	else {
		vectParamForNewModel.back()[4] = idx;
	}
}

void CircuitBoardInspection::onTableWidgetModelMsg(int cRow, int cCol) {
	std::cout << "Tabel row[" << cRow << "]" << std::endl;
	curSetBlock = cRow;
	curState = ON_SET_BLOCK_PARAM;
	updateStepOnPageCreateModel();
	if (cRow == 0)	// 第一行无法位置检测
		ui.groupBoxDetectionPos->setEnabled(false);

	//m_ProcessDisplay->updateDisplayByBlockIdx(cRow + 1);
}

void CircuitBoardInspection::updateDisplayOnPageParam(std::vector<std::vector<int>> &vect, int curIdx) {
	// 只需要显示某一行的参数
	// curSetBlock: 某一行
	// 区域类型
	ui.comboBoxAreaCate->setCurrentIndex(vect[curIdx][1]);
	// 有无检测
	if (vect[curIdx][11] == 0)
		ui.radioButtonDetectionHaveOrNotOff->setChecked(true);
	else
		ui.radioButtonDetectionHaveOrNotOn->setChecked(true);
	// 颜色检测
	if (vect[curIdx][2] == 0)
		ui.radioButtonDetectionColorOff->setChecked(true);
	else
		ui.radioButtonDetectionColorOn->setChecked(true);
	if (ui.comboBoxDetectionColorNormal->count() == 0) {
		switch (vect[curIdx][1])
		{
		case 0: {	// 花结束带
			ui.comboBoxDetectionColorNormal->addItem("灰");
			ui.comboBoxDetectionColorNormal->addItem("白");
			ui.comboBoxDetectionColorNormal->addItem("粉");
			break;
		}
		case 1: {	// T18R
			ui.comboBoxDetectionColorNormal->addItem("白");
			ui.comboBoxDetectionColorNormal->addItem("黑");
			ui.comboBoxDetectionColorNormal->addItem("黄");
			break;
		}
		case 2: {	// 胶布
			ui.comboBoxDetectionColorNormal->addItem("红");
			ui.comboBoxDetectionColorNormal->addItem("白");
			ui.comboBoxDetectionColorNormal->addItem("黑");
			ui.comboBoxDetectionColorNormal->addItem("黄");
			ui.comboBoxDetectionColorNormal->addItem("绿");
			ui.comboBoxDetectionColorNormal->addItem("灰");
			break;
		}
		case 3: {	// 插头
			ui.comboBoxDetectionColorNormal->addItem("黑");
			ui.comboBoxDetectionColorNormal->addItem("白");
			ui.comboBoxDetectionColorNormal->addItem("粉");
			ui.comboBoxDetectionColorNormal->addItem("蓝");
			ui.comboBoxDetectionColorNormal->addItem("灰");
			ui.comboBoxDetectionColorNormal->addItem("绿");
			ui.comboBoxDetectionColorNormal->addItem("黄");
			break;
		}
		default:
			break;
		}
	}
	// 设置当前颜色索引
	ui.comboBoxDetectionColorNormal->setCurrentIndex(vect[curIdx][3]);
	// 类型检测
	if (vect[curIdx][12] == 0)
		ui.radioButtonDetectionCateOff->setChecked(true);
	else
		ui.radioButtonDetectionCateOn->setChecked(true);
	// 方向检测
	if (vect[curIdx][13] == 0)
		ui.radioButtonDetectionDirectionOff->setChecked(true);
	else
		ui.radioButtonDetectionDirectionOn->setChecked(true);
	// 位置检测
	if (vect[curIdx][7] == 0)
		ui.radioButtonDetectionPosOff->setChecked(true);
	else
		ui.radioButtonDetectionPosOn->setChecked(true);

	disconnect(ui.comboBoxStartID, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxStartID(int)));
	ui.comboBoxStartID->clear();
	for (int preIdx = 1; preIdx < curIdx; ++preIdx)
		ui.comboBoxStartID->addItem(QString::fromStdString(std::to_string(preIdx)));
	//if (curSetBlock == 1)
		ui.comboBoxStartID->setCurrentIndex(-1);
	//else
	//	ui.comboBoxStartID->setCurrentIndex(vect[curSetBlock + 1][8]);

	connect(ui.comboBoxStartID, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxStartID(int)));

	ui.lineEditDetectionPosNormal->setText(QString::fromStdString(std::to_string(vect[curIdx][9])));
	ui.lineEditDetectionPosOffset->setText(QString::fromStdString(std::to_string(vect[curIdx][10])));
	// 长度检测
	if (vect[curIdx][4] == 0)
		ui.radioButtonDetectionLengthOff->setChecked(true);
	else
		ui.radioButtonDetectionLengthOn->setChecked(true);
	ui.lineEditDetectionLengthNormal->setText(QString::fromStdString(std::to_string(vect[curIdx][5])));
	ui.lineEditDetectionLengthOffset->setText(QString::fromStdString(std::to_string(vect[curIdx][6])));
}

void CircuitBoardInspection::onLandMarkChangeFromProcessDisplay(int idx, int x1, int y1, int x2, int y2) {
	std::cout << "[" << idx << "]" << std::endl;
	std::cout << landMark[idx][0] << " -> " << x1 << std::endl;
	std::cout << landMark[idx][1] << " -> " << y1 << std::endl;
	std::cout << landMark[idx][2] << " -> " << x2 << std::endl;
	std::cout << landMark[idx][3] << " -> " << y2 << std::endl;

	landMark[idx][0] = x1;
	landMark[idx][1] = y1;
	landMark[idx][2] = x2;
	landMark[idx][3] = y2;
}

void CircuitBoardInspection::onLengthParamModify(int idx, int pixelNorm, int pixelOffset, double angle) {
	std::cout << "[" << idx << "]" << std::endl;

	for (int i = lengthProcessParam.size(); i <= idx; ++i) {
		lengthProcessParam.push_back({"0", "0", "0.0"});
	}

	std::cout << lengthProcessParam[idx][0] << " -> " << pixelNorm << std::endl;
	std::cout << lengthProcessParam[idx][1] << " -> " << pixelOffset << std::endl;
	std::cout << lengthProcessParam[idx][2] << " -> " << angle << std::endl;

	lengthProcessParam[idx][0] = std::to_string(pixelNorm);
	lengthProcessParam[idx][1] = std::to_string(pixelOffset);
	lengthProcessParam[idx][2] = std::to_string(angle);
}

void CircuitBoardInspection::onTrueTargetPartAxisModify(int idx, int leftUpX, int leftUpY, int rightBottonX, int rightBottonY, int centerX, int centerY) {
	std::cout << "[" << idx << "]" << std::endl;

	for (int i = trueTargetPartAxis.size(); i <= idx; ++i) {
		trueTargetPartAxis.push_back({ 0, 0, 0, 0, 0, 0 });
	}

	// trueTargetPartAxis.push_back({ leftUpX, leftUpY, rightBottonX, rightBottonY, centerX, centerY });

	std::cout << trueTargetPartAxis[idx][0] << " -> " << leftUpX << std::endl;
	std::cout << trueTargetPartAxis[idx][1] << " -> " << leftUpY << std::endl;
	std::cout << trueTargetPartAxis[idx][2] << " -> " << rightBottonX << std::endl;
	std::cout << trueTargetPartAxis[idx][3] << " -> " << rightBottonY << std::endl;
	std::cout << trueTargetPartAxis[idx][4] << " -> " << centerX << std::endl;
	std::cout << trueTargetPartAxis[idx][5] << " -> " << centerY << std::endl;

	trueTargetPartAxis[idx][0] = leftUpX;
	trueTargetPartAxis[idx][1] = leftUpY;
	trueTargetPartAxis[idx][2] = rightBottonX;
	trueTargetPartAxis[idx][3] = rightBottonY;
	trueTargetPartAxis[idx][4] = centerX;
	trueTargetPartAxis[idx][5] = centerY;
}

void CircuitBoardInspection::onTwoTargetDistance(int idx, int pixelNorm, int pixelOffset) {
	std::cout << "[" << idx << "]" << std::endl;

	for (int i = posProcessParam.size(); i <= idx; ++i) {
		posProcessParam.push_back({ 0, 0});
	}

	std::cout << posProcessParam[idx][0] << " -> " << pixelNorm << std::endl;
	std::cout << posProcessParam[idx][1] << " -> " << pixelOffset << std::endl;

	posProcessParam[idx][0] = pixelNorm;
	posProcessParam[idx][1] = pixelOffset;
}

//// 根据每个block的类型，首先做一次相关的算法检测，并显示
//void CircuitBoardInspection::updateAlgorithmDisplay(int cateIdx, int detectIdx) {
//	// 插入可选颜色
//	// detectIdx: 0		颜色
//	// detectIdx: 1		长度
//	// detectIdx: 2		位置
//	// detectIdx: 3		有无
//	// detectIdx: 4		类型
//	// detectIdx: 5		方向
//	switch (cateIdx)
//	{
//		case 0: {	// 花结束带
//
//			break;
//		}
//		case 1: {	// T18R
//
//			break;
//		}
//		case 2: {	// 胶布
//			switch (detectIdx)
//			{
//			case 1: {
//				break;
//			}
//			default:
//				break;
//			}
//			break;
//		}
//		case 3: {	// 插头
//
//			break;
//		}
//		default:
//			break;
//	}
//}

void CircuitBoardInspection::updateDisplayOnTableModelMsg(std::vector<std::vector<int>> &vect) {
	for (int i = 1; i < vect.size(); ++i) {	// 第一行代表的是最大范围，不属于局部
		for (int j = 0; j < vect[i].size(); ++j) {
			// [1]区域编号 : (int)
			// [2]区域类型 : 花结束带(0) / T18R(1) / 胶布(2) / 插头(3)
			// [3]颜色检测 : 否(0) / 是(1)
			// [4]颜色规格值 :	1.扎带(花结束带[灰(0)、白(1)、粉(2)] / T18R[白(0)、黑(1)、黄(2)])	// 6种
			//					2.胶布(红(0)、白(1)、黑(2)、黄(3)、绿(4)、灰(5))		// 6种
			//					3.插头(黑(0)、白(1)、粉(2)、蓝(3)、灰(4)、绿(5)、黄(6))	// 7种
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
			switch (j + 1)
			{
				case 1: {	// 区域编号
					ui.tableWidgetModelMsg->setItem(i - 1, j, new QTableWidgetItem(QString::fromStdString(std::to_string(vect[i][0]))));
					ui.tableWidgetModelMsg->item(i - 1, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);	// 设置单元格居中
					break;
				}
				case 2: {	// 区域类型
					// 首先插入box
					QComboBox *cbBox = new QComboBox();
					cbBox->addItem("花结束带");
					cbBox->addItem("T18R");
					cbBox->addItem("胶布");
					cbBox->addItem("插头");
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// 根据当前索引更新Index
					cbBox->setCurrentIndex(vect[i][1]);
					break;
				}
				case 3: {	// 颜色检测
					QComboBox *cbBox = new QComboBox();
					cbBox->addItem("否");
					cbBox->addItem("是");
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// 根据当前索引更新Index
					cbBox->setCurrentIndex(vect[i][2]);
					break;
				}
				case 4: {	// 颜色规格值
					ui.tableWidgetModelMsg->removeCellWidget(i - 1, j);
					QComboBox *cbBox = new QComboBox();
					switch (vect[i][1])
					{
						case 0: {	// 花结束带
							cbBox->addItem("灰");
							cbBox->addItem("白");
							cbBox->addItem("粉");
							break;
						}
						case 1: {	// T18R
							cbBox->addItem("白");
							cbBox->addItem("黑");
							cbBox->addItem("黄");
							break;
						}
						case 2: {	// 胶布
							cbBox->addItem("红");
							cbBox->addItem("白");
							cbBox->addItem("黑");
							cbBox->addItem("黄");
							cbBox->addItem("绿");
							cbBox->addItem("灰");
							break;
						}
						case 3: {	// 插头
							cbBox->addItem("黑");
							cbBox->addItem("白");
							cbBox->addItem("粉");
							cbBox->addItem("蓝");
							cbBox->addItem("灰");
							cbBox->addItem("绿");
							cbBox->addItem("黄");
							break;
						}
						default:
							break;
					}
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// 根据当前索引更新Index
					if (vect[i][2] == 0)	// 如果不检测的话，失能
						cbBox->setDisabled(true);
					cbBox->setCurrentIndex(vect[i][3]);
					break;
				}
				case 5: {	// 长度限制检测
					QComboBox *cbBox = new QComboBox();
					cbBox->addItem("否");
					cbBox->addItem("是");
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// 根据当前索引更新Index
					cbBox->setCurrentIndex(vect[i][4]);
					break;
				}
				case 6: {	// 长度规格值
					QLabel *lb = new QLabel();
					lb->setAlignment(Qt::AlignHCenter);
					lb->setText(QString::fromStdString(std::to_string(vect[i][5])));
					QWidget *lbWidget = new QWidget();
					QHBoxLayout *lbLayout = new QHBoxLayout();
					lbLayout->setMargin(2);
					lbLayout->addWidget(lb);
					lbLayout->setAlignment(Qt::AlignHCenter);
					lbWidget->setLayout(lbLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, lbWidget);
					if (vect[i][4] == 0)	// 如果不检测的话，失能
						lb->setDisabled(true);
					break;
				}
				case 7: {	// 长度偏差值
					QLabel *lb = new QLabel();
					lb->setAlignment(Qt::AlignHCenter);
					lb->setText(QString::fromStdString(std::to_string(vect[i][6])));
					QWidget *lbWidget = new QWidget();
					QHBoxLayout *lbLayout = new QHBoxLayout();
					lbLayout->setMargin(2);
					lbLayout->addWidget(lb);
					lbLayout->setAlignment(Qt::AlignHCenter);
					lbWidget->setLayout(lbLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, lbWidget);
					if (vect[i][4] == 0)	// 如果不检测的话，失能
						lb->setDisabled(true);
					break;
				}
				case 8: {	// 位置检测
					if (i == 1)
						continue;
					QComboBox *cbBox = new QComboBox();
					cbBox->addItem("否");
					cbBox->addItem("是");
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// 根据当前索引更新Index
					cbBox->setCurrentIndex(vect[i][7]);
					break;
				}
				case 9: {	// 起始ID
					if (i == 1)
						continue;
					//ui.tableWidgetModelMsg->removeCellWidget(i - 1, j);
					//QComboBox *cbBox = new QComboBox();
					//for (int preIdx = 1; preIdx < i; ++preIdx)
					//	cbBox->addItem(QString::fromStdString(std::to_string(preIdx)));
					//QWidget *cbBoxWidget = new QWidget();
					//QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					//cbBoxLayout->setMargin(2);
					//cbBoxLayout->addWidget(cbBox);
					//cbBoxLayout->setAlignment(Qt::AlignHCenter);
					//cbBoxWidget->setLayout(cbBoxLayout);
					//ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					QLabel *lb = new QLabel();
					lb->setAlignment(Qt::AlignHCenter);
					lb->setText(QString::fromStdString(std::to_string(vect[i][8])));
					QWidget *lbWidget = new QWidget();
					QHBoxLayout *lbLayout = new QHBoxLayout();
					lbLayout->setMargin(2);
					lbLayout->addWidget(lb);
					lbLayout->setAlignment(Qt::AlignHCenter);
					lbWidget->setLayout(lbLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, lbWidget);
					if (vect[i][7] == 0)	// 如果不检测的话，失能
						lb->setDisabled(true);
					//if (vect[i][7] == 0)	// 如果不检测的话，失能
					//	cbBox->setDisabled(true);
					// 根据当前索引更新Index
					//cbBox->setCurrentIndex(vect[i][8]);
					break;
				}
				case 10: {	// 位置规格值
					if (i == 1)
						continue;
					QLabel *lb = new QLabel();
					lb->setAlignment(Qt::AlignHCenter);
					lb->setText(QString::fromStdString(std::to_string(vect[i][9])));
					QWidget *lbWidget = new QWidget();
					QHBoxLayout *lbLayout = new QHBoxLayout();
					lbLayout->setMargin(2);
					lbLayout->addWidget(lb);
					lbLayout->setAlignment(Qt::AlignHCenter);
					lbWidget->setLayout(lbLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, lbWidget);
					if (vect[i][7] == 0)	// 如果不检测的话，失能
						lb->setDisabled(true);
					break;
				}
				case 11: {	// 位置偏差值
					if (i == 1)
						continue;
					QLabel *lb = new QLabel();
					lb->setAlignment(Qt::AlignHCenter);
					lb->setText(QString::fromStdString(std::to_string(vect[i][10])));
					QWidget *lbWidget = new QWidget();
					QHBoxLayout *lbLayout = new QHBoxLayout();
					lbLayout->setMargin(2);
					lbLayout->addWidget(lb);
					lbLayout->setAlignment(Qt::AlignHCenter);
					lbWidget->setLayout(lbLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, lbWidget);
					if (vect[i][7] == 0)	// 如果不检测的话，失能
						lb->setDisabled(true);
					break;
				}
				case 12: {	// 有无检测
					QComboBox *cbBox = new QComboBox();
					cbBox->addItem("否");
					cbBox->addItem("是");
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// 根据当前索引更新Index
					cbBox->setCurrentIndex(vect[i][11]);
					break;
				}
				case 13: {	// 类型检测
					QComboBox *cbBox = new QComboBox();
					cbBox->addItem("否");
					cbBox->addItem("是");
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// 根据当前索引更新Index
					cbBox->setCurrentIndex(vect[i][12]);
					break;
				}
				case 14: {	// 方向检测
					QComboBox *cbBox = new QComboBox();
					cbBox->addItem("否");
					cbBox->addItem("是");
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// 根据当前索引更新Index
					cbBox->setCurrentIndex(vect[i][13]);
					break;
				}
				default:
					break;
				}
		}
	}
}

void CircuitBoardInspection::adjustMatChannel(cv::Mat& src, int chl) {
	// 

	if (src.channels() != 1) {
		std::cout << "Only One Channel Src." << std::endl;
		return;
	}

	std::vector <cv::Mat> mv;
	split(src, mv);
	cv::Mat dstChl = mv[chl];

	int nRows = dstChl.rows;
	int nCols = dstChl.cols;
	uchar *pDataMat;
	//Vec3d
	int pixMax = 0, pixMin = 255;
	for (int j = 0; j < nRows; j++)
	{
		pDataMat = dstChl.ptr<uchar>(j);//ptr<>()得到的是一行指针
		for (int i = 0; i < nCols; i++)
		{
			if (pDataMat[i] > pixMax)
				pixMax = pDataMat[i];
			if (pDataMat[i] < pixMin)
				pixMin = pDataMat[i];
		}
	}
	pixMin = 0;
	//对比度拉伸映射
	for (int j = 0; j < nRows; j++)
	{
		pDataMat = dstChl.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++)
		{
			pDataMat[i] = (pDataMat[i] - pixMin) * 255 / (pixMax - pixMin);
		}
	}
	// cout << src.channels() << endl;
	if (src.channels() == 3) {

		for (int j = 0; j < nRows; j++)
		{
			pDataMat = dstChl.ptr<uchar>(j);
			for (int i = 0; i < nCols; i++) {
				src.at<cv::Vec3b>(j, i)[chl] = pDataMat[i];
			}
		}
	}
	else {
		uchar *pDataOriginMat;
		for (int j = 0; j < nRows; j++)
		{
			pDataOriginMat = src.ptr<uchar>(j);
			pDataMat = dstChl.ptr<uchar>(j);
			for (int i = 0; i < nCols; i++) {
				pDataOriginMat[i] = pDataMat[i];
			}
		}
	}

}