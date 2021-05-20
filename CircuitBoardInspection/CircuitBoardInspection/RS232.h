#pragma once

#include <iostream>
#include <QtSerialPort\QSerialPort>
#include <QtSerialPort\QSerialPortInfo>

class RS232Communication
{
public:
	RS232Communication();
	~RS232Communication();

	bool RS232Init();	// 打开串口
	bool RS232Close();	// 关闭串口
	void sendSingleChar(QString cmd);	// 发送单个字符
	QString readData();

private:
	QSerialPort *m_serial;

	//// 串口参数
	//const int m_baud = QSerialPort::Baud9600;	// 波特率
	//const int m_data = QSerialPort::Data8;	// 数据位
	//const int m_parity = QSerialPort::NoParity;	// 奇偶校验位
	//const int m_stop = QSerialPort::OneStop;	// 停止位
	//const int m_flowControl = QSerialPort::NoFlowControl;	// 添加流控

};


