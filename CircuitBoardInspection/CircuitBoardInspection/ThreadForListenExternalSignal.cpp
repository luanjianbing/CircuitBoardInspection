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
	//// ��ʼ������
	//if (!this->RS232Init())
	//	exit(0);
	//connect(m_serial, SIGNAL(readyRead()), this, SLOT(portReadyRead()));
	//connect(m_serial, &QSerialPort::readyRead, this, &ThreadForListenExternalSignal::portReadyRead);
	//���̹߳������Ӧ���߳�ʵ�����д���λ��
	while (!m_stopFlag) {
		//char tmp = 'A';
		//m_serial->write(&tmp);
		//sendSingleChar("A");
		//portReadyRead();
		if (m_signalFlag) {	// ������Ҫ��(||)�ϼ���ⲿӲ���źţ���ǰֻ��������ź�

			emit signalGetExternalSignal();
			m_signalFlag = false;
			//m_signalPortFlag = false;
		}
		//usleep(5);	// ÿ��5ms����һ���ж�
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
//	// ��ȡ�����õĴ���
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
//			//���ô�������
//			m_serial->setPortName(serialPortInfo.portName());	// ���ﴮ�����ֱ�����"COM?"
//																//���ò�����
//			m_serial->setBaudRate(QSerialPort::Baud9600);
//			//��������λ
//			m_serial->setDataBits(QSerialPort::Data8);
//			//������żУ��λ
//			m_serial->setParity(QSerialPort::NoParity);
//			//����ֹͣλ
//			m_serial->setStopBits(QSerialPort::OneStop);
//			//��������
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