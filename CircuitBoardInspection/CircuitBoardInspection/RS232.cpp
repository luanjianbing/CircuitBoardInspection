#include "RS232.h"

RS232Communication::RS232Communication()
{
}

RS232Communication::~RS232Communication()
{
	delete m_serial;
}

void RS232Communication::sendSingleChar(QString cmd) {
	bool isNum = false;
	char ch = cmd.toInt(&isNum, 16);
	std::cout << "Send data[" << ch << "]" << std::endl;
	m_serial->write(&ch, 1);
}

QString RS232Communication::readData() {
	QString ans = "";
	QByteArray buf;
	buf = m_serial->readAll();
	if (!buf.isEmpty()) {
		ans += buf.toHex();
	}
	buf.clear();
	return ans;
}

bool RS232Communication::RS232Init() {
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

bool RS232Communication::RS232Close() {
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