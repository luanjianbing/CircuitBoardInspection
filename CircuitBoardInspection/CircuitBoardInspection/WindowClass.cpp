#pragma execution_character_set("utf-8")
#include "WindowClass.h"

WindowClass::WindowClass(MyCamera *myCamera, MysqlSetting *mysql_conn, QMainWindow *parent)
	: myCamera(myCamera), mysql_conn(mysql_conn), QMainWindow(parent)
{
	ui.setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose, true); 
	setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint
		&~Qt::WindowCloseButtonHint);
	setFixedSize(this->width(), this->height());
	this->showFullScreen();
	//setWindowFlags(Qt::FramelessWindowHint);

	// actionClose
	connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(onActionClose()));

	// actionRun
	connect(ui.actionRun, SIGNAL(triggered()), this, SLOT(onActionRun()));
	ui.actionRun->setEnabled(false);

	// actionEnlarge
	connect(ui.actionEnlarge, SIGNAL(triggered()), this, SLOT(onActionEnlarge()));

	// actionImageSetting
	connect(ui.actionImageSetting, SIGNAL(triggered()), this, SLOT(onActionImageSetting()));

	// actionStatistics
	connect(ui.actionStatistics, SIGNAL(triggered()), this, SLOT(onActionStatistics()));

	// ����ʵʱ��ʾ���߳�
	m_realTimeGrabThread = new ThreadForRealTimeGrab(myCamera);
	connect(m_realTimeGrabThread, SIGNAL(signalGrabFrameOver()), SLOT(dealSignalGrabFrameOver()));
	// ��������ⲿ�ź�
	m_listenExternalSignal = new ThreadForListenExternalSignal();
	connect(m_listenExternalSignal, SIGNAL(signalGetExternalSignal()), SLOT(dealSignalGetExternalSignal()));

	// �ⲿ�ź�
	//connect(ui.buttonRun, SIGNAL(released()), this, SLOT(onButtonRun()));
	connect(ui.actionSignal, SIGNAL(triggered()), this, SLOT(onButtonRun()));
	ui.actionSignal->setEnabled(false);

	// ����
	if (!RS232Init()) {
		QMessageBox::warning(NULL, "����", "���ڳ�ʼ��ʧ�ܣ�", QMessageBox::Yes);
		//exit(0);
	}
	else {
		connect(m_serial, SIGNAL(readyRead()), this, SLOT(portReadyRead()));
		if (m_serialTimer == NULL) {
			m_serialTimer = new QTimer();
			m_serialTimer->setInterval(5);	// interval for timer
			connect(m_serialTimer, SIGNAL(timeout()), this, SLOT(onPortSendSingleChar()));
		}
	}

	// ״̬��
	ui.statusbar->showMessage("SINGLE STATION VISUAL INSPECTION SYSTEM VERSION @202105");

	// �˵���
	ui.menubar->setVisible(false);
}

bool isFullScreenFlag = false;
void WindowClass::onActionEnlarge() {
	if (!isFullScreenFlag) {
		ui.actionEnlarge->setIcon(QIcon(".\\icon\\Narrow.png"));
		this->setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint
			&~Qt::WindowCloseButtonHint);
		this->showFullScreen();
	}
	else {
		ui.actionEnlarge->setIcon(QIcon(".\\icon\\Enlarge.png"));
		this->setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint
			&~Qt::WindowCloseButtonHint);
		this->showNormal();
	}
	isFullScreenFlag = !isFullScreenFlag;
}

void WindowClass::onActionClose() {
	this->close();
}

void WindowClass::onPortSendSingleChar() {
	//char ch = targetChar;
	char ch = 'A';
	m_serial->write(&ch, 1);
}

void WindowClass::portReadyRead() {
	QByteArray buf;
	buf = m_serial->readAll();
	if (!buf.isEmpty()) {
		std::string ch = buf.toStdString();
		if (ch != lastPortGetChar) {
			if (ch == "A") {
				// std::cout << "[" << lastPortGetChar << "]->[" << ch << "]" << std::endl;
				this->onButtonRun();
			}
			lastPortGetChar = ch;
		}
	}
	buf.clear();
}

bool WindowClass::RS232Init() {
	m_serial = new QSerialPort();
	// ��ȡ�����õĴ���
	QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();
	std::cout << "Ports Num is [" << serialPortInfos.count() << "] ";
	if (serialPortInfos.count() < 1) {
		std::cout << "Port Count Error." << std::endl;
		return false;
	}
	for (const QSerialPortInfo &serialPortInfo : serialPortInfos)
	{
		if ("EIASF103Y23" == serialPortInfo.serialNumber()) {
			// std::cout << "Start Open Port." << std::endl;
			//���ô�������
			m_serial->setPortName(serialPortInfo.portName());	// ���ﴮ�����ֱ�����"COM?"
																//���ò�����
			m_serial->setBaudRate(QSerialPort::Baud9600);
			//��������λ
			m_serial->setDataBits(QSerialPort::Data8);
			//������żУ��λ
			m_serial->setParity(QSerialPort::NoParity);
			//����ֹͣλ
			m_serial->setStopBits(QSerialPort::OneStop);
			//��������
			m_serial->setFlowControl(QSerialPort::NoFlowControl);
			if (!m_serial->open(QIODevice::ReadWrite)) {
				std::cout << "Open Port Error." << std::endl;
				return false;
			}
			else {
				std::cout << "Open Port Success." << std::endl;
				break;
			}
		}
	}
	return true;
}

bool WindowClass::RS232Close() {
	try
	{
		m_serial->close();
	}
	catch (const std::exception &e)
	{
		std::cout << "Close Port Error[" << e.what() << "]" << std::endl;
		return false;
	}
	std::cout << "Close Port." << std::endl;
	return true;
}

void WindowClass::onButtonRun() {	// ����һ��ģ���ⲿ�����ź�
	m_listenExternalSignal->setSignal(true);
}

void WindowClass::onActionStatistics() {
	Statistics *statistics = new Statistics(mysql_conn);
	// �����Ӵ������ԣ����Ӵ��ڹر�֮���ͷ��Ӵ��ڵ���Դ(�ͷ�ָ��)
	statistics->setAttribute(Qt::WA_DeleteOnClose);	// ����ͷ�ָ��ᱨ��
	statistics->show();
}

void WindowClass::onActionImageSetting() {
	CircuitBoardInspection *imageSetting = new CircuitBoardInspection(myCamera, mysql_conn);
	connect(imageSetting, SIGNAL(signalLoadModel(QString)), this, SLOT(dealSignalLoadModel(QString)));
	// �����Ӵ������ԣ����Ӵ��ڹر�֮���ͷ��Ӵ��ڵ���Դ(�ͷ�ָ��)
	//imageSetting->setAttribute(Qt::WA_DeleteOnClose);	// ����ͷ�ָ��ᱨ��
	imageSetting->show();
}

void WindowClass::dealSignalLoadModel(QString loadModel) {
	ui.actionRun->setEnabled(true);

	curRunModel = loadModel.toStdString();
	ui.labelModelName->setText(QString::fromStdString(curRunModel));
	std::cout << "curRunModel : " << curRunModel << std::endl;

	// ��׼��ɫ������Ϣ
	mysql_conn->readColorHSVParam(HSVColorForGetTrueTarget);
	// ��ȡģ�͵�����
	landMarkWithId.clear();
	std::string modelBlockTable = curRunModel + "_block_table";
	std::vector<std::vector<std::string>> landMarkWithId_str = mysql_conn->tab2Vector(modelBlockTable.c_str(), 5);
	for (int i = 0; i < landMarkWithId_str.size(); ++i) {
		std::vector<int> tmp;
		for (int j = 1; j < 5; ++j) {
			tmp.push_back(atoi(landMarkWithId_str[i][j].c_str()));
		}
		landMarkWithId.push_back(tmp);
	}
	// ������Ϣ
	vectParamForModel.clear();
	std::string modelParamTable = curRunModel + "_param_table";
	std::vector<std::vector<std::string>> vectParamForModel_str = mysql_conn->tab2Vector(modelParamTable.c_str(), 14);
	for (int i = 0; i < landMarkWithId_str.size(); ++i) {
		std::vector<int> tmp;
		for (int j = 0; j < 14; ++j) {
			tmp.push_back(atoi(vectParamForModel_str[i][j].c_str()));
		}
		vectParamForModel.push_back(tmp);
	}
	// �������ȼ���׼ֵ��ƫ����Ƕ�
	lengthPixel.clear();
	std::string modelLengthPixelTable = curRunModel + "_length_pixel_table";
	std::vector<std::vector<std::string>> lengthPixel_str = mysql_conn->tab2Vector(modelLengthPixelTable.c_str(), 4);
	for (int i = 0; i < lengthPixel_str.size(); ++i) {
		std::vector<std::string> tmp;
		for (int j = 1; j < 4; ++j) {
			tmp.push_back(lengthPixel_str[i][j]);
		}
		lengthPixel.push_back(tmp);
	}
	// ����Ŀ�����������Ϣ
	trueTargetPartAxis.clear();
	std::string modelTrueTargetBlockTable = curRunModel + "_true_target_block_table";
	std::vector<std::vector<std::string>> trueTargetPartAxis_str = mysql_conn->tab2Vector(modelTrueTargetBlockTable.c_str(), 7);
	for (int i = 0; i < trueTargetPartAxis_str.size(); ++i) {
		std::vector<int> tmp;
		for (int j = 1; j < 7; ++j) {
			tmp.push_back(atoi(trueTargetPartAxis_str[i][j].c_str()));
		}
		trueTargetPartAxis.push_back(tmp);
	}
	// λ�ü�����ر�׼ֵ��ƫ��
	posProcessParam.clear();
	std::string modelPosPixelTable = curRunModel + "_pos_pixel_table";
	std::vector<std::vector<std::string>> posProcessParam_str = mysql_conn->tab2Vector(modelPosPixelTable.c_str(), 3);
	for (int i = 0; i < posProcessParam_str.size(); ++i) {
		std::vector<int> tmp;
		for (int j = 1; j < 3; ++j) {
			tmp.push_back(atoi(posProcessParam_str[i][j].c_str()));
		}
		posProcessParam.push_back(tmp);
	}
	// ��ͷ������
	plugDirectMaskMsg.clear();
	std::string modelplugDirectMaskMsgTable = curRunModel + "_plug_direction_table";
	std::vector<std::vector<std::string>> plugDirectMaskMsg_str = mysql_conn->tab2Vector(modelplugDirectMaskMsgTable.c_str(), 3);
	for (int i = 0; i < plugDirectMaskMsg_str.size(); ++i) {
		std::vector<int> tmp;
		for (int j = 1; j < 3; ++j) {
			tmp.push_back(atoi(plugDirectMaskMsg_str[i][j].c_str()));
		}
		plugDirectMaskMsg.push_back(tmp);
	}

	// ��ȡĿ��block��mask 
	maskForTargetBlock.clear();
	maskForTargetBlock.push_back(cv::Mat());	// 0��λ������
	for (int i = 1; i < landMarkWithId.size(); ++i) {
		std::string thisPath = ".\\mask\\" + curRunModel + "\\" + std::to_string(i) + ".jpg";
		maskForTargetBlock.push_back(cv::imread(thisPath));
	}
	// ��ȡ����Ŀ��block��ģ��
	multiTempMat.clear();
	std::string dirPath = ".\\multiTemplate\\" + curRunModel + "\\";	// ���浱ǰģ�Ͷ�ģ����ļ���
	std::string searchPath = dirPath + "*.jpg";
	intptr_t handle;
	struct _finddata_t fileInfo;
	handle = _findfirst(searchPath.c_str(), &fileInfo);
	if (handle == -1)
		// assert("Get Current Model's MultiTemplate Error.");
		std::cout << "None multiTemplate." << std::endl;
	else {
		do {
			std::string tempImageName = fileInfo.name;
			int lastDownLine = tempImageName.find_last_of('_');
			int lastSecondDownLine = tempImageName.substr(0, lastDownLine).find_last_of('_');
			int blockIdx = atoi(tempImageName.substr(lastSecondDownLine + 1, lastDownLine).c_str());	// ��ͼ���ǵڼ���block��
			// std::cout << lastDownLine << " " << lastSecondDownLine << " " << blockIdx << std::endl;
			// ��ȡ�����ͼ��
			std::string thisImagePath = dirPath + tempImageName;
			// std::cout << thisImagePath << std::endl;
			cv::Mat tmpMat = cv::imread(thisImagePath);
			if (!multiTempMat.count(blockIdx)) {	// ��ǰidx�в��ڽṹ��
				std::list<cv::Mat> tmpList;
				tmpList.push_back(tmpMat);
				multiTempMat.insert(std::make_pair(blockIdx, tmpList));
			}
			else {	// ��ǰidx�Ѿ���ģ��¼��
				// ֱ�Ӳ��뵽list��
				multiTempMat[blockIdx].push_back(tmpMat);
			}
		} while (!_findnext(handle, &fileInfo));
	}

	/************************************************************/
	cv::Mat singleMat(myCamera->getCameraResolution(), CV_8UC3);
	myCamera->getOneFrame(singleMat);
	//singleMat = cv::imread("./test/norm.jpg");	// ���Ա�׼ͼ��
	/************************************************************/

	// ��ȡĿ�����ROI
	
	singleMat = singleMat(cv::Range(landMarkWithId[0][1], landMarkWithId[0][3]),
		cv::Range(landMarkWithId[0][0], landMarkWithId[0][2]));
	//dstMaxRoi = dstMaxRoi(cv::Rect(landMarkWithId[0][0], landMarkWithId[0][1], (landMarkWithId[0][2] - landMarkWithId[0][0]), (landMarkWithId[0][3] - landMarkWithId[0][1])));
	
	// ��������߶�
	mCameraWidth = singleMat.cols;	// ���澭�����Χ�궨��Ŀ��
	mCameraHeight = singleMat.rows;	// ���澭�����Χ�궨��ĸ߶�
	matForMutiTemp = cv::Mat::zeros(mCameraHeight, mCameraWidth, CV_8UC3);

	//// ����һ����׼ͼ��
	////normTemp = singleMat.clone();
	////cv::imwrite("norm.jpg", singleMat);
	//normTemp = cv::imread("norm.jpg");
	QMessageBox::StandardButton rs = QMessageBox::information(NULL, "��ʾ", "�Ƿ񱣴浱ǰͼ��Ϊ��׼ͼ��(norm.jpg)", QMessageBox::Yes | QMessageBox::No);
	if (rs == QMessageBox::Yes) {	// ����Ϊ��׼ͼ��
		normTemp = singleMat.clone();
		cv::imwrite("norm.jpg", singleMat);
	}
	else {	// ֱ�Ӵӱ��ض�ȡ
		normTemp = cv::imread("norm.jpg");
	}

	for (int i = 1; i < landMarkWithId.size(); i++) {

		cv::rectangle(singleMat, cv::Point(landMarkWithId[i][0], landMarkWithId[i][1]),
			cv::Point(landMarkWithId[i][2], landMarkWithId[i][3]), CvScalar(0, 170, 0), 8, cv::LINE_8, 0);

		cv::Size fontSize = cv::getTextSize(std::to_string(i), cv::FONT_HERSHEY_SIMPLEX, 4, 6, 0);
		cv::rectangle(singleMat, cv::Point(landMarkWithId[i][0], landMarkWithId[i][1] - fontSize.height),
			cv::Point(landMarkWithId[i][0] + fontSize.width, landMarkWithId[i][1]), CvScalar(0, 170, 0), cv::FILLED);
		cv::putText(singleMat, std::to_string(i), cv::Point(landMarkWithId[i][0], landMarkWithId[i][1]), cv::FONT_HERSHEY_SIMPLEX, 4, (0, 0, 0), 6);
	}

	cv::Mat dstMaxRoi;
	singleMat.copyTo(dstMaxRoi);
	cv::cvtColor(dstMaxRoi, dstMaxRoi, cv::COLOR_BGR2RGB);
	QImage singleQImage(dstMaxRoi.data, dstMaxRoi.cols, dstMaxRoi.rows, dstMaxRoi.cols*dstMaxRoi.channels(), QImage::Format_RGB888);
	//singleQImage.save("test2.jpg");
	ui.labelDisplay->setPixmap(QPixmap::fromImage(singleQImage).
		scaled(ui.labelDisplay->width(), ui.labelDisplay->height(), Qt::KeepAspectRatio));

	// ��������
	imageProcess = new ImageProcess(landMarkWithId, vectParamForModel, lengthPixel, trueTargetPartAxis, posProcessParam, plugDirectMaskMsg, normTemp);
	imageProcess->setHSVColorForGetTrueTarget(HSVColorForGetTrueTarget);	// ������ɫ��Ϣ
	// ����mask��Ϣ
	imageProcess->setMaskForTargetBlock(maskForTargetBlock);
	// ����multiTemp��Ϣ
	imageProcess->setMultiTempForTargetBlock(multiTempMat);
	// ��׼һ�£��Ժ��һ��
	imageProcess->matchTwoImage(singleMat, singleMat);

	// ״̬��
	QString tmpMsg = "LOAD MODEL " + QString::fromStdString(curRunModel);
	ui.statusbar->showMessage(tmpMsg);
}

void WindowClass::onActionRun() {
	// (TODO)����һ���̸߳���̬��ʾ����ʱץ����ʾ(���̱߳��浽���أ������źŵ������棬�������ȡ��ʾ)
	if (!realTimeGrab) {
		if (m_serialTimer->isActive() == false) {	// �������ڷ���
			m_serialTimer->start();
		}

		// ����ʵʱ��ʾ���߳�		
		m_realTimeGrabThread->start();

		// ��������ⲿ�źŵ��߳�
		m_listenExternalSignal->start();

		ui.actionSignal->setEnabled(true);

		ui.actionRun->setText("ֹͣ");
		ui.actionRun->setIcon(QIcon(".\\icon\\Stop.png"));

		ui.statusbar->showMessage("Start.");

		realTimeGrab = !realTimeGrab;
	}
	else {
		if (m_serialTimer->isActive() == true) {	// �رմ��ڷ���
			m_serialTimer->stop();
		}

		// ����ʵʱ��ʾ���߳�	
		m_realTimeGrabThread->stop();
		// ��������ⲿ�źŵ��߳�
		m_listenExternalSignal->stop();

		ui.actionSignal->setEnabled(false);

		ui.actionRun->setText("����");
		ui.actionRun->setIcon(QIcon(".\\icon\\Run.png"));

		ui.statusbar->showMessage("Stop.");

		realTimeGrab = !realTimeGrab;
	}

	// (TODO)������ν���ʮ�����������Ƿ���Ҫ����ʹ���̳߳أ����������ͨ����Ӧ�ⲿ�ź�ץ��ͼ��
	// 1. �����ⲿ�źţ��Ƿ�ʼץ��ͼ�񣬿�ʼ��⣩(TODO��μ���������ʱ������һ������ϵ�buttonʵ��ģ��)	// ����������ⲿ�źŵĻ�����ҲҪ��һ���̸߳�������ⲿ����
	// 2. �����յ��źŵ�ʱ�����̳߳�����������񣬵�������ɺ󣬷��ؽ����������ô֪������Ľ�����̳߳���δ�������֮��������̣߳���������ܻ���Ҫ��һ���̸߳�������̳߳��е��������


}

void WindowClass::dealSignalGrabFrameOver() {
	std::cout << "real time mat show once" << std::endl;
	//cv::Mat realTimeMat(myCamera->getCameraResolution(), CV_8UC3);
	//if (myCamera->getOneFrame(realTimeMat)) {
	//	cv::cvtColor(realTimeMat, realTimeMat, cv::COLOR_BGR2RGB);
	//	QImage singleQImage(realTimeMat.data, realTimeMat.cols, realTimeMat.rows, realTimeMat.cols*realTimeMat.channels(), QImage::Format_RGB888);
	//	ui.labelDisplayRealTime->setPixmap(QPixmap::fromImage(singleQImage).
	//		scaled(ui.labelDisplay->width(), ui.labelDisplay->height(), Qt::KeepAspectRatio));
	//}
	cv::Mat realTimeMat = cv::imread("./realTime.jpg");
	cv::cvtColor(realTimeMat, realTimeMat, cv::COLOR_BGR2RGB);
	QImage singleQImage(realTimeMat.data, realTimeMat.cols, realTimeMat.rows, realTimeMat.cols*realTimeMat.channels(), QImage::Format_RGB888);
	ui.labelDisplayRealTime->setPixmap(QPixmap::fromImage(singleQImage).
		scaled(ui.labelDisplay->width(), ui.labelDisplay->height(), Qt::KeepAspectRatio));
}

void WindowClass::dealSignalGetExternalSignal() {
	
	// ���յ��ⲿ�ź�
	std::cout << "get external signal" << std::endl;
	// �ܵĴ���+1
	totalCounts += 1;

	// (TODO)�ر���Ӧ���˫���¼�
	canGetDoubleClickedSignal = false;

	// (TODO)�費��Ҫ�����̳߳أ�
	// ���ʹ�����ץ�ĵĻ���������Ҫ��ʵʱ��ʾ�Ǹ��̹߳ص�ץ�����ٴ�
	m_realTimeGrabThread->stop();
	m_listenExternalSignal->stop();
	// �����Ȳ������̳߳ص����⣬�Ȱ�˳�������ǵ�������м��
	/*********************************************************************/
	cv::Mat singleMat(myCamera->getCameraResolution(), CV_8UC3);
	myCamera->getOneFrame(singleMat);
	// singleMat = cv::imread("./test/Error1.jpg");	// ����ͼ��
	// singleMat = cv::imread("./test/Error2.jpg");	// ����ͼ��
	// singleMat = cv::imread("./test/Error3.jpg");	// ����ͼ��

	//singleMat = cv::imread("./test/Error1_1.jpg");	// ����ͼ��Error1_1
	//singleMat = cv::imread("./test/Error1_2.jpg");	// ����ͼ��Error1_2
	//singleMat = cv::imread("./test/Error1_3.jpg");	// ����ͼ��Error1_3
	//singleMat = cv::imread("./test/Error1_4.jpg");	// ����ͼ��Error1_4
	//singleMat = cv::imread("./test/Error1_5.jpg");	// ����ͼ��Error1_5
	//singleMat = cv::imread("./test/Error2_1.jpg");	// ����ͼ��Error2_1
	//singleMat = cv::imread("./test/Error2_2.jpg");	// ����ͼ��Error2_2
	//singleMat = cv::imread("./test/Error2_3.jpg");	// ����ͼ��Error2_3
	//singleMat = cv::imread("./test/Error2_4.jpg");	// ����ͼ��Error2_4
	//singleMat = cv::imread("./test/Error3_1.jpg");	// ����ͼ��Error3_1
	//singleMat = cv::imread("./test/Error3_2.jpg");	// ����ͼ��Error3_2
	//singleMat = cv::imread("./test/Error3_3.jpg");	// ����ͼ��Error3_3
	//singleMat = cv::imread("./test/Error3_4.jpg");	// ����ͼ��Error3_4
	//singleMat = cv::imread("./test/Error3_5.jpg");	// ����ͼ��Error3_5

	/*********************************************************************/
	if (singleMat.empty())
		assert("Get input Image Error.");
	
	singleMat = singleMat(cv::Range(landMarkWithId[0][1], landMarkWithId[0][3]),
		cv::Range(landMarkWithId[0][0], landMarkWithId[0][2]));
	cv::Mat dstMaxRoi;
	singleMat.copyTo(dstMaxRoi);
	singleMat.copyTo(matForMutiTemp);	// ���ڶ�ģ�屣��
	//cv::imwrite("./tmp.jpg", dstMaxRoi);
	std::vector<int> res = imageProcess->blockProcessing(dstMaxRoi);

	// ��ʾ����
	updateDisplay(dstMaxRoi, res);
	// (TODO)������Ӧ���˫���¼�
	canGetDoubleClickedSignal = true;

	m_listenExternalSignal->start();
	m_realTimeGrabThread->start();

	//// ״̬��
	//ui.statusbar->showMessage("FREE");
}

void WindowClass::mouseDoubleClickEvent(QMouseEvent *ev) {
	if (ev->buttons() & Qt::LeftButton) {
		int m_x = QCursor::pos().x();
		int m_y = QCursor::pos().y();
		QWidget *w_obj = QApplication::widgetAt(m_x, m_y);
		if (canGetDoubleClickedSignal && w_obj->objectName() == "labelDisplay") {	// ֻ�����ʾ���ֽ��д���
			/************�ر������߳�*************/
			if (realTimeGrab) {
				// ����ʵʱ��ʾ���߳�	
				m_realTimeGrabThread->stop();
				// ��������ⲿ�źŵ��߳�
				m_listenExternalSignal->stop();
			}
			/************�ر������߳�*************/

			QPoint axisInWindow = QPoint(w_obj->mapToGlobal(QPoint(0, 0)));	// ��ȡ�ÿؼ��ڴ����е�����
			int r_x = abs(axisInWindow.rx() - m_x);	// ��Կؼ�������x
			int r_y = abs(axisInWindow.ry() - m_y);	// ��Կؼ�������y
			// std::cout << r_x << " " << r_y << std::endl;
			// ����ʵ�����ű���
			float thisScaleW = static_cast<float>(mCameraWidth) / w_obj->width();
			float thisScaleH = static_cast<float>(mCameraHeight) / w_obj->height();
			float trueScale = MAX(thisScaleW, thisScaleH);
			int wOffset = 0, hOffset = 0;
			if (trueScale == thisScaleH) {	// ���ո߶ȵ����ű�������˵������ϴ���ƫ����
				int curW = static_cast<int>(static_cast<float>(mCameraWidth) / trueScale);
				wOffset = static_cast<int>(static_cast<float>((w_obj->width() - curW)) / 2.0);
			}
			else {	// ���տ�ȵ����ű�������˵���߶��ϴ���ƫ����
				int curH = static_cast<int>(static_cast<float>(mCameraHeight) / trueScale);
				hOffset = static_cast<int>(static_cast<float>((w_obj->height() - curH)) / 2.0);
			}
			int cvtX = static_cast<int>(static_cast<float>(r_x - wOffset) * trueScale);	// ��ͼ���ϵ���������x
			int cvtY = static_cast<int>(static_cast<float>(r_y - hOffset) * trueScale);	// ��ͼ���ϵ���������y
			// ��������ȥ�����ǵڼ���block��
			int blockIdx = getDoubleClickedBlock(cvtX, cvtY);
			// std::cout << "Double clicked id:" << blockIdx << std::endl;
			if (blockIdx != -1) {
				QString ctxt = "�뵥��\"Yes\"���Ϊλ��" + QString::fromStdString(std::to_string(blockIdx)) + "����ģ�壡";
				QMessageBox::StandardButton rs = QMessageBox::information(NULL, "��ʾ", ctxt, QMessageBox::Yes | QMessageBox::No);
				if (rs == QMessageBox::Yes) {
					// (TODO)������ʱͨ��map��list���������棬������ôһ����������ɾ����ĳ��ģ�壬���ﱣ���ȡ��������ģ�壬�д�����
					// �ȼ��費��ɾ��ģ��
					std::string dirPath = ".\\multiTemplate\\" + curRunModel + "\\";
					std::string tmpImagePath = dirPath + curRunModel + "_" + std::to_string(blockIdx)
						+ "_" + std::to_string(multiTempMat[blockIdx].size()) + ".jpg";
					// ����ͼ��
					cv::Mat forSaveMat = matForMutiTemp(cv::Range(landMarkWithId[blockIdx][1], landMarkWithId[blockIdx][3]), 
						cv::Range(landMarkWithId[blockIdx][0], landMarkWithId[blockIdx][2]));
					cv::imwrite(tmpImagePath, forSaveMat);
					std::cout << "Save template " << tmpImagePath << std::endl;
					// ��������ͼ��
					if (!multiTempMat.count(blockIdx)) {	// ��ǰidx�в��ڽṹ��
						std::list<cv::Mat> tmpList;
						tmpList.push_back(forSaveMat);
						multiTempMat.insert(std::make_pair(blockIdx, tmpList));
					}
					else {	// ��ǰidx�Ѿ���ģ��¼��
						// ֱ�Ӳ��뵽list��
						multiTempMat[blockIdx].push_back(forSaveMat);
					}
					// ����multiTemp��Ϣ
					imageProcess->setMultiTempForTargetBlock(multiTempMat);
				}
			}
			/************���������߳�*************/
			if (realTimeGrab) {
				// ����ʵʱ��ʾ���߳�		
				m_realTimeGrabThread->start();
				// ��������ⲿ�źŵ��߳�
				m_listenExternalSignal->start();
			}
			/************���������߳�*************/
		}
		// delete w_obj;	// ����ɾ����ɾ����ɾ���ؼ�����
	}
}

int WindowClass::getDoubleClickedBlock(int x, int y) {
	for (int i = 1; i < landMarkWithId.size(); ++i) {
		if (x >= landMarkWithId[i][0] && x <= landMarkWithId[i][2]
			&& y >= landMarkWithId[i][1] && y <= landMarkWithId[i][3]) {
			return i;
		}
	}
	return -1;
}

void WindowClass::updateDisplay(cv::Mat &dstMaxRoi, std::vector<int> &res) {
	//cv::imwrite("./tmp.jpg", dstMaxRoi);
	ui.labelNormPart->clear();
	ui.labelErrorPart->clear();
	cv::Mat dstMaxRoiDisplay;
	dstMaxRoi.copyTo(dstMaxRoiDisplay);
	// ͼ����ʾ����
	//res[5] = 0;
	bool isOK = true;
	for (int i = 1; i < res.size(); ++i) {
		//std::cout << "res :" << res[i] << std::endl;
		if (!res[i]) {	// �д���
			if (isOK) {
				isOK = false;
				cv::Mat dstNormTemp;
				normTemp.copyTo(dstNormTemp);
				// ������ʾһ�µ�һ�����ִ����λ��
				cv::Mat firstErrorNormPart = dstNormTemp(cv::Range(landMarkWithId[i][1], landMarkWithId[i][3]),
					cv::Range(landMarkWithId[i][0], landMarkWithId[i][2]));
				//cv::imshow("firstErrorNormPart", firstErrorNormPart);
				//cv::waitKey(0);
				cv::cvtColor(firstErrorNormPart, firstErrorNormPart, cv::COLOR_BGR2RGB);
				QImage firstErrorNormPartQImage(firstErrorNormPart.data, firstErrorNormPart.cols, firstErrorNormPart.rows, firstErrorNormPart.step, QImage::Format_RGB888);
				ui.labelNormPart->setPixmap(QPixmap::fromImage(firstErrorNormPartQImage).
					scaled(ui.labelNormPart->width(), ui.labelNormPart->height(), Qt::KeepAspectRatio));

				cv::Mat dstRealMat;
				dstMaxRoi.copyTo(dstRealMat);
				cv::Mat firstErrorRealPart = dstRealMat(cv::Range(landMarkWithId[i][1], landMarkWithId[i][3]),
					cv::Range(landMarkWithId[i][0], landMarkWithId[i][2]));
				//cv::imshow("firstErrorRealPart", firstErrorRealPart);
				//cv::waitKey(0);
				cv::cvtColor(firstErrorRealPart, firstErrorRealPart, cv::COLOR_BGR2RGB);
				QImage firstErrorRealPartQImage(firstErrorRealPart.data, firstErrorRealPart.cols, firstErrorRealPart.rows, firstErrorRealPart.step, QImage::Format_RGB888);
				ui.labelErrorPart->setPixmap(QPixmap::fromImage(firstErrorRealPartQImage).
					scaled(ui.labelErrorPart->width(), ui.labelErrorPart->height(), Qt::KeepAspectRatio));
			}
			cv::rectangle(dstMaxRoiDisplay, cv::Point(landMarkWithId[i][0], landMarkWithId[i][1]),
				cv::Point(landMarkWithId[i][2], landMarkWithId[i][3]), CvScalar(0, 85, 255), 8, cv::LINE_8, 0);

			cv::Size fontSize = cv::getTextSize(std::to_string(i), cv::FONT_HERSHEY_SIMPLEX, 4, 6, 0);
			cv::rectangle(dstMaxRoiDisplay, cv::Point(landMarkWithId[i][0], landMarkWithId[i][1] - fontSize.height),
				cv::Point(landMarkWithId[i][0] + fontSize.width, landMarkWithId[i][1]), CvScalar(0, 85, 255), cv::FILLED);
			cv::putText(dstMaxRoiDisplay, std::to_string(i), cv::Point(landMarkWithId[i][0], landMarkWithId[i][1]), cv::FONT_HERSHEY_SIMPLEX, 4, (0, 0, 0), 6);
		}
		else {
			cv::rectangle(dstMaxRoiDisplay, cv::Point(landMarkWithId[i][0], landMarkWithId[i][1]),
				cv::Point(landMarkWithId[i][2], landMarkWithId[i][3]), CvScalar(0, 170, 0), 8, cv::LINE_8, 0);

			cv::Size fontSize = cv::getTextSize(std::to_string(i), cv::FONT_HERSHEY_SIMPLEX, 4, 6, 0);
			cv::rectangle(dstMaxRoiDisplay, cv::Point(landMarkWithId[i][0], landMarkWithId[i][1] - fontSize.height),
				cv::Point(landMarkWithId[i][0] + fontSize.width, landMarkWithId[i][1]), CvScalar(0, 170, 0), cv::FILLED);
			cv::putText(dstMaxRoiDisplay, std::to_string(i), cv::Point(landMarkWithId[i][0], landMarkWithId[i][1]), cv::FONT_HERSHEY_SIMPLEX, 4, (0, 0, 0), 6);
		}
	}

	if (isOK) {
		insertIntoTableLog("SUCCESS");	// ���ݿ����һ�����
		ui.labelResult->setText(QApplication::translate("mainWindowClass", "<html><head/><body><p><span style=\" font-size:72pt; color:#00aa00;\">OK</span></p></body></html>", nullptr));
		OKCounts += 1;
	}
	else {
		insertIntoTableLog("ERROR");	// ���ݿ����һ�����
		ui.labelResult->setText(QApplication::translate("mainWindowClass", "<html><head/><body><p><span style=\" font-size:72pt; color:#aa0000;\">NG</span></p></body></html>", nullptr));
		NGCounts += 1;
	}

	cv::cvtColor(dstMaxRoiDisplay, dstMaxRoiDisplay, cv::COLOR_BGR2RGB);
	QImage singleQImage(dstMaxRoiDisplay.data, dstMaxRoiDisplay.cols, dstMaxRoiDisplay.rows, dstMaxRoiDisplay.cols * dstMaxRoiDisplay.channels(), QImage::Format_RGB888);
	ui.labelDisplay->setPixmap(QPixmap::fromImage(singleQImage).
		scaled(ui.labelDisplay->width(), ui.labelDisplay->height(), Qt::KeepAspectRatio));

	// ͳ����Ϣ����
	ui.labelTotalCounts->setText(QString::fromStdString(std::to_string(totalCounts)));
	ui.labelOKCounts->setText(QString::fromStdString(std::to_string(OKCounts)));
	ui.labelNGCounts->setText(QString::fromStdString(std::to_string(NGCounts)));
}

void WindowClass::insertIntoTableLog(std::string thisLog) {
	// ��ȡ��ǰʱ��
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
	std::vector<std::string> _thisLog(3);
	_thisLog[0] = current_date.toStdString();
	_thisLog[1] = curRunModel;
	_thisLog[2] = thisLog;
	mysql_conn->insertLogData("log", _thisLog);
}

//// �ر��¼���������Ϣ�����ݿ���
//void WindowClass::closeEvent(QCloseEvent * event) {
//	std::cout << "is on saving data" << std::endl;
//}

WindowClass::~WindowClass()
{
	if (m_realTimeGrabThread->isRunning()) {
		m_realTimeGrabThread->stop();
	}
	delete m_realTimeGrabThread;
	if (m_listenExternalSignal->isRunning()) {
		m_listenExternalSignal->stop();
	}
	delete m_listenExternalSignal;

	delete imageProcess;

	RS232Close();
	delete m_serial;

	if (m_serialTimer->isActive()) {
		m_serialTimer->stop();
	}
	delete m_serialTimer;
}
