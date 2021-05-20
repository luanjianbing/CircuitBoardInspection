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

	// 负责实时显示的线程
	m_realTimeGrabThread = new ThreadForRealTimeGrab(myCamera);
	connect(m_realTimeGrabThread, SIGNAL(signalGrabFrameOver()), SLOT(dealSignalGrabFrameOver()));
	// 负责监听外部信号
	m_listenExternalSignal = new ThreadForListenExternalSignal();
	connect(m_listenExternalSignal, SIGNAL(signalGetExternalSignal()), SLOT(dealSignalGetExternalSignal()));

	// 外部信号
	//connect(ui.buttonRun, SIGNAL(released()), this, SLOT(onButtonRun()));
	connect(ui.actionSignal, SIGNAL(triggered()), this, SLOT(onButtonRun()));
	ui.actionSignal->setEnabled(false);

	// 串口
	if (!RS232Init()) {
		QMessageBox::warning(NULL, "警告", "串口初始化失败！", QMessageBox::Yes);
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

	// 状态栏
	ui.statusbar->showMessage("SINGLE STATION VISUAL INSPECTION SYSTEM VERSION @202105");

	// 菜单栏
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
	// 获取可以用的串口
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
			//设置串口名字
			m_serial->setPortName(serialPortInfo.portName());	// 这里串口名字必须是"COM?"
																//设置波特率
			m_serial->setBaudRate(QSerialPort::Baud9600);
			//设置数据位
			m_serial->setDataBits(QSerialPort::Data8);
			//设置奇偶校验位
			m_serial->setParity(QSerialPort::NoParity);
			//设置停止位
			m_serial->setStopBits(QSerialPort::OneStop);
			//设置流控
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

void WindowClass::onButtonRun() {	// 给出一个模拟外部触发信号
	m_listenExternalSignal->setSignal(true);
}

void WindowClass::onActionStatistics() {
	Statistics *statistics = new Statistics(mysql_conn);
	// 设置子窗口属性，在子窗口关闭之后，释放子窗口的资源(释放指针)
	statistics->setAttribute(Qt::WA_DeleteOnClose);	// 如果释放指针会报错
	statistics->show();
}

void WindowClass::onActionImageSetting() {
	CircuitBoardInspection *imageSetting = new CircuitBoardInspection(myCamera, mysql_conn);
	connect(imageSetting, SIGNAL(signalLoadModel(QString)), this, SLOT(dealSignalLoadModel(QString)));
	// 设置子窗口属性，在子窗口关闭之后，释放子窗口的资源(释放指针)
	//imageSetting->setAttribute(Qt::WA_DeleteOnClose);	// 如果释放指针会报错
	imageSetting->show();
}

void WindowClass::dealSignalLoadModel(QString loadModel) {
	ui.actionRun->setEnabled(true);

	curRunModel = loadModel.toStdString();
	ui.labelModelName->setText(QString::fromStdString(curRunModel));
	std::cout << "curRunModel : " << curRunModel << std::endl;

	// 标准颜色参数信息
	mysql_conn->readColorHSVParam(HSVColorForGetTrueTarget);
	// 获取模型的坐标
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
	// 参数信息
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
	// 胶布长度检测标准值和偏差与角度
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
	// 真正目标物的坐标信息
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
	// 位置检测像素标准值与偏差
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
	// 插头方向检测
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

	// 读取目标block的mask 
	maskForTargetBlock.clear();
	maskForTargetBlock.push_back(cv::Mat());	// 0号位不存在
	for (int i = 1; i < landMarkWithId.size(); ++i) {
		std::string thisPath = ".\\mask\\" + curRunModel + "\\" + std::to_string(i) + ".jpg";
		maskForTargetBlock.push_back(cv::imread(thisPath));
	}
	// 读取已有目标block的模板
	multiTempMat.clear();
	std::string dirPath = ".\\multiTemplate\\" + curRunModel + "\\";	// 保存当前模型多模板的文件夹
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
			int blockIdx = atoi(tempImageName.substr(lastSecondDownLine + 1, lastDownLine).c_str());	// 该图像是第几个block的
			// std::cout << lastDownLine << " " << lastSecondDownLine << " " << blockIdx << std::endl;
			// 读取并添加图像
			std::string thisImagePath = dirPath + tempImageName;
			// std::cout << thisImagePath << std::endl;
			cv::Mat tmpMat = cv::imread(thisImagePath);
			if (!multiTempMat.count(blockIdx)) {	// 当前idx尚不在结构中
				std::list<cv::Mat> tmpList;
				tmpList.push_back(tmpMat);
				multiTempMat.insert(std::make_pair(blockIdx, tmpList));
			}
			else {	// 当前idx已经有模板录入
				// 直接插入到list中
				multiTempMat[blockIdx].push_back(tmpMat);
			}
		} while (!_findnext(handle, &fileInfo));
	}

	/************************************************************/
	cv::Mat singleMat(myCamera->getCameraResolution(), CV_8UC3);
	myCamera->getOneFrame(singleMat);
	//singleMat = cv::imread("./test/norm.jpg");	// 测试标准图像
	/************************************************************/

	// 截取目标最大ROI
	
	singleMat = singleMat(cv::Range(landMarkWithId[0][1], landMarkWithId[0][3]),
		cv::Range(landMarkWithId[0][0], landMarkWithId[0][2]));
	//dstMaxRoi = dstMaxRoi(cv::Rect(landMarkWithId[0][0], landMarkWithId[0][1], (landMarkWithId[0][2] - landMarkWithId[0][0]), (landMarkWithId[0][3] - landMarkWithId[0][1])));
	
	// 保存宽度与高度
	mCameraWidth = singleMat.cols;	// 保存经过最大范围标定后的宽度
	mCameraHeight = singleMat.rows;	// 保存经过最大范围标定后的高度
	matForMutiTemp = cv::Mat::zeros(mCameraHeight, mCameraWidth, CV_8UC3);

	//// 保存一个标准图像
	////normTemp = singleMat.clone();
	////cv::imwrite("norm.jpg", singleMat);
	//normTemp = cv::imread("norm.jpg");
	QMessageBox::StandardButton rs = QMessageBox::information(NULL, "提示", "是否保存当前图像为标准图像(norm.jpg)", QMessageBox::Yes | QMessageBox::No);
	if (rs == QMessageBox::Yes) {	// 保存为标准图像
		normTemp = singleMat.clone();
		cv::imwrite("norm.jpg", singleMat);
	}
	else {	// 直接从本地读取
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

	// 创建对象
	imageProcess = new ImageProcess(landMarkWithId, vectParamForModel, lengthPixel, trueTargetPartAxis, posProcessParam, plugDirectMaskMsg, normTemp);
	imageProcess->setHSVColorForGetTrueTarget(HSVColorForGetTrueTarget);	// 设置颜色信息
	// 设置mask信息
	imageProcess->setMaskForTargetBlock(maskForTargetBlock);
	// 设置multiTemp信息
	imageProcess->setMultiTempForTargetBlock(multiTempMat);
	// 配准一下，以后快一点
	imageProcess->matchTwoImage(singleMat, singleMat);

	// 状态栏
	QString tmpMsg = "LOAD MODEL " + QString::fromStdString(curRunModel);
	ui.statusbar->showMessage(tmpMsg);
}

void WindowClass::onActionRun() {
	// (TODO)开辟一个线程负责动态显示，定时抓拍显示(子线程保存到本地，给个信号到主界面，主界面读取显示)
	if (!realTimeGrab) {
		if (m_serialTimer->isActive() == false) {	// 启动串口发送
			m_serialTimer->start();
		}

		// 负责实时显示的线程		
		m_realTimeGrabThread->start();

		// 负责监听外部信号的线程
		m_listenExternalSignal->start();

		ui.actionSignal->setEnabled(true);

		ui.actionRun->setText("停止");
		ui.actionRun->setIcon(QIcon(".\\icon\\Stop.png"));

		ui.statusbar->showMessage("Start.");

		realTimeGrab = !realTimeGrab;
	}
	else {
		if (m_serialTimer->isActive() == true) {	// 关闭串口发送
			m_serialTimer->stop();
		}

		// 负责实时显示的线程	
		m_realTimeGrabThread->stop();
		// 负责监听外部信号的线程
		m_listenExternalSignal->stop();

		ui.actionSignal->setEnabled(false);

		ui.actionRun->setText("运行");
		ui.actionRun->setIcon(QIcon(".\\icon\\Run.png"));

		ui.statusbar->showMessage("Stop.");

		realTimeGrab = !realTimeGrab;
	}

	// (TODO)此外如何将三十二个区域处理（是否需要考虑使用线程池？），这边是通过响应外部信号抓拍图像
	// 1. 监听外部信号（是否开始抓拍图像，开始检测）(TODO如何监听？，暂时可以用一个软件上的button实现模拟)	// 如果后面监测外部信号的话这里也要开一个线程负责监听外部物理
	// 2. 当接收到信号的时候，往线程池里面添加任务，当处理完成后，返回结果（界面怎么知道处理的结果？线程池如何处理完了之后告诉主线程），这里可能还需要开一个线程负责监听线程池中的任务完成


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
	
	// 接收到外部信号
	std::cout << "get external signal" << std::endl;
	// 总的次数+1
	totalCounts += 1;

	// (TODO)关闭响应鼠标双击事件
	canGetDoubleClickedSignal = false;

	// (TODO)需不需要考虑线程池？
	// 如果使用相机抓拍的话，可能需要把实时显示那个线程关掉抓拍完再打开
	m_realTimeGrabThread->stop();
	m_listenExternalSignal->stop();
	// 假设先不考虑线程池的问题，先按顺序对所标记的区域进行检测
	/*********************************************************************/
	cv::Mat singleMat(myCamera->getCameraResolution(), CV_8UC3);
	myCamera->getOneFrame(singleMat);
	// singleMat = cv::imread("./test/Error1.jpg");	// 测试图像
	// singleMat = cv::imread("./test/Error2.jpg");	// 测试图像
	// singleMat = cv::imread("./test/Error3.jpg");	// 测试图像

	//singleMat = cv::imread("./test/Error1_1.jpg");	// 测试图像Error1_1
	//singleMat = cv::imread("./test/Error1_2.jpg");	// 测试图像Error1_2
	//singleMat = cv::imread("./test/Error1_3.jpg");	// 测试图像Error1_3
	//singleMat = cv::imread("./test/Error1_4.jpg");	// 测试图像Error1_4
	//singleMat = cv::imread("./test/Error1_5.jpg");	// 测试图像Error1_5
	//singleMat = cv::imread("./test/Error2_1.jpg");	// 测试图像Error2_1
	//singleMat = cv::imread("./test/Error2_2.jpg");	// 测试图像Error2_2
	//singleMat = cv::imread("./test/Error2_3.jpg");	// 测试图像Error2_3
	//singleMat = cv::imread("./test/Error2_4.jpg");	// 测试图像Error2_4
	//singleMat = cv::imread("./test/Error3_1.jpg");	// 测试图像Error3_1
	//singleMat = cv::imread("./test/Error3_2.jpg");	// 测试图像Error3_2
	//singleMat = cv::imread("./test/Error3_3.jpg");	// 测试图像Error3_3
	//singleMat = cv::imread("./test/Error3_4.jpg");	// 测试图像Error3_4
	//singleMat = cv::imread("./test/Error3_5.jpg");	// 测试图像Error3_5

	/*********************************************************************/
	if (singleMat.empty())
		assert("Get input Image Error.");
	
	singleMat = singleMat(cv::Range(landMarkWithId[0][1], landMarkWithId[0][3]),
		cv::Range(landMarkWithId[0][0], landMarkWithId[0][2]));
	cv::Mat dstMaxRoi;
	singleMat.copyTo(dstMaxRoi);
	singleMat.copyTo(matForMutiTemp);	// 用于多模板保存
	//cv::imwrite("./tmp.jpg", dstMaxRoi);
	std::vector<int> res = imageProcess->blockProcessing(dstMaxRoi);

	// 显示更新
	updateDisplay(dstMaxRoi, res);
	// (TODO)开启响应鼠标双击事件
	canGetDoubleClickedSignal = true;

	m_listenExternalSignal->start();
	m_realTimeGrabThread->start();

	//// 状态栏
	//ui.statusbar->showMessage("FREE");
}

void WindowClass::mouseDoubleClickEvent(QMouseEvent *ev) {
	if (ev->buttons() & Qt::LeftButton) {
		int m_x = QCursor::pos().x();
		int m_y = QCursor::pos().y();
		QWidget *w_obj = QApplication::widgetAt(m_x, m_y);
		if (canGetDoubleClickedSignal && w_obj->objectName() == "labelDisplay") {	// 只针对显示部分进行处理
			/************关闭其他线程*************/
			if (realTimeGrab) {
				// 负责实时显示的线程	
				m_realTimeGrabThread->stop();
				// 负责监听外部信号的线程
				m_listenExternalSignal->stop();
			}
			/************关闭其他线程*************/

			QPoint axisInWindow = QPoint(w_obj->mapToGlobal(QPoint(0, 0)));	// 获取该控件在窗体中的坐标
			int r_x = abs(axisInWindow.rx() - m_x);	// 相对控件的坐标x
			int r_y = abs(axisInWindow.ry() - m_y);	// 相对控件的坐标y
			// std::cout << r_x << " " << r_y << std::endl;
			// 计算实际缩放倍数
			float thisScaleW = static_cast<float>(mCameraWidth) / w_obj->width();
			float thisScaleH = static_cast<float>(mCameraHeight) / w_obj->height();
			float trueScale = MAX(thisScaleW, thisScaleH);
			int wOffset = 0, hOffset = 0;
			if (trueScale == thisScaleH) {	// 按照高度的缩放倍数来，说明宽度上存在偏移量
				int curW = static_cast<int>(static_cast<float>(mCameraWidth) / trueScale);
				wOffset = static_cast<int>(static_cast<float>((w_obj->width() - curW)) / 2.0);
			}
			else {	// 按照宽度的缩放倍数来，说明高度上存在偏移量
				int curH = static_cast<int>(static_cast<float>(mCameraHeight) / trueScale);
				hOffset = static_cast<int>(static_cast<float>((w_obj->height() - curH)) / 2.0);
			}
			int cvtX = static_cast<int>(static_cast<float>(r_x - wOffset) * trueScale);	// 在图像上的真正坐标x
			int cvtY = static_cast<int>(static_cast<float>(r_y - hOffset) * trueScale);	// 在图像上的真正坐标y
			// 根据坐标去检索是第几个block的
			int blockIdx = getDoubleClickedBlock(cvtX, cvtY);
			// std::cout << "Double clicked id:" << blockIdx << std::endl;
			if (blockIdx != -1) {
				QString ctxt = "请单击\"Yes\"添加为位置" + QString::fromStdString(std::to_string(blockIdx)) + "处的模板！";
				QMessageBox::StandardButton rs = QMessageBox::information(NULL, "提示", ctxt, QMessageBox::Yes | QMessageBox::No);
				if (rs == QMessageBox::Yes) {
					// (TODO)这里暂时通过map的list长度来保存，存在这么一种情况，如果删掉了某个模板，这里保存会取代掉最后的模板，有待完善
					// 先假设不能删除模板
					std::string dirPath = ".\\multiTemplate\\" + curRunModel + "\\";
					std::string tmpImagePath = dirPath + curRunModel + "_" + std::to_string(blockIdx)
						+ "_" + std::to_string(multiTempMat[blockIdx].size()) + ".jpg";
					// 保存图像
					cv::Mat forSaveMat = matForMutiTemp(cv::Range(landMarkWithId[blockIdx][1], landMarkWithId[blockIdx][3]), 
						cv::Range(landMarkWithId[blockIdx][0], landMarkWithId[blockIdx][2]));
					cv::imwrite(tmpImagePath, forSaveMat);
					std::cout << "Save template " << tmpImagePath << std::endl;
					// 更新已有图像
					if (!multiTempMat.count(blockIdx)) {	// 当前idx尚不在结构中
						std::list<cv::Mat> tmpList;
						tmpList.push_back(forSaveMat);
						multiTempMat.insert(std::make_pair(blockIdx, tmpList));
					}
					else {	// 当前idx已经有模板录入
						// 直接插入到list中
						multiTempMat[blockIdx].push_back(forSaveMat);
					}
					// 设置multiTemp信息
					imageProcess->setMultiTempForTargetBlock(multiTempMat);
				}
			}
			/************开启其他线程*************/
			if (realTimeGrab) {
				// 负责实时显示的线程		
				m_realTimeGrabThread->start();
				// 负责监听外部信号的线程
				m_listenExternalSignal->start();
			}
			/************开启其他线程*************/
		}
		// delete w_obj;	// 不能删除，删除会删除控件对象
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
	// 图像显示更新
	//res[5] = 0;
	bool isOK = true;
	for (int i = 1; i < res.size(); ++i) {
		//std::cout << "res :" << res[i] << std::endl;
		if (!res[i]) {	// 有错误
			if (isOK) {
				isOK = false;
				cv::Mat dstNormTemp;
				normTemp.copyTo(dstNormTemp);
				// 这里显示一下第一个出现错误的位置
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
		insertIntoTableLog("SUCCESS");	// 数据库插入一条结果
		ui.labelResult->setText(QApplication::translate("mainWindowClass", "<html><head/><body><p><span style=\" font-size:72pt; color:#00aa00;\">OK</span></p></body></html>", nullptr));
		OKCounts += 1;
	}
	else {
		insertIntoTableLog("ERROR");	// 数据库插入一条结果
		ui.labelResult->setText(QApplication::translate("mainWindowClass", "<html><head/><body><p><span style=\" font-size:72pt; color:#aa0000;\">NG</span></p></body></html>", nullptr));
		NGCounts += 1;
	}

	cv::cvtColor(dstMaxRoiDisplay, dstMaxRoiDisplay, cv::COLOR_BGR2RGB);
	QImage singleQImage(dstMaxRoiDisplay.data, dstMaxRoiDisplay.cols, dstMaxRoiDisplay.rows, dstMaxRoiDisplay.cols * dstMaxRoiDisplay.channels(), QImage::Format_RGB888);
	ui.labelDisplay->setPixmap(QPixmap::fromImage(singleQImage).
		scaled(ui.labelDisplay->width(), ui.labelDisplay->height(), Qt::KeepAspectRatio));

	// 统计信息更新
	ui.labelTotalCounts->setText(QString::fromStdString(std::to_string(totalCounts)));
	ui.labelOKCounts->setText(QString::fromStdString(std::to_string(OKCounts)));
	ui.labelNGCounts->setText(QString::fromStdString(std::to_string(NGCounts)));
}

void WindowClass::insertIntoTableLog(std::string thisLog) {
	// 获取当前时间
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
	std::vector<std::string> _thisLog(3);
	_thisLog[0] = current_date.toStdString();
	_thisLog[1] = curRunModel;
	_thisLog[2] = thisLog;
	mysql_conn->insertLogData("log", _thisLog);
}

//// 关闭事件，保存信息到数据库中
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
