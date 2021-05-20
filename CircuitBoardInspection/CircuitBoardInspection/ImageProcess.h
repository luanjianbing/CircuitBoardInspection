#pragma once

#include "common.h"

#include <opencv2/features2d.hpp>
#include <opencv2/opencv.hpp>

#include <ctime>
#include <thread>
#include <numeric>

typedef struct
{
	cv::Point2f left_top;
	cv::Point2f left_bottom;
	cv::Point2f right_top;
	cv::Point2f right_bottom;
}four_corners_t;
class ImageProcess
{
public:
	ImageProcess(){};
	ImageProcess(std::vector<std::vector<int>> &block, std::vector<std::vector<int>> &param, std::vector<std::vector<std::string>> &lengthPixel, 
		std::vector<std::vector<int>> &trueTargetPartAxis, std::vector<std::vector<int>> &posProcessParam, std::vector<std::vector<int>> &plugDirectMaskMsg,
		cv::Mat &norm);
	~ImageProcess();

	std::vector<int> blockProcessing(cv::Mat src);

	bool isSatisfied(cv::Mat &src, bool isBlack, int lengthNorm, int lengthOffset, double angle);

	cv::Mat lengthDetectAndDisplay(cv::Mat &src, bool isBlack, int lengthNorm, int lengthOffset, int &pixelNorm, int &pixelOffset, double &angle);

	cv::Rect findTrueArea(cv::Mat &src, int cateIdx, int color, int curBlockIdx);
	cv::RotatedRect findTapeDetailTrueArea(cv::Mat &src, int cateIdx, int color);

	cv::Mat posDetectAndDisplay(cv::Mat &startMat, int startCate, int startColor, cv::Point startAxis, cv::Mat &endMat, int endCate, int endColor, cv::Point endAxis,
		int posLengthNorm, int posLengthOffset, int &pixelNorm, int &pixelOffset);
	bool posIsSatisfied(cv::Mat &startMat, int startCate, int startColor, cv::Point startAxis, 
		cv::Mat &endMat, int endCate, int endColor, cv::Point endAxis, int pixelNorm, int pixelOffset);

	void setHSVColorForGetTrueTarget(std::unordered_map<std::string, HSVLimit> &new_HSVColorForGetTrueTarget);
	void setMaskForTargetBlock(const std::vector<cv::Mat> &new_maskForTargetBlock);
	void setMultiTempForTargetBlock(const std::map<int, std::list<cv::Mat>> &new_multiTempMat);
	void setCurModelName(std::string modelName);

	void matchTwoImage(cv::Mat &templateImage, cv::Mat &toMatchImage);

private:
	std::vector<std::vector<int>> block;
	std::vector<std::vector<int>> param;
	std::vector<std::vector<std::string>> lengthPixel;
	std::vector<std::vector<int>> trueTargetPartAxis;
	std::vector<std::vector<int>> posProcessParam;
	std::vector<std::vector<int>> plugDirectMaskMsg;
	cv::Mat norm;
	std::string curNewModelName = "";

	bool funcColorDetection(cv::Mat &partRoi, int idx, int cateIdx, int targetColor, int xOffset, int yOffset);
	bool funcLengthDetection(cv::Mat &partRoi, bool isBlack, int lengthPixelNorm, int lengthPixelOffset, double angle);
	bool funcPosDetection(cv::Mat &startMat, int startCate, int startColor, cv::Point startAxis,
		cv::Mat &endMat, int endCate, int endColor, cv::Point endAxis, int pixelNorm, int pixelOffset);
	bool funcHaveOrNotDetection(cv::Mat &partRoi, cv::Mat &normPartRoi, int idx, int xOffset, int yOffset);
	bool funcCateDetection(cv::Mat &partRoi, cv::Mat &normPartRoi, int idx, int xOffset, int yOffset);
	bool funcDirectDetection(cv::Mat &partRoi, cv::Mat &normPartRoi, int idx, int xOffset, int yOffset);
	double getSimByMask(cv::Mat &imgOrg, cv::Mat &imgComp, int idx, int xOffset, int yOffset);
	double getSimByOrgWithoutMask(cv::Mat &imgOrg, cv::Mat &imgComp, int idx, int xOffset, int yOffset);
	double getSimByTargetColorArea(cv::Mat &imgOrg, cv::Mat &imgComp, int idx, int xOffset, int yOffset);
	double Correlation(cv::Mat &tmplateImg, cv::Mat &toMatchImg);
	bool colorIsSaticfiedWithMask(cv::Mat &imgComp, cv::Mat &curMask, int idx, int cateIdx, int targetColor, int xOffset, int yOffset);
	int getTargetChannelAvgWithMask(cv::Mat &src, int chl, cv::Mat &cmpMask);
	int getTargetMaxNumGrayValWithMask(cv::Mat &src, int chl);

	void maskMatShow(cv::Mat &imgOrg, cv::Mat &imgComp, int idx, int xOffset, int yOffset);
	void setTrueTargetBlockScale(double scale);
	void continueTargetBlockScale(cv::Point &startPoint, cv::Point &endPoint, int idx, int xoffset, int yoffset, double scale);

	double ssimDetect(cv::Mat &imgOrg, cv::Mat &imgComp);

	int getLength(cv::Mat &src);	// 判断边界
	std::pair<int, int> getLengthForDisplay(cv::Mat &src);
	cv::Mat findLengthDisplay(cv::Mat &src, int &pixelNorm);

	void cutDstRoi(cv:: Mat &diffMask);
	void cutDstRoi(cv::Mat &diffMask, cv::Rect &rect);

	double DegreeTrans(double theta);
	void rotateImage(cv::Mat src, cv::Mat& img_rotate, double degree);
	double CalcDegree(const cv::Mat &srcImage, cv::Mat &dst);
	void RemoveSmallRegion(cv::Mat &Src, cv::Mat &Dst, int AreaLimit, int CheckMode, int NeihborMode);

	// [4]颜色规格值 :	1.扎带(花结束带[灰(0)、白(1)、粉(2)] / T18R[白(0)、黑(1)、黄(2)])	// 6种
	//					2.胶布(红(0)、白(1)、黑(2)、黄(3)、绿(4)、灰(5))		// 6种
	//					3.插头(黑(0)、白(1)、粉(2)、蓝(3)、灰(4)、绿(5)、黄(6))	// 7种
	std::unordered_map<std::string, HSVLimit> HSVColorForGetTrueTarget;
	std::vector<cv::Mat> maskForTargetBlock;
	std::map<int, std::list<cv::Mat>> multiTempMat;	// <block索引, 该block索引的多模板>
	//std::unordered_map<std::string, HSVLimit> HSVColorForGetTrueTarget =
	//{
	//	//{ "GrayOrWhite" ,{ 0, 180, 0, 43, 23, 255 } },
	//	////{ "Gray" ,{ 0, 180, 0, 43, 23, 221 } },
	//	////{ "White" ,{ 0, 180, 0, 30, 221, 255 } },
	//	//{ "Black" ,{ 0, 180, 0, 255, 0, 23 } },
	//	//{ "RedOrPink" ,{ 156, 180, 43, 255, 46, 255 } },	// 这里丢失了0开始的一部分
	//	//{ "Yellow" ,{ 26, 34, 43, 255, 46, 255 } },
	//	//{ "Green" ,{ 35, 77, 43, 255, 46, 255 } },
	//	//{ "Blue" ,{ 100, 124, 43, 255, 46, 255 } }
	//	{ "Gray", { 0, 180, 0, 43, 46, 220 } },
	//	{ "White", { 0, 180, 0, 30, 221, 255 } },
	//	{ "Pink",{ 156, 180, 43, 255, 46, 255 } },
	//	{ "Black",{ 0, 180, 0, 255, 0, 46 } },
	//	{ "Yellow",{ 26, 34, 43, 255, 46, 255 } },
	//	{ "Red",{ 156, 180, 43, 255, 46, 255 } },
	//	{ "Green",{ 35, 77, 43, 255, 46, 255 } },
	//	{ "Blue" ,{ 100, 124, 43, 255, 46, 255 } }
	//};
	void adjustMatChannel(cv::Mat& src, int chl);
	void myFloodFill(cv::Mat &src, int x, int y);
	void floodFillEdge(cv::Mat &src);
	cv::Rect getTargetColorRect(cv::Mat &src, std::string targetColor, int cateIdx, int curBlockIdx, cv::Mat &getMask);
	cv::RotatedRect getTapeDetailRect(cv::Mat &src, std::string targetColor, int cateIdx);
	double getDiffFromMatHCh(cv::Mat &h_mat, cv::Mat &curMask);	// 求直方图的方差用以区分胶布的黑色和其他
	
	four_corners_t corners;
	void CalcCorners(const cv::Mat& H, const cv::Mat& src);
	double CalcDegreeBlackSence(const cv::Mat &srcImage, cv::Mat &dst);

	cv::Rect getRealTimePosition(cv::Mat &partRoi, cv::Mat &temp);
	cv::Mat imgTranslate(cv::Mat &matSrc, int xOffset, int yOffset, bool bScale);

	double multiTemplateMatch(cv::Mat &curNorm, cv::Mat &curPartRoi, int idx);
	bool multiTempColorMatch(cv::Mat &partRoi, int idx, int cateIdx, int targetColor);

	bool isUsingMultiThreading = false;	// 是否用多线程来处理
	void processInInterval(cv::Mat &src, int startIdx, int endIdx, std::vector<int> &res);

	double getSimByTargetColorAreaWithoutMask(cv::Mat &imgOrg, cv::Mat &imgComp, int idx);

	void getTargetMatWithDefineMask(cv::Mat &src, int idx);

	cv::Mat nearestNeighRotate(cv::Mat img, float angle);
	double getPlugDirection(cv::Mat &imgOrg, cv::Mat &imgComp, int idx, int xOffset, int yOffset);

	void rotateImageBlackSence(cv::Mat src, cv::Mat& img_rotate, double degree);
	void cutDstRoiGray(cv::Mat &diffMask, cv::Rect &rect);
	cv::Mat myDft(cv::Mat &img_src);
	double getBlackTapeDFT(cv::Mat &imgOrg, cv::Mat &imgComp, int idx, int xOffset, int yOffse);
	double getPlugDirectionTopHat(cv::Mat &imgOrg, cv::Mat &imgComp, int idx, int xOffset, int yOffset);
};


