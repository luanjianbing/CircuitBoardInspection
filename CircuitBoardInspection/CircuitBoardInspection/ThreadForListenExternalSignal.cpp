#include "ThreadForListenExternalSignal.h"

ThreadForListenExternalSignal::ThreadForListenExternalSignal(QObject* parent)
{
	
}

ThreadForListenExternalSignal::~ThreadForListenExternalSignal() {
	//delete m_serial;
}

void ThreadForListenExternalSignal::stop()
{
	
	m_stopFlag = true;
	this->quit();
	this->wait();
	m_stopFlag = false;
}

void ThreadForListenExternalSignal::setSignal(bool m_signal) {
	m_signalFlag = m_signal;
}

//void ThreadForListenExternalSignal::portReadyRead() {
//	//std::cout << "Get Port Recv[Ex]." << std::endl;
//	//if (!m_signalPortFlag) {
//	//	//disconnect(m_serial, SIGNAL(readyRead()), this, SLOT(portReadyRead()));
//
//	//	std::cout << "Get Port Recv." << std::endl;
//
//	//	m_signalPortFlag = true;
//	//	//connect(m_serial, SIGNAL(readyRead()), this, SLOT(portReadyRead()));
//	//}
//	QString ans = "";
//	QByteArray buf;
//	buf = m_serial->readAll();
//	if (!buf.isEmpty()) {
//		ans += buf.toHex();
//	}
//	buf.clear();
//	if (ans != "")
//		std::cout << ans.toStdString() << std::endl;
//}

void ThreadForListenExternalSignal::run()
{
	//// 初始化串口
	//if (!this->RS232Init())
	//	exit(0);
	//connect(m_serial, SIGNAL(readyRead()), this, SLOT(portReadyRead()));
	//connect(m_serial, &QSerialPort::readyRead, this, &ThreadForListenExternalSignal::portReadyRead);
	//该线程管理类对应的线程实际运行代码位置
	while (!m_stopFlag) {
		//char tmp = 'A';
		//m_serial->write(&tmp);
		//sendSingleChar("A");
		//portReadyRead();
		if (m_signalFlag) {	// 这里需要或(||)上监测外部硬件信号，当前只考虑软件信号

			emit signalGetExternalSignal();
			m_signalFlag = false;
			//m_signalPortFlag = false;
		}
		//usleep(5);	// 每过5ms进行一次判断
	}
	//if (!this->RS232Close())
	//	exit(0);
}

//void ThreadForListenExternalSignal::sendSingleChar(QString &&cmd) {
//	//bool isNum = false;
//	//char ch = cmd.toInt(&isNum, 16);
//	// std::cout << "Send data[" << ch << "]" << std::endl;
//	char ch = 'A';
//	m_serial->write(&ch, 1);
//}
//
//bool ThreadForListenExternalSignal::RS232Init() {
//	m_serial = new QSerialPort();
//	// 获取可以用的串口
//	QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();
//	std::cout << "Ports Num is [" << serialPortInfos.count() << "] ";
//	if (serialPortInfos.count() < 1) {
//		std::cout << "Port Count Error." << std::endl;
//		return false;
//	}
//	for (const QSerialPortInfo &serialPortInfo : serialPortInfos)
//	{
//		if ("EIASF103Y23" == serialPortInfo.serialNumber()) {
//			// std::cout << "Start Open Port." << std::endl;
//			//设置串口名字
//			m_serial->setPortName(serialPortInfo.portName());	// 这里串口名字必须是"COM?"
//																//设置波特率
//			m_serial->setBaudRate(QSerialPort::Baud9600);
//			//设置数据位
//			m_serial->setDataBits(QSerialPort::Data8);
//			//设置奇偶校验位
//			m_serial->setParity(QSerialPort::NoParity);
//			//设置停止位
//			m_serial->setStopBits(QSerialPort::OneStop);
//			//设置流控
//			m_serial->setFlowControl(QSerialPort::NoFlowControl);
//			if (!m_serial->open(QIODevice::ReadWrite)) {
//				std::cout << "Open Port Error." << std::endl;
//				return false;
//			}
//			else {
//				m_serial->setDataTerminalReady(false);
//				std::cout << "Open Port Success." << std::endl;
//				break;
//			}
//		}
//	}
//	return true;
//}
//
//bool ThreadForListenExternalSignal::RS232Close() {
//	try
//	{
//		m_serial->close();
//	}
//	catch (const std::exception &e)
//	{
//		std::cout << "Close Port Error[" << e.what() << "]" << std::endl;
//		return false;
//	}
//	std::cout << "Close Port." << std::endl;
//	return true;
//}