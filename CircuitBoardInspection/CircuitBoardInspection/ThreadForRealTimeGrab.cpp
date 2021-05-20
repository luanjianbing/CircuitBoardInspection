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
	m_stopFlag = false;	// 为了能够多次启动
}

void ThreadForRealTimeGrab::run()
{
	//该线程管理类对应的线程实际运行代码位置
	while (!m_stopFlag) {
		//std::cout << "send grab signal" << std::endl;
		cv::Mat singleMat(myCamera->getCameraResolution(), CV_8UC3);
		myCamera->getOneFrame(singleMat);
		cv::imwrite("./realTime.jpg", singleMat);

		emit signalGrabFrameOver();
		usleep(50);
	}
}