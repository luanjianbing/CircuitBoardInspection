#pragma once

#include "common.h"
#include "MvCameraControl.h"

class MyCamera {
private:
	static const int cameraWidth = 5472;	// ���ͼ����
	static const int cameraHeight = 3648;	// ���ͼ��߶�
	//static const int cameraWidth = 3840;	// ���ͼ����
	//static const int cameraHeight = 2748;	// ���ͼ��߶�

	bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo) const;

	// convert data stream in Mat format
	bool Convert2Mat(MV_FRAME_OUT_INFO_EX* pstImageInfo, unsigned char * pData, cv::Mat & srcImg) const;

	int RGB2BGR(unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight) const;

	void* handle = NULL;
	int g_nPayloadSize = 0;

	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	unsigned char *pData = NULL;	// �̶����ͼ�񻺴���
public:
	MyCamera();
	~MyCamera();

	// �����ʼ��
	bool cameraInit();

	// ��ȡ����ͼ��
	bool getOneFrame(cv::Mat &singleMat);

	// �ر����
	bool cameraClose();

	// ���ͼ��ֱ��ʣ� ��cameraWidth��cameraHeight��������
	cv::Size getCameraResolution() const;
};