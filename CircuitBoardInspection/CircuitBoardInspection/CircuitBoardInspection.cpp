#pragma execution_character_set("utf-8")
#include "CircuitBoardInspection.h"

CircuitBoardInspection::CircuitBoardInspection(MyCamera *myCamera, MysqlSetting *mysql_conn, QWidget *parent)
    : myCamera(myCamera), mysql_conn(mysql_conn), QWidget(parent)
{
    ui.setupUi(this);
	setFixedSize(this->width(), this->height());
	// ����ģ����
	myModelsName = mysql_conn->tab2Vector("model", 7);
	//std::cout << "myModelsName: " << myModelsName.size() << std::endl;
	//if (myModelsName.size() > 0) {
	//	for (int i = 0; i < myModelsName.size(); ++i) {
	//		for (int j = 0; j < myModelsName[i].size(); ++j)
	//			std::cout << myModelsName[i][j] << " ";
	//		std::cout << std::endl;
	//	}
	//}
	// ������ɫHSV��Ϣ
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
		QMessageBox::information(NULL, "��ʾ", "�������ñ��Ŀ����ɫ������ƫ�", QMessageBox::Yes);
		ui.checkBoxColorMark->setChecked(false);
	}
}

void CircuitBoardInspection::onButtonModifyColor() {
	// ������ɫ��Ϣ
	ui.labelDisplayColorThrd->clear();
	// ���ú�/��/��
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
	QMessageBox::information(NULL, "��ʾ", "��ɫ��Ϣ������⣡", QMessageBox::Yes);

	ui.toolBox->setCurrentIndex(1);	// ��ת���������ò���һҳ
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
	cv::Mat structureElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));      //�����ṹԪ
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

// ��ȡ����ͼ��ۺ���
void CircuitBoardInspection::onButtonSingleImage() {
	std::cout << "Get One Frame" << std::endl;
	// ��ȡ����ͼ��
	/**********************************************************/
	cv::Mat singleMat(myCamera->getCameraResolution(), CV_8UC3);
	myCamera->getOneFrame(singleMat);
	//singleMat = cv::imread("./test/norm.jpg");	// ���Ա�׼ͼ��
	/**********************************************************/
	//cv::imshow("singleMat", singleMat);
	// ��ʾ
	// ������ʾ���ԣ�ֻ��BGR������������ɫ��������Ҫת��
	cv::cvtColor(singleMat, singleMat, cv::COLOR_BGR2RGB);
	QImage singleQImage(singleMat.data, singleMat.cols, singleMat.rows, QImage::Format_RGB888);
	ui.labelDisplay->setPixmap(QPixmap::fromImage(singleQImage).
		scaled(ui.labelDisplay->width(), ui.labelDisplay->height(), Qt::KeepAspectRatio));

	// ���²�����ʾ
	if (curState != ON_GET_CONTINUOUS_IMAGE) {	// ������ȡͼ��׼������һ��
		curState = ON_LOAD_OR_CREATE_MODEL;
		updateStepOnPageCreateModel();
	}
}

// ��ȡ����ͼ��ۺ���
void CircuitBoardInspection::onButtonContinuousImage() {
	// ������ʱ������ͣ��ȡ���Ų���ͼ��
	if (timerForContinuousFrame == NULL) {
		timerForContinuousFrame = new QTimer();
		timerForContinuousFrame->setInterval(20);	// interval for timer
		connect(timerForContinuousFrame, SIGNAL(timeout()), this, SLOT(onButtonSingleImage()));
	}

	if (timerForContinuousFrame->isActive() == false) {	// ����������ȡͼ��ʱ��
		timerForContinuousFrame->start();
		ui.buttonContinuousImage->setText("ֹͣ��ȡͼ��");

		curState = ON_GET_CONTINUOUS_IMAGE;
	}
	else {	// ֹͣ��ʱ��
		timerForContinuousFrame->stop();
		disconnect(timerForContinuousFrame, SIGNAL(timeout()), this, SLOT(onButtonSingleImage()));
		delete timerForContinuousFrame;
		timerForContinuousFrame = NULL;
		ui.buttonContinuousImage->setText("��ȡ��������ͼ��");

		curState = ON_GET_IMAGE;
	}
	updateStepOnPageCreateModel();
}

// ����ģ��
void CircuitBoardInspection::onButtonLoadModel() {
	std::cout << "buttonLoadModel clicked" << std::endl;

	// �����������ȥ���ݿ���ȡ��block��param�����ݸ�������
	emit signalLoadModel(ui.comboBoxHaveModel->currentText());

	curState = ON_LOAD_MODEL;
	updateStepOnPageCreateModel();
}

// �½�ģ��
void CircuitBoardInspection::onButtonCreateNewModel() {
	std::cout << "buttonCreateNewModel clicked" << std::endl;

	if (ui.lineEditNewModelName->text().isEmpty()) {	// (TODO)������������涨��ʽ
		QMessageBox::information(NULL, "��ʾ", "<�½�ģ������>����", QMessageBox::Yes);
	}
	else {	// û���⣬���봴��ģ�͵Ĺ���
		// ��ȡ�½�ģ������
		newModelName = ui.lineEditNewModelName->text();
		std::cout << "create new model [" << newModelName.toStdString() << "]" << std::endl;

		// ��ȡһ��ͼ��(�����½�)
		/**********************************************/
		myCamera->getOneFrame(matForDraw);
		//matForDraw = cv::imread("./test/norm.jpg");	// ���Ա�׼ͼ��
		frameSize = myCamera->getCameraResolution();
		/**********************************************/
		// ����ͼ��ʵ�����ŵı���
		float ratioX = static_cast<float>(frameSize.width) / ui.labelDisplay->width();
		float ratioY = static_cast<float>(frameSize.height) / ui.labelDisplay->height();
		adaptRatio = MAX(ratioX, ratioY);	// �������ŵı���ȡС��
		offsetY = static_cast<int>((ui.labelDisplay->height() - (frameSize.height / adaptRatio)) / 2);

		// �����ļ������ڱ��浱ǰ����block��mask
		std::string path = ".\\mask\\" + newModelName.toStdString();
		if (_access(path.c_str(), 0) == -1) {
			std::string cmd = "mkdir " + path;
			system(cmd.c_str());
		}
		else {
			std::cout << path << " has Exist." << std::endl;
		}
		// �����ļ������ڱ��浱ǰ����block�Ķ�ģ��
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

// �½�ģ�ͱ������
void CircuitBoardInspection::onButtonStartCircleROI() {
	// ��ʼ��ͼ
	// (TODO)�����ͼ���Ҽ��ƶ�
	if (curState == ON_CREATE_NEW_MODEL) {	// ���ȹ涨���Χ
		QMessageBox::information(NULL, "��ʾ", "����ȷ�����Χ���ٱ�Ǿֲ�����", QMessageBox::Yes);

		isOnLandMark = true;	// ��ʼ���
		ui.labelDisplay->installEventFilter(this);

		ui.buttonStartCircleROI->setText("�������");

		curState = ON_MAX_AREA_MARK;	// �����������
		updateStepOnPageCreateModel();
	}
	//else if (curState == ON_PART_AREA_MARK_SCALE){	// ������¼С����
	//	QMessageBox::information(NULL, "��ʾ", "����궨��ϣ������ø������������", QMessageBox::Yes);

	//	ui.buttonStartCircleROI->setText("��ʼ");

	//	// ����״̬
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
		case CircuitBoardInspection::ON_LOAD_OR_CREATE_MODEL: {	// ����ѡ���Ǽ�������ģ�ͻ����½�ģ����Ϣ
			updateHaveModel();
			ui.groupBoxLoadOrCreateModel->setEnabled(true);
			break;
		}
		case CircuitBoardInspection::ON_LOAD_MODEL: {	// ��������ģ��
			QMessageBox::information(NULL, "��ʾ", "ģ�ͼ��سɹ���", QMessageBox::Yes);
			// (TODO)Ŀǰֻ�����򵥵Ĺ��ܣ�ֱ�ӹر�
			//this->close();
			// ��ʱʧ�ܣ��������޸ı��
			ui.tableWidgetModelMsg->setEnabled(true);
			// ��ʼ���������ã�����ʾ�ڱ����
			initNewModelSettingParam();
			break;
		}
		case CircuitBoardInspection::ON_CREATE_NEW_MODEL: {	// �½�ģ��
			// ����mask�ļ���


			ui.groupBoxCircleROI->setEnabled(true);
			ui.buttonFinishCreateModel->setEnabled(true);

			ui.groupBoxLoadImage->setEnabled(false);
			ui.groupBoxLoadOrCreateModel->setEnabled(false);

			// �����Ǳ��ÿһ����Ĳ������֣�ȫ����ʼ��ʧ��
			ui.groupBoxDetectHaveOrNot->setDisabled(true);
			ui.groupBoxDetectionColor->setDisabled(true);
			ui.groupBoxDetectionCate->setDisabled(true);
			ui.groupBoxDetectionDirection->setDisabled(true);
			ui.groupBoxDetectionLength->setDisabled(true);
			ui.groupBoxDetectionPos->setDisabled(true);
			break;
		}
		case CircuitBoardInspection::ON_MAX_AREA_MARK: {	// ������Χ
			// �����������
			// ��������
			connect(ui.comboBoxAreaCate, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxAreaCate(int)));
			// ����
			btnGroupHaveOrNot = new QButtonGroup();
			btnGroupHaveOrNot->addButton(ui.radioButtonDetectionHaveOrNotOff, 0);	// set���޼��
			btnGroupHaveOrNot->addButton(ui.radioButtonDetectionHaveOrNotOn, 1);	// unset���޼��
			connect(btnGroupHaveOrNot, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupHaveOrNot(int)));
			// ��ɫ
			btnGroupColor = new QButtonGroup();
			btnGroupColor->addButton(ui.radioButtonDetectionColorOff, 0);	// set��ɫ���
			btnGroupColor->addButton(ui.radioButtonDetectionColorOn, 1);	// unset��ɫ���
			connect(btnGroupColor, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupColor(int)));
			connect(ui.comboBoxDetectionColorNormal, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxDetectionColorNormal(int)));
			// ����
			btnGroupCate = new QButtonGroup();
			btnGroupCate->addButton(ui.radioButtonDetectionCateOff, 0);	// set���ͼ��
			btnGroupCate->addButton(ui.radioButtonDetectionCateOn, 1);	// unset���ͼ��
			connect(btnGroupCate, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupCate(int)));
			// ����
			btnGroupDirection = new QButtonGroup();
			btnGroupDirection->addButton(ui.radioButtonDetectionDirectionOff, 0);	// set������
			btnGroupDirection->addButton(ui.radioButtonDetectionDirectionOn, 1);	// unset������
			connect(btnGroupDirection, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupDirection(int)));
			// λ��
			btnGroupPos = new QButtonGroup();
			btnGroupPos->addButton(ui.radioButtonDetectionPosOff, 0);	// setλ�ü��
			btnGroupPos->addButton(ui.radioButtonDetectionPosOn, 1);	// unsetλ�ü��
			connect(btnGroupPos, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupPos(int)));
			connect(ui.comboBoxStartID, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxStartID(int)));
			connect(ui.lineEditDetectionPosNormal, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionPosNormal()));
			connect(ui.lineEditDetectionPosOffset, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionPosOffset()));
			// ����
			btnGroupLength = new QButtonGroup();
			btnGroupLength->addButton(ui.radioButtonDetectionLengthOff, 0);	// set���ȼ��
			btnGroupLength->addButton(ui.radioButtonDetectionLengthOn, 1);	// unset���ȼ��
			connect(btnGroupLength, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupLength(int)));
			connect(ui.lineEditDetectionLengthNormal, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionLengthNormal()));
			connect(ui.lineEditDetectionLengthOffset, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionLengthOffset()));

			
			// ��Ӧlength�޸�
			connect(m_ProcessDisplay, SIGNAL(signalLengthParamModify(int, int, int, double)), this, SLOT(onLengthParamModify(int, int, int, double)));
			// ��Ӧblock�޸�
			connect(m_ProcessDisplay, SIGNAL(signalLandMarkChange(int, int, int, int, int)), this, SLOT(onLandMarkChangeFromProcessDisplay(int, int, int, int, int)));
			// ��Ӧϸ��targetѡȡ
			connect(m_ProcessDisplay, SIGNAL(signalTrueTargetPartAxis(int, int, int, int, int, int, int)), this, SLOT(onTrueTargetPartAxisModify(int, int, int, int, int, int, int)));
			// ��Ӧpos������Ϣ���
			connect(m_ProcessDisplay, SIGNAL(signalTwoTargetDistance(int, int, int)), this, SLOT(onTwoTargetDistance(int, int, int)));
			break;
		}

		//case CircuitBoardInspection::ON_COLOR_MARK: {
		//	// ui.groupBoxGetColor->setEnabled(true);
		//	break;
		//}
		case CircuitBoardInspection::ON_PART_AREA_MARK: {	// ��Ǿֲ�

			// ֮���ͼ���Ե�һ�α�����Χ��Ϊ��ʼͼ��
			matForDraw = matForDraw(cv::Range(landMark[0][1], landMark[0][3]),
				cv::Range(landMark[0][0], landMark[0][2]));
			frameSize = cv::Size(landMark[0][2] - landMark[0][0], landMark[0][3] - landMark[0][1]);
			// ����ͼ��ʵ�����ŵı���
			float ratioX = static_cast<float>(frameSize.width) / ui.labelDisplay->width();
			float ratioY = static_cast<float>(frameSize.height) / ui.labelDisplay->height();
			adaptRatio = MAX(ratioX, ratioY);	// �������ŵı���ȡС��
			offsetY = static_cast<int>((ui.labelDisplay->height() - (frameSize.height / adaptRatio)) / 2);

			// ������Ҫ��Ϊ��ȥ����ʾ��
			x1 = 27;
			y1 = 27;
			x2 = 27;
			y2 = 27;

			// ���ݸ��������ھֲ���ʾ
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
		case CircuitBoardInspection::ON_CREATE_NEW_MODEL_FINISH: {	// ģ���½�����
			ui.toolBox->setCurrentIndex(0);	// ��ת���������ò���һҳ

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
			// ��ȡ��������ģ��
			updateHaveModel();
			// �رջ���
			isOnLandMark = false;
			ui.groupBoxCircleROI->setEnabled(false);
			//m_ProcessDisplay->close();
			//delete m_ProcessDisplay;
			QMessageBox::information(NULL, "��ʾ", "������ģ�����!", QMessageBox::Yes);

			ui.buttonStartCircleROI->setText("��ʼ");
			break;
		}
		case CircuitBoardInspection::ON_SET_TABLE_PARAM: {	// ��������ϣ�����ÿ������Ĺ���
			

			//ui.toolBox->setCurrentIndex(1);	// ��ת���������ò���һҳ
			//ui.buttonModifyMsg->setEnabled(true);
			//ui.buttonDeleteMsg->setEnabled(true);
			//ui.buttonDeleteAllMsg->setEnabled(true);
			//ui.tableWidgetModelMsg->setEnabled(true);

			// ��ʼ���������ã�����ʾ�ڱ����
			//initNewModelSettingParam();

			//m_ProcessDisplay->setBlockMsg(landMark);
			//m_ProcessDisplay->setMatMsg(matForDraw);
			//m_ProcessDisplay->setParamMsg(vectParamForNewModel);
			//m_ProcessDisplay->show();
			//// ��Ӧblock�޸�
			//connect(m_ProcessDisplay, SIGNAL(signalLandMarkChange(int, int, int, int, int)), this, SLOT(onLandMarkChangeFromProcessDisplay(int, int, int, int, int)));
			//// ��Ӧlength�޸�
			//connect(m_ProcessDisplay, SIGNAL(signalLengthParamModify(int, int, int)), this, SLOT(onLengthParamModify(int, int, int)));

			//connect(ui.buttonFinishCreateModel, SIGNAL(released()), this, SLOT(onButtonFinishCreateModel()));

			//// ���ڴ�ų��ȼ�������
			//lengthProcessParam.resize(landMark.size());
			//for (int i = 0; i < lengthProcessParam.size(); ++i) {
			//	lengthProcessParam[i].resize(2, 0);
			//}

			break;
		}
		case CircuitBoardInspection::ON_SET_BLOCK_PARAM: {	// ��������ÿ����Ĺ���
			//ui.groupBoxDetectHaveOrNot->setEnabled(true);
			//ui.groupBoxDetectionColor->setEnabled(true);
			//ui.groupBoxDetectionCate->setEnabled(true);
			//ui.groupBoxDetectionDirection->setEnabled(true);
			//ui.groupBoxDetectionLength->setEnabled(true);

			//ui.groupBoxDetectionPos->setEnabled(true);

			// �����ʾ����
			//updateDisplayOnTableModelMsg(vectParamForNewModel);	// ����vect��ʼ����ʾ���
			// �Ϸ���ʾ����
			//updateDisplayOnPageParam(vectParamForNewModel);
			break;
		}
		
		default: {
			break;
		}
	}
}

void CircuitBoardInspection::onButtonConfirmMsg() {
	// ÿ��һ��block��ʧ�ܹ���ѡ�ֱ��ѡ�����������
	//ui.comboBoxAreaCate->setCurrentIndex(-1);
	ui.groupBoxDetectHaveOrNot->setDisabled(true);
	ui.groupBoxDetectionColor->setDisabled(true);
	ui.groupBoxDetectionCate->setDisabled(true);
	ui.groupBoxDetectionDirection->setDisabled(true);
	ui.groupBoxDetectionLength->setDisabled(true);
	ui.groupBoxDetectionPos->setDisabled(true);

	curState = ON_PART_AREA_MARK_SCALE;	// ģ�ͽ���
	updateStepOnPageCreateModel();
}

void CircuitBoardInspection::updateHaveModel() {	// ������ʾ���е�ģ��
	myModelsName.clear();
	myModelsName = mysql_conn->tab2Vector("model", 7);

	ui.comboBoxHaveModel->clear();
	for (int i = 0; i < myModelsName.size(); ++i) {
		ui.comboBoxHaveModel->addItem(QString::fromStdString(myModelsName[i][1]));
	}
}

// ��ģ����������
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
		// �����±�
		mysql_conn->detAndNewData("model", myModelsName);

		// ���������±�
		mysql_conn->createNewTabelPARAM(newParamTabelName.c_str());	
		mysql_conn->detAndNewData(newParamTabelName.c_str(), vectParamForNewModel);
		// ������block��
		mysql_conn->createNewTabelBLOCK(newBlockTabelName.c_str());
		mysql_conn->detAndNewDataWithID(newBlockTabelName.c_str(), landMark);
		// ������LengthParam��
		mysql_conn->createNewTabelLengthPixelParam(newLengthPixelParamTabelName.c_str());
		mysql_conn->detAndNewDataWithID(newLengthPixelParamTabelName.c_str(), lengthProcessParam);
		// ������TrueTargetAxis��
		mysql_conn->createNewTabelTrueTargetBlock(newTrueTargetBlockTabelName.c_str());
		mysql_conn->detAndNewDataWithID(newTrueTargetBlockTabelName.c_str(), trueTargetPartAxis);
		// �����µ�pospixel��
		mysql_conn->createNewTabelPosPixelParam(newPosPixelParamTabelName.c_str());
		mysql_conn->detAndNewDataWithID(newPosPixelParamTabelName.c_str(), posProcessParam);
		// �����µ�PlugDirectMaskMsg��
		mysql_conn->createNewTabelPlugDirectMaskMsg(newPlugDirectMaskMsgTableName.c_str());
		mysql_conn->detAndNewDataWithID(newPlugDirectMaskMsgTableName.c_str(), plugDirectMaskMsg);

		curState = ON_CREATE_NEW_MODEL_FINISH;	// ģ�ͽ���
		updateStepOnPageCreateModel();
	}
	else {
		QMessageBox::warning(NULL, "����", "ģ�����Ѿ����ڣ�", QMessageBox::Yes);
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
			// �Ŵ�
			if (curState == ON_PART_AREA_MARK) {	// ���һ�·Ŵ�ֲ�
				// ����ѡȡ�ľֲ��Ŵ�ͼ�񣬵õ��Ŵ���ʾ�ľ��ο�
				partRect.height = static_cast<int>(static_cast<float>(frameSize.height) / partScale);
				partRect.width = partRect.height;
				
				float ratioX = static_cast<float>(partRect.width) / ui.labelDisplay->width();
				float ratioY = static_cast<float>(partRect.height) / ui.labelDisplay->height();
				adaptRatio = MAX(ratioX, ratioY);	// �������ŵı���ȡС��
				offsetY = static_cast<int>((ui.labelDisplay->height() - (partRect.height / adaptRatio)) / 2);

				matForDrawPart = matForDraw(cv::Range(trueStartY, partRect.height), cv::Range(trueStartX, partRect.width));

				
			}
			else if (curState == ON_PART_AREA_MARK_SCALE) {	// �ƶ�
				x1 = 27;
				y1 = 27;
				x2 = 27;
				y2 = 27;
				partStartX = ev->pos().x();
				partStartY = ev->pos().y();
			}
			//else if (curState == ON_PART_AREA_MARK_SCALE) {	// ����ȥ��ʾ
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
			// ����Ǵ����������ϻ���Ļ����������������
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
				//		QMessageBox::information(NULL, "��ʾ", "δѡ��Ŀ����ɫ��", QMessageBox::Yes);
				//	}
				//	else {

				//	}
				//	return;
				//}

				if (!isOnColorMark) {
					landMarkSave();
					
					if (curState == ON_MAX_AREA_MARK) {	// ���˵�һ���Ǳ��һ�������򣬽��������Ǳ��С����
						//ui.toolBox->setCurrentIndex(1);	// ��ת���������ò���һҳ
						//
						//m_ProcessDisplay->show();

						ui.groupBoxGetColor->setEnabled(true);

						curState = ON_PART_AREA_MARK;
						updateStepOnPageCreateModel();

						isOnColorMark = true;
						// ���labelDisplayColorThrd�����¼���������������ͼ
						ui.labelDisplayColorThrd->installEventFilter(this);
						// ui.labelDisplay->removeEventFilter(this);
					}
					else {
						curState = ON_PART_PARAM_SET;	// ���þֲ�����
						updateStepOnPageCreateModel();
					}
					std::cout << "landMarkSave: " << landMark.size() << " vectParamForNewModel: " << vectParamForNewModel.size() << std::endl;
				}
				else {
					if (ui.comboBoxHaveColor->currentText() == "") {
						QMessageBox::warning(NULL, "����", "����ѡ��Ŀ����ɫ��", QMessageBox::Yes);
						return;
					}
					else {
						int startX = static_cast<int>((x1 - 27) * adaptRatio);
						int startY = static_cast<int>((y1 - 27 - offsetY) * adaptRatio);
						int endX = static_cast<int>((x2 - 27) * adaptRatio);
						int endY = static_cast<int>((y2 - 27 - offsetY) * adaptRatio);
						if (!ui.checkBoxColorMark->isChecked()) {	// û��ѡ�У����ڱ�ǽ׶�
							
							if (!isOnTargetColorAverage) {	// ˵�������ڱ���ⲿ��Χ�Ľ׶�
								isOnTargetColorAverage = true;	// ��һ���׶�
								colorMarkStartPoint = cv::Point(startX, startY);	// ��¼�ⲿ��Χ
								colorMarkEndPoint = cv::Point(endX, endY);

								matForColorTest = matForDrawPart(cv::Range(startY, endY), cv::Range(startX, endX));
								cv::Mat matForColorTestDisplay;
								cv::cvtColor(matForColorTest, matForColorTestDisplay, cv::COLOR_BGR2RGB);
								QImage imgForDrawQImage = QImage(matForColorTestDisplay.data, matForColorTestDisplay.cols, matForColorTestDisplay.rows, matForColorTestDisplay.step, QImage::Format_RGB888);
								ui.labelDisplayColorThrd->setPixmap(QPixmap::fromImage(imgForDrawQImage).scaled(ui.labelDisplayColorThrd->width(), ui.labelDisplayColorThrd->height(), Qt::KeepAspectRatio));
								// ����任
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
							else {	// ���ڼ���Ŀ����ɫ����ƽ��ֵ�Ľ׶�
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
						else {	// ���ڲ��Խ׶�
							if (isOnTargetColorAverage) isOnTargetColorAverage = false;	// ��֤��һ��һ���Ǵӱ����������ʼ

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
			x2 = ev->pos().x();	// (TODO)��Ҫ��������
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
				matForDraw.copyTo(matForDraw_cp);	// ����ͼƬ
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
			if(curState >= ON_PART_AREA_MARK_SCALE)	// �ֲ��Ѿ���ǹ���
				matForDrawPart.copyTo(mat_cp);	// ����ͼƬ
			else {
				matForDraw.copyTo(mat_cp);	// ����ͼƬ
				// ��ͼƬ�ͽ�ȥ��������(�����Ǳ�����Ŀ�)
				landMarkRun(mat_cp);
			}

			if (isOnTargetColorAverage) {	// ��ɫ�궨��ʱ�����ڻ�������
				cv::rectangle(mat_cp, colorMarkStartPoint,
					colorMarkEndPoint, CvScalar(0, 255, 0), 8, cv::LINE_8, 0);
			}

			//// ��ͼƬ�ͽ�ȥ��������(������ʵʱ�Ŀ�)�������������ͼ��ʵ����ֻ��������ȥ����Ϊû�п������������
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
	std::vector<int> _landmark = { x1 - 27, y1 - 27, x2 - 27, y2 - 27 };	// ���ȵõ��������������������ؼ�������

	_landmark[0] = static_cast<int>(_landmark[0] * adaptRatio + trueStartX);
	_landmark[1] = static_cast<int>((_landmark[1] - offsetY) * adaptRatio + trueStartY);
	_landmark[2] = static_cast<int>(_landmark[2] * adaptRatio + trueStartX);
	_landmark[3] = static_cast<int>((_landmark[3] - offsetY) * adaptRatio + trueStartY);

	landMark.push_back(_landmark);

	// ѹ��һ�в���
	std::vector<int> _vectParamForNewModel(14, 0);
	_vectParamForNewModel[0] = vectParamForNewModel.size();
	_vectParamForNewModel[1] = -1;
	vectParamForNewModel.push_back(_vectParamForNewModel);
	// ��ʾ�������һ��
	updateDisplayOnPageParam(vectParamForNewModel, vectParamForNewModel.size() - 1);

	// ����֮ǰ�Ѿ�������һ��������ʾ��matForDraw��ȥ
	m_ProcessDisplay->setBlockMsg(landMark);	// �����µ�������Ϣ��ʵ����ÿ��ֻʹ�������һ��
	if (landMark.size() > 1)	// ��һ����ȡ��ֻ��ʾ����ľֲ���Ϣ
		m_ProcessDisplay->updateDisplayByBlockIdx(landMark.size() - 1);
}

void CircuitBoardInspection::landMarkRun(cv::Mat &mat) {	// �ͽ�ȥ����
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

// ��ʼ����ʾ�������
void CircuitBoardInspection::initNewModelSettingParam() {
	//std::string curModel = ui.comboBoxHaveModel->currentText().toStdString();
	std::vector<std::vector<int>> vectParamReadByName;
	std::vector<std::vector<int>> landMarkReadByName;

	int curModelIdx = ui.comboBoxHaveModel->currentIndex();
	// ȡ������
	std::vector<std::vector<std::string>> landMarkReadByName_str = mysql_conn->tab2Vector(myModelsName[curModelIdx][3].c_str(), 5);
	for (int i = 0; i < landMarkReadByName_str.size(); ++i) {
		std::vector<int> tmp;
		for (int j = 1; j < 5; ++j) {
			tmp.push_back(atoi(landMarkReadByName_str[i][j].c_str()));
		}
		landMarkReadByName.push_back(tmp);
	}
	std::cout << "Get Load Model Block." << std::endl;
	// ȡ��param
	std::vector<std::vector<std::string>> vectParamReadByName_str = mysql_conn->tab2Vector(myModelsName[curModelIdx][2].c_str(), 14);
	for (int i = 0; i < vectParamReadByName_str.size(); ++i) {
		std::vector<int> tmp;
		for (int j = 0; j < 14; ++j) {
			tmp.push_back(atoi(vectParamReadByName_str[i][j].c_str()));
		}
		vectParamReadByName.push_back(tmp);
	}
	std::cout << "Get Load Model Param." << std::endl;
	// �����ʾ
	ui.tableWidgetModelMsg->setRowCount(vectParamReadByName.size() - 1);	// ��ʼ���������
	ui.tableWidgetModelMsg->setEditTriggers(QAbstractItemView::NoEditTriggers);	// ��ʼ�����ɱ༭
	updateDisplayOnTableModelMsg(vectParamReadByName);


	//vectParamForNewModel.resize(landMark.size());
	//for (int i = 0; i < vectParamForNewModel.size(); ++i)
	//	vectParamForNewModel[i].resize(14, 0);	// 14��
	//for (int i = 0; i < vectParamForNewModel.size(); ++i)
	//	vectParamForNewModel[i][0] = i;	// ��ʼ��������

	//ui.tableWidgetModelMsg->setRowCount(vectParamForNewModel.size() - 1);	// ��ʼ���������
	//ui.tableWidgetModelMsg->setEditTriggers(QAbstractItemView::NoEditTriggers);	// ��ʼ�����ɱ༭
	
	//updateDisplayOnTableModelMsg(vectParamForNewModel);	// ����vect��ʼ����ʾ���

	//// ��Ӧ������¼�����ȡ����Ϣ��Ȼ�󿪷�����ľ����������ʼ����
	//connect(ui.tableWidgetModelMsg, SIGNAL(cellClicked(int, int)), this, SLOT(onTableWidgetModelMsg(int, int)));

	//// �����������
	//// ��������
	//connect(ui.comboBoxAreaCate, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxAreaCate(int)));
	//// ����
	//btnGroupHaveOrNot = new QButtonGroup();
	//btnGroupHaveOrNot->addButton(ui.radioButtonDetectionHaveOrNotOff, 0);	// set���޼��
	//btnGroupHaveOrNot->addButton(ui.radioButtonDetectionHaveOrNotOn, 1);	// unset���޼��
	//connect(btnGroupHaveOrNot, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupHaveOrNot(int)));
	//// ��ɫ
	//btnGroupColor = new QButtonGroup();
	//btnGroupColor->addButton(ui.radioButtonDetectionColorOff, 0);	// set��ɫ���
	//btnGroupColor->addButton(ui.radioButtonDetectionColorOn, 1);	// unset��ɫ���
	//connect(btnGroupColor, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupColor(int)));
	//connect(ui.comboBoxDetectionColorNormal, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxDetectionColorNormal(int)));
	//// ����
	//btnGroupCate = new QButtonGroup();
	//btnGroupCate->addButton(ui.radioButtonDetectionCateOff, 0);	// set���ͼ��
	//btnGroupCate->addButton(ui.radioButtonDetectionCateOn, 1);	// unset���ͼ��
	//connect(btnGroupCate, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupCate(int)));
	//// ����
	//btnGroupDirection = new QButtonGroup();
	//btnGroupDirection->addButton(ui.radioButtonDetectionDirectionOff, 0);	// set������
	//btnGroupDirection->addButton(ui.radioButtonDetectionDirectionOn, 1);	// unset������
	//connect(btnGroupDirection, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupDirection(int)));
	//// λ��
	//btnGroupPos = new QButtonGroup();
	//btnGroupPos->addButton(ui.radioButtonDetectionPosOff, 0);	// setλ�ü��
	//btnGroupPos->addButton(ui.radioButtonDetectionPosOn, 1);	// unsetλ�ü��
	//connect(btnGroupPos, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupPos(int)));
	//connect(ui.comboBoxStartID, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxStartID(int)));
	//connect(ui.lineEditDetectionPosNormal, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionPosNormal()));
	//connect(ui.lineEditDetectionPosOffset, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionPosOffset()));
	//// ����
	//btnGroupLength = new QButtonGroup();
	//btnGroupLength->addButton(ui.radioButtonDetectionLengthOff, 0);	// setλ�ü��
	//btnGroupLength->addButton(ui.radioButtonDetectionLengthOn, 1);	// unsetλ�ü��
	//connect(btnGroupLength, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupLength(int)));
	//connect(ui.lineEditDetectionLengthNormal, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionLengthNormal()));
	//connect(ui.lineEditDetectionLengthOffset, SIGNAL(editingFinished()), this, SLOT(onLineEditDetectionLengthOffset()));
}

void CircuitBoardInspection::onComboBoxAreaCate(int idx) {
	disconnect(ui.comboBoxDetectionColorNormal, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxDetectionColorNormal(int)));
	vectParamForNewModel.back()[1] = idx;

	ui.comboBoxDetectionColorNormal->clear();
	// �����ѡ��ɫ
	switch (idx)
	{
	case 0: {	// ��������
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		break;
	}
	case 1: {	// T18R
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		break;
	}
	case 2: {	// ����
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		break;
	}
	case 3: {	// ��ͷ
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		ui.comboBoxDetectionColorNormal->addItem("��");
		break;
	}
	default:
		break;
	}
	ui.comboBoxDetectionColorNormal->setCurrentIndex(-1);

	// ��ͷ����
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
		// ÿ��ѡ����ɫ��ʱ�򣬸��ݵ�ǰ����ID����ɫID����һ��ϸ��Ŀ��������Ϣ
		m_ProcessDisplay->setParamMsg(vectParamForNewModel);	// ˢ�²���
		m_ProcessDisplay->updateAlgorithmDisplay(vectParamForNewModel.back()[1], vectParamForNewModel.back()[3], -1);	// (�ͺţ���ɫ, -1����ֻ��СĿ������)
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

void CircuitBoardInspection::onBtnGroupHaveOrNot(int idx) {	// ���޼��
	//std::cout << "onBtnGroupHaveOrNot: " << idx << std::endl;
	//vectParamForNewModel[curSetBlock + 1][11] = idx;
	vectParamForNewModel.back()[11] = idx;
}

void CircuitBoardInspection::onBtnGroupColor(int idx) {	// ��ɫ���
	//std::cout << "onBtnGroupColor: " << idx << std::endl;
	//vectParamForNewModel[curSetBlock + 1][2] = idx;
	vectParamForNewModel.back()[2] = idx;
}

void CircuitBoardInspection::onBtnGroupCate(int idx) {	// ���ͼ��
	//std::cout << "onBtnGroupCate: " << idx << std::endl;
	//vectParamForNewModel[curSetBlock + 1][12] = idx;
	if (idx == 1) {
		if (vectParamForNewModel.back()[1] != 0 && vectParamForNewModel.back()[1] != 1) {	// ���ͼ��ֻ�������
			ui.radioButtonDetectionCateOff->setChecked(true);
			QMessageBox::warning(NULL, "����", "���ͼ������������", QMessageBox::Yes);
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

void CircuitBoardInspection::onBtnGroupDirection(int idx) {	// ������
	//std::cout << "onBtnGroupDirection: " << idx << std::endl;
	//vectParamForNewModel[curSetBlock + 1][13] = idx;
	//vectParamForNewModel.back()[13] = idx;
	if (idx == 1) {
		if (vectParamForNewModel.back()[1] == 3) {	// ����ǲ�ͷ������Ļ�
			// ����Ҫѡ��mask����
			if (ui.comboBoxPlugMask->currentIndex() != -1) {	// 
				plugDirectMaskMsg.push_back({ vectParamForNewModel.back()[0],  ui.comboBoxPlugMask->currentIndex() });
				vectParamForNewModel.back()[13] = idx;
			}
			else {	// û��ѡ��mask
				ui.radioButtonDetectionDirectionOff->setChecked(true);
				QMessageBox::warning(NULL, "����", "��ͷ��������ѡ��Mask���ͣ�", QMessageBox::Yes);
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

void CircuitBoardInspection::onBtnGroupPos(int idx) {	// λ�ü��
	//std::cout << "onBtnGroupPos: " << idx << std::endl;
	//vectParamForNewModel[curSetBlock + 1][7] = idx;
	if (idx == 1) {
		if (vectParamForNewModel.back()[9] == 0 || vectParamForNewModel.back()[10] == 0) {
			ui.radioButtonDetectionPosOff->setChecked(true);
			QMessageBox::warning(NULL, "����", "������д������", QMessageBox::Yes);
			return;
		}
		else if (ui.comboBoxStartID->currentIndex() == -1) {
			ui.radioButtonDetectionPosOff->setChecked(true);
			QMessageBox::warning(NULL, "����", "��ѡ����ʼID��", QMessageBox::Yes);
			return;
		}
		else {
			vectParamForNewModel.back()[7] = idx;
			m_ProcessDisplay->setParamMsg(vectParamForNewModel);	// ˢ�²���
			m_ProcessDisplay->updateAlgorithmDisplay(vectParamForNewModel.back()[1], vectParamForNewModel.back()[3], 2);	// (�ͺţ���ɫ, λ�ü��)
		}
	}
	else {
		vectParamForNewModel.back()[7] = idx;
	}
}

void CircuitBoardInspection::onBtnGroupLength(int idx) {	// ���ȼ��
	//std::cout << "onBtnGroupLength: " << idx << std::endl;
	//vectParamForNewModel[curSetBlock + 1][4] = idx;
	//vectParamForNewModel.back()[4] = idx;
	if (idx == 1) {
		if (vectParamForNewModel.back()[5] == 0 || vectParamForNewModel.back()[6] == 0) {
			ui.radioButtonDetectionLengthOff->setChecked(true);
			QMessageBox::warning(NULL, "����", "������д������", QMessageBox::Yes);
			return;
		}
		else {
			if (vectParamForNewModel.back()[1] != 2){	// ֻ�н������Խ��г��ȼ��
				ui.radioButtonDetectionLengthOff->setChecked(true);
				QMessageBox::warning(NULL, "����", "���ȼ�����Խ�����", QMessageBox::Yes);
				return;
			}
			else {
				vectParamForNewModel.back()[4] = idx;
				// �Ѿ�������������Կ�ʼԤ�����ʾЧ��
				m_ProcessDisplay->setParamMsg(vectParamForNewModel);	// ˢ�²���
				m_ProcessDisplay->updateAlgorithmDisplay(vectParamForNewModel.back()[1], vectParamForNewModel.back()[3], 1);	// (�ͺţ���ɫ, ���ȼ��)
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
	if (cRow == 0)	// ��һ���޷�λ�ü��
		ui.groupBoxDetectionPos->setEnabled(false);

	//m_ProcessDisplay->updateDisplayByBlockIdx(cRow + 1);
}

void CircuitBoardInspection::updateDisplayOnPageParam(std::vector<std::vector<int>> &vect, int curIdx) {
	// ֻ��Ҫ��ʾĳһ�еĲ���
	// curSetBlock: ĳһ��
	// ��������
	ui.comboBoxAreaCate->setCurrentIndex(vect[curIdx][1]);
	// ���޼��
	if (vect[curIdx][11] == 0)
		ui.radioButtonDetectionHaveOrNotOff->setChecked(true);
	else
		ui.radioButtonDetectionHaveOrNotOn->setChecked(true);
	// ��ɫ���
	if (vect[curIdx][2] == 0)
		ui.radioButtonDetectionColorOff->setChecked(true);
	else
		ui.radioButtonDetectionColorOn->setChecked(true);
	if (ui.comboBoxDetectionColorNormal->count() == 0) {
		switch (vect[curIdx][1])
		{
		case 0: {	// ��������
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			break;
		}
		case 1: {	// T18R
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			break;
		}
		case 2: {	// ����
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			break;
		}
		case 3: {	// ��ͷ
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			ui.comboBoxDetectionColorNormal->addItem("��");
			break;
		}
		default:
			break;
		}
	}
	// ���õ�ǰ��ɫ����
	ui.comboBoxDetectionColorNormal->setCurrentIndex(vect[curIdx][3]);
	// ���ͼ��
	if (vect[curIdx][12] == 0)
		ui.radioButtonDetectionCateOff->setChecked(true);
	else
		ui.radioButtonDetectionCateOn->setChecked(true);
	// ������
	if (vect[curIdx][13] == 0)
		ui.radioButtonDetectionDirectionOff->setChecked(true);
	else
		ui.radioButtonDetectionDirectionOn->setChecked(true);
	// λ�ü��
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
	// ���ȼ��
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

//// ����ÿ��block�����ͣ�������һ����ص��㷨��⣬����ʾ
//void CircuitBoardInspection::updateAlgorithmDisplay(int cateIdx, int detectIdx) {
//	// �����ѡ��ɫ
//	// detectIdx: 0		��ɫ
//	// detectIdx: 1		����
//	// detectIdx: 2		λ��
//	// detectIdx: 3		����
//	// detectIdx: 4		����
//	// detectIdx: 5		����
//	switch (cateIdx)
//	{
//		case 0: {	// ��������
//
//			break;
//		}
//		case 1: {	// T18R
//
//			break;
//		}
//		case 2: {	// ����
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
//		case 3: {	// ��ͷ
//
//			break;
//		}
//		default:
//			break;
//	}
//}

void CircuitBoardInspection::updateDisplayOnTableModelMsg(std::vector<std::vector<int>> &vect) {
	for (int i = 1; i < vect.size(); ++i) {	// ��һ�д���������Χ�������ھֲ�
		for (int j = 0; j < vect[i].size(); ++j) {
			// [1]������ : (int)
			// [2]�������� : ��������(0) / T18R(1) / ����(2) / ��ͷ(3)
			// [3]��ɫ��� : ��(0) / ��(1)
			// [4]��ɫ���ֵ :	1.����(��������[��(0)����(1)����(2)] / T18R[��(0)����(1)����(2)])	// 6��
			//					2.����(��(0)����(1)����(2)����(3)����(4)����(5))		// 6��
			//					3.��ͷ(��(0)����(1)����(2)����(3)����(4)����(5)����(6))	// 7��
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
			switch (j + 1)
			{
				case 1: {	// ������
					ui.tableWidgetModelMsg->setItem(i - 1, j, new QTableWidgetItem(QString::fromStdString(std::to_string(vect[i][0]))));
					ui.tableWidgetModelMsg->item(i - 1, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);	// ���õ�Ԫ�����
					break;
				}
				case 2: {	// ��������
					// ���Ȳ���box
					QComboBox *cbBox = new QComboBox();
					cbBox->addItem("��������");
					cbBox->addItem("T18R");
					cbBox->addItem("����");
					cbBox->addItem("��ͷ");
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// ���ݵ�ǰ��������Index
					cbBox->setCurrentIndex(vect[i][1]);
					break;
				}
				case 3: {	// ��ɫ���
					QComboBox *cbBox = new QComboBox();
					cbBox->addItem("��");
					cbBox->addItem("��");
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// ���ݵ�ǰ��������Index
					cbBox->setCurrentIndex(vect[i][2]);
					break;
				}
				case 4: {	// ��ɫ���ֵ
					ui.tableWidgetModelMsg->removeCellWidget(i - 1, j);
					QComboBox *cbBox = new QComboBox();
					switch (vect[i][1])
					{
						case 0: {	// ��������
							cbBox->addItem("��");
							cbBox->addItem("��");
							cbBox->addItem("��");
							break;
						}
						case 1: {	// T18R
							cbBox->addItem("��");
							cbBox->addItem("��");
							cbBox->addItem("��");
							break;
						}
						case 2: {	// ����
							cbBox->addItem("��");
							cbBox->addItem("��");
							cbBox->addItem("��");
							cbBox->addItem("��");
							cbBox->addItem("��");
							cbBox->addItem("��");
							break;
						}
						case 3: {	// ��ͷ
							cbBox->addItem("��");
							cbBox->addItem("��");
							cbBox->addItem("��");
							cbBox->addItem("��");
							cbBox->addItem("��");
							cbBox->addItem("��");
							cbBox->addItem("��");
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
					// ���ݵ�ǰ��������Index
					if (vect[i][2] == 0)	// ��������Ļ���ʧ��
						cbBox->setDisabled(true);
					cbBox->setCurrentIndex(vect[i][3]);
					break;
				}
				case 5: {	// �������Ƽ��
					QComboBox *cbBox = new QComboBox();
					cbBox->addItem("��");
					cbBox->addItem("��");
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// ���ݵ�ǰ��������Index
					cbBox->setCurrentIndex(vect[i][4]);
					break;
				}
				case 6: {	// ���ȹ��ֵ
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
					if (vect[i][4] == 0)	// ��������Ļ���ʧ��
						lb->setDisabled(true);
					break;
				}
				case 7: {	// ����ƫ��ֵ
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
					if (vect[i][4] == 0)	// ��������Ļ���ʧ��
						lb->setDisabled(true);
					break;
				}
				case 8: {	// λ�ü��
					if (i == 1)
						continue;
					QComboBox *cbBox = new QComboBox();
					cbBox->addItem("��");
					cbBox->addItem("��");
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// ���ݵ�ǰ��������Index
					cbBox->setCurrentIndex(vect[i][7]);
					break;
				}
				case 9: {	// ��ʼID
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
					if (vect[i][7] == 0)	// ��������Ļ���ʧ��
						lb->setDisabled(true);
					//if (vect[i][7] == 0)	// ��������Ļ���ʧ��
					//	cbBox->setDisabled(true);
					// ���ݵ�ǰ��������Index
					//cbBox->setCurrentIndex(vect[i][8]);
					break;
				}
				case 10: {	// λ�ù��ֵ
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
					if (vect[i][7] == 0)	// ��������Ļ���ʧ��
						lb->setDisabled(true);
					break;
				}
				case 11: {	// λ��ƫ��ֵ
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
					if (vect[i][7] == 0)	// ��������Ļ���ʧ��
						lb->setDisabled(true);
					break;
				}
				case 12: {	// ���޼��
					QComboBox *cbBox = new QComboBox();
					cbBox->addItem("��");
					cbBox->addItem("��");
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// ���ݵ�ǰ��������Index
					cbBox->setCurrentIndex(vect[i][11]);
					break;
				}
				case 13: {	// ���ͼ��
					QComboBox *cbBox = new QComboBox();
					cbBox->addItem("��");
					cbBox->addItem("��");
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// ���ݵ�ǰ��������Index
					cbBox->setCurrentIndex(vect[i][12]);
					break;
				}
				case 14: {	// ������
					QComboBox *cbBox = new QComboBox();
					cbBox->addItem("��");
					cbBox->addItem("��");
					QWidget *cbBoxWidget = new QWidget();
					QHBoxLayout *cbBoxLayout = new QHBoxLayout();
					cbBoxLayout->setMargin(2);
					cbBoxLayout->addWidget(cbBox);
					cbBoxLayout->setAlignment(Qt::AlignHCenter);
					cbBoxWidget->setLayout(cbBoxLayout);
					ui.tableWidgetModelMsg->setCellWidget(i - 1, j, cbBoxWidget);
					// ���ݵ�ǰ��������Index
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
		pDataMat = dstChl.ptr<uchar>(j);//ptr<>()�õ�����һ��ָ��
		for (int i = 0; i < nCols; i++)
		{
			if (pDataMat[i] > pixMax)
				pixMax = pDataMat[i];
			if (pDataMat[i] < pixMin)
				pixMin = pDataMat[i];
		}
	}
	pixMin = 0;
	//�Աȶ�����ӳ��
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