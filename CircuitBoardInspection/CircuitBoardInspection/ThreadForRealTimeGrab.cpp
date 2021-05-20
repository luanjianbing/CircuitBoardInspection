#include "ThreadForRealTimeGrab.h"

ThreadForRealTimeGrab::ThreadForRealTimeGrab(MyCamera *myCamera, QObject* parent)
	: myCamera(myCamera) {
}

ThreadForRealTimeGrab::~ThreadForRealTimeGrab() {
}

void ThreadForRealTimeGrab::stop()
{
	m_stopFlag = true;
	this->quit();
	this->wait();
	m_stopFlag = false;	// Ϊ���ܹ��������
}

void ThreadForRealTimeGrab::run()
{
	//���̹߳������Ӧ���߳�ʵ�����д���λ��
	while (!m_stopFlag) {
		//std::cout << "send grab signal" << std::endl;
		cv::Mat singleMat(myCamera->getCameraResolution(), CV_8UC3);
		myCamera->getOneFrame(singleMat);
		cv::imwrite("./realTime.jpg", singleMat);

		emit signalGrabFrameOver();
		usleep(50);
	}
}