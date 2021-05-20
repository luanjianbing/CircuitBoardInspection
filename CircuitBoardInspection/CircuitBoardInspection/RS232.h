#pragma once

#include <iostream>
#include <QtSerialPort\QSerialPort>
#include <QtSerialPort\QSerialPortInfo>

class RS232Communication
{
public:
	RS232Communication();
	~RS232Communication();

	bool RS232Init();	// �򿪴���
	bool RS232Close();	// �رմ���
	void sendSingleChar(QString cmd);	// ���͵����ַ�
	QString readData();

private:
	QSerialPort *m_serial;

	//// ���ڲ���
	//const int m_baud = QSerialPort::Baud9600;	// ������
	//const int m_data = QSerialPort::Data8;	// ����λ
	//const int m_parity = QSerialPort::NoParity;	// ��żУ��λ
	//const int m_stop = QSerialPort::OneStop;	// ֹͣλ
	//const int m_flowControl = QSerialPort::NoFlowControl;	// �������

};


