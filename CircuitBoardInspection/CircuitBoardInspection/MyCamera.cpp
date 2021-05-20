#include "MyCamera.h"

MyCamera::MyCamera() {

}

MyCamera::~MyCamera() {

}

bool MyCamera::cameraInit() {
	int nRet = MV_OK;
	// Enum device
	MV_CC_DEVICE_INFO_LIST stDeviceList;
	memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
	nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
	if (MV_OK != nRet || stDeviceList.nDeviceNum != 1) {	// 枚举外部相机失败
		printf("Enum Devices fail! nRet [0x%x]\n", nRet);
		return false;
	}
	else {
		printf("[device %d]:\n", 0);
		MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[0];
		if (NULL == pDeviceInfo)
		{
			return false;
		}
		PrintDeviceInfo(pDeviceInfo);
	}

	// 默认只有一个相机
	// Select device and create handle
	if (MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[0]) != MV_OK) {
		printf("Create Handle fail! nRet [0x%x]\n", nRet);
		return false;
	}
	// open device
	if (MV_CC_OpenDevice(handle) != MV_OK) {
		printf("Open Device fail! nRet [0x%x]\n", nRet);
		return false;
	}
	// 设置获取的图像格式为RGB
	nRet = MV_CC_SetPixelFormat(handle, PixelType_Gvsp_RGB8_Packed);
	if (MV_OK != nRet)
	{
		printf("Set PixelFormat fail! nRet [0x%x]\n", nRet);
		return false;
	}
	// 打开自动曝光
	nRet = MV_CC_SetEnumValue(handle, "ExposureAuto", MV_EXPOSURE_AUTO_MODE_CONTINUOUS);
	if (MV_OK != nRet)
	{
		printf("Set ExposureAuto fail! nRet [0x%x]\n", nRet);
		return false;
	}
	// 打开自动增益
	nRet = MV_CC_SetEnumValue(handle, "GainAuto", MV_GAIN_MODE_CONTINUOUS);
	if (MV_OK != nRet)
	{
		printf("Set GainAuto fail! nRet [0x%x]\n", nRet);
		return false;
	}
	// 打开伽马增强
	nRet = MV_CC_SetBoolValue(handle, "GammaEnable", true);
	if (MV_OK != nRet)
	{
		printf("Set GammaEnable fail! nRet [0x%x]\n", nRet);
		return false;
	}
	// Detection network optimal package size(It only works for the GigE camera)
	if (stDeviceList.pDeviceInfo[0]->nTLayerType == MV_GIGE_DEVICE)
	{
		int nPacketSize = MV_CC_GetOptimalPacketSize(handle);
		if (nPacketSize > 0)
		{
			if (MV_CC_SetIntValue(handle, "GevSCPSPacketSize", nPacketSize) != MV_OK)
			{
				printf("Warning: Set Packet Size fail!\n");
				return false;
			}
		}
		else
		{
			printf("Warning: Get Packet Size fail!\n");
			return false;
		}
	}
	// Set trigger mode as off
	nRet = MV_CC_SetEnumValue(handle, "TriggerMode", 0);
	if (MV_OK != nRet)
	{
		printf("Set Trigger Mode fail! nRet [0x%x]\n", nRet);
		return false;
	}
	// Get payload size
	MVCC_INTVALUE stParam;
	memset(&stParam, 0, sizeof(MVCC_INTVALUE));
	nRet = MV_CC_GetIntValue(handle, "PayloadSize", &stParam);
	if (MV_OK != nRet)
	{
		printf("Get PayloadSize fail! nRet [0x%x]\n", nRet);
		return false;
	}
	g_nPayloadSize = stParam.nCurValue;

	// 分配相机图像缓存区
	memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
	pData = (unsigned char *)malloc(sizeof(unsigned char) * (g_nPayloadSize));
	if (pData == NULL)
	{
		printf("Allocate memory failed.\n");
		return false;
	}

	return true;
}

bool MyCamera::getOneFrame(cv::Mat &singleMat) {
	//singleMat = cv::imread("./Test.jpg");	// BGR
	// cv::cvtColor(singleMat, singleMat, cv::COLOR_BGR2RGB);
	int nRet = MV_OK;

	// Start grab image
	nRet = MV_CC_StartGrabbing(handle);
	if (MV_OK != nRet)
	{
		printf("Start Grabbing fail! nRet [0x%x]\n", nRet);
		return false;
	}

	// get one frame from camera with timeout=1000ms
	nRet = MV_CC_GetOneFrameTimeout(handle, pData, g_nPayloadSize, &stImageInfo, 1000);
	if (nRet != MV_OK)
	{
		printf("No data[0x%x]\n", nRet);
		free(pData);
		pData = NULL;
		return false;
	}

	// 数据去转换
	bool bConvertRet = false;
	bConvertRet = Convert2Mat(&stImageInfo, pData, singleMat);
	// print result
	if (!bConvertRet)
	{
		printf("OpenCV format convert failed.\n");
		return false;
	}

	// Stop grab image
	nRet = MV_CC_StopGrabbing(handle);
	if (MV_OK != nRet)
	{
		printf("Stop Grabbing fail! nRet [0x%x]\n", nRet);
		return false;
	}
	
	return true;
}

bool MyCamera::cameraClose() {
	int nRet = MV_OK;

	// Close device
	nRet = MV_CC_CloseDevice(handle);
	if (MV_OK != nRet)
	{
		printf("ClosDevice fail! nRet [0x%x]\n", nRet);
		return false;
	}

	// Destroy handle
	nRet = MV_CC_DestroyHandle(handle);
	if (MV_OK != nRet)
	{
		printf("Destroy Handle fail! nRet [0x%x]\n", nRet);
		return false;
	}

	// 删除缓存区
	free(pData);
	pData = NULL;

	return true;
}

cv::Size MyCamera::getCameraResolution() const {
	return cv::Size(MyCamera::cameraWidth, MyCamera::cameraHeight);
}

// print the discovered devices information to user
bool MyCamera::PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo) const
{
	if (NULL == pstMVDevInfo)
	{
		printf("The Pointer of pstMVDevInfo is NULL!\n");
		return false;
	}
	if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
	{
		int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
		int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
		int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
		int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

		// print current ip and user defined name
		printf("CurrentIp: %d.%d.%d.%d\n", nIp1, nIp2, nIp3, nIp4);
		printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
	}
	else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
	{
		printf("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
		printf("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
		printf("Device Number: %d\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.nDeviceNumber);
	}
	else
	{
		printf("Not support.\n");
	}

	return true;
}

int MyCamera::RGB2BGR(unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight) const
{
	if (NULL == pRgbData)
	{
		return MV_E_PARAMETER;
	}

	for (unsigned int j = 0; j < nHeight; j++)
	{
		for (unsigned int i = 0; i < nWidth; i++)
		{
			unsigned char red = pRgbData[j * (nWidth * 3) + i * 3];
			pRgbData[j * (nWidth * 3) + i * 3] = pRgbData[j * (nWidth * 3) + i * 3 + 2];
			pRgbData[j * (nWidth * 3) + i * 3 + 2] = red;
		}
	}

	return MV_OK;
}

// convert data stream in Mat format
bool MyCamera::Convert2Mat(MV_FRAME_OUT_INFO_EX* pstImageInfo, unsigned char * pData, cv::Mat & srcImg) const
{
	if (pstImageInfo->enPixelType == PixelType_Gvsp_Mono8)
	{
		srcImg = cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC1, pData);
	}
	else if (pstImageInfo->enPixelType == PixelType_Gvsp_RGB8_Packed)
	{
		RGB2BGR(pData, pstImageInfo->nWidth, pstImageInfo->nHeight);
		srcImg = cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC3, pData);
	}
	/*****************************/
	else if (pstImageInfo->enPixelType == PixelType_Gvsp_BayerGR8|| pstImageInfo->enPixelType == PixelType_Gvsp_BayerGB8) {
		//printf("PixelType_Gvsp_BayerGR8\n");
		//return false;
		/*
		进入这个判断会出现内存泄露问题
		因为没有释放pDataForRGB
		释放之后就没有图了
		*/
		unsigned char *pDataForRGB = NULL;

		pDataForRGB = (unsigned char*)malloc(stImageInfo.nWidth * stImageInfo.nHeight * 3);
		if (NULL == pDataForRGB)
		{
			printf("malloc pDataForRGB fail !\n");
			return false;
		}
		unsigned int nDataSizeForRGB = stImageInfo.nWidth * stImageInfo.nHeight * 3;

		MV_CC_PIXEL_CONVERT_PARAM stConvertParam = { 0 };
		memset(&stConvertParam, 0, sizeof(MV_CC_PIXEL_CONVERT_PARAM));
		stConvertParam.nWidth = stImageInfo.nWidth;
		stConvertParam.nHeight = stImageInfo.nHeight;
		stConvertParam.pSrcData = pData;
		stConvertParam.nSrcDataLen = stImageInfo.nFrameLen;
		stConvertParam.enSrcPixelType = stImageInfo.enPixelType;
		stConvertParam.enDstPixelType = PixelType_Gvsp_RGB8_Packed;
		stConvertParam.pDstBuffer = pDataForRGB;
		stConvertParam.nDstBufferSize = nDataSizeForRGB;
		if (MV_OK != MV_CC_ConvertPixelType(handle, &stConvertParam))
		{
			printf("Convert Pixel Type fail!\n");
			return false;
		}
		RGB2BGR(pDataForRGB, pstImageInfo->nWidth, pstImageInfo->nHeight);
		srcImg = cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC3, pDataForRGB);
		//cv::imwrite("tmp.jpg", srcImg);
		//free(pDataForRGB);
		//pDataForRGB = NULL;
	}
	else
	{
		printf("unsupported pixel format\n");
		return false;
	}

	if (NULL == srcImg.data)
	{
		return false;
	}

	return true;;
}