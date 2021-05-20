#pragma once

#include <QThread>

#include <iostream>
#include <QtSerialPort\QSerialPort>
#include <QtSerialPort\QSerialPortInfo>

class ThreadForListenExternalSignal : public QThread {
	Q_OBJECT
public:
	ThreadForListenExternalSignal(QObject* parent = nullptr);
	~ThreadForListenExternalSignal();

	void setSignal(bool m_signal);

	void stop();
signals:
	void signalGetExternalSignal();
protected:
	void run();
private:
	bool m_stopFlag = false;
	bool m_signalFlag = false;
	//bool m_signalPortFlag = false;

	//QSerialPort *m_serial;
	//bool RS232Init();	// 打开串口
	//bool RS232Close();	// 关闭串口
	//void sendSingleChar(QString &&cmd);
private slots:
	//void portReadyRead();
};