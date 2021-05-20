#pragma once

#include "common.h"
#include "MvCameraControl.h"

class MyCamera {
private:
	static const int cameraWidth = 5472;	// 相机图像宽度
	static const int cameraHeight = 3648;	// 相机图像高度
	//static const int cameraWidth = 3840;	// 相机图像宽度
	//static const int cameraHeight = 2748;	// 相机图像高度

	bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo) const;

	// convert data stream in Mat format
	bool Convert2Mat(MV_FRAME_OUT_INFO_EX* pstImageInfo, unsigned char * pData, cv::Mat & srcImg) const;

	int RGB2BGR(unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight) const;

	void* handle = NULL;
	int g_nPayloadSize = 0;

	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	unsigned char *pData = NULL;	// 固定相机图像缓存区
public:
	MyCamera();
	~MyCamera();

	// 相机初始化
	bool cameraInit();

	// 获取单张图像
	bool getOneFrame(cv::Mat &singleMat);

	// 关闭相机
	bool cameraClose();

	// 获得图像分辨率， 即cameraWidth与cameraHeight两个变量
	cv::Size getCameraResolution() const;
};