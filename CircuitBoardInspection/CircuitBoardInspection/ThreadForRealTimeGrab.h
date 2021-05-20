#pragma once
#include <QThread>
#include "MyCamera.h"

// �����Ǹ���̬��ʾץ�ĵ��߳�
class ThreadForRealTimeGrab : public QThread {
	Q_OBJECT
public:
	ThreadForRealTimeGrab(MyCamera *myCamera, QObject* parent = nullptr);
	~ThreadForRealTimeGrab();

	void stop();
signals:
	void signalGrabFrameOver();
protected:
	void run();
private:
	MyCamera *myCamera;
	bool m_stopFlag = false;
};