#include "ImageProcess.h"

ImageProcess::ImageProcess(std::vector<std::vector<int>> &block, std::vector<std::vector<int>> &param, 
	std::vector<std::vector<std::string>> &lengthPixel, std::vector<std::vector<int>> &trueTargetPartAxis, 
	std::vector<std::vector<int>> &posProcessParam, std::vector<std::vector<int>> &plugDirectMaskMsg, cv::Mat &norm)
	: block(block), param(param), lengthPixel(lengthPixel), 
	trueTargetPartAxis(trueTargetPartAxis), posProcessParam(posProcessParam), 
	plugDirectMaskMsg(plugDirectMaskMsg), norm(norm)
{

}

std::vector<int> ImageProcess::blockProcessing(cv::Mat src) {
	std::vector<int> res(block.size());
	//cv::imwrite("src.jpg", src);
	// [1]区域编号 : (int)
	// [2]区域类型 : 花结束带(0) / T18R(1) / 胶布(2) / 插头(3)
	// [3]颜色检测 : 否(0) / 是(1)
	// [4]颜色规格值 :	1.扎带(花结束带[灰(0)、白(1)、粉(2)] / T18R[白(0)、黑(1)、黄(2)])	// 6种
	//					2.胶布(红(0)、白(1)、黑(2)、黄(3)、绿(4)、灰(5))		// 6种
	//					3.插头(黑(0)、白(1)、粉(2)、蓝(3)、灰(4)、绿(5)、黄(6))	// 7种
	// [5]长度限制检测(指胶布本身长度) : 否(0) / 是(1)
	// [6]长度规格值 : (int) 单位为mm > 0
	// [7]长度偏差值 : (int) 单位为mm > 0
	// [8]位置检测 : 否(0) / 是(1)
	// [9]起始ID :	从起始ID至当前ID的距离，(int)
	// [10]位置规格值 : (int) 单位为mm > 0
	// [11]位置偏差值 : (int) 单位为mm > 0
	// [12]有无检测 : 否(0) / 是(1)
	// [13]类型检测 : 否(0) / 是(1)
	// [14]方向检测 : 否(0) / 是(1)
	
	// 进行配准
	//clock_t start = clock();
	matchTwoImage(norm, src);
	//std::cout << "Total match: " << ((double)clock() - start) / CLOCKS_PER_SEC << std::endl;
	// cv::imwrite("hasMatch.jpg", src);
	// 扩大trueTargetBlock的大小
	setTrueTargetBlockScale(1.0);

	clock_t start = clock();
	if (isUsingMultiThreading) {	// 多线程处理
		int perIntvl = (block.size() - 1) / 4;
		auto lamThread_1 = [&] {
			processInInterval(src, 1, perIntvl, res);	// 第一个线程从i=1开始
		};
		auto lamThread_2 = [&] {
			processInInterval(src, perIntvl + 1, 2 * perIntvl, res);
		};
		auto lamThread_3 = [&] {
			processInInterval(src, 2 * perIntvl + 1, 3 * perIntvl, res);
		};
		auto lamThread_4 = [&] {
			processInInterval(src, 3 * perIntvl + 1, block.size() - 1, res);	// 最后一个线程到i=block.size()-1结束
		};

		std::thread m_thread_1(lamThread_1);
		std::thread m_thread_2(lamThread_2);
		std::thread m_thread_3(lamThread_3);
		std::thread m_thread_4(lamThread_4);
		m_thread_1.join();
		m_thread_2.join();
		m_thread_3.join();
		m_thread_4.join();
	}
	else {	// 单线程处理
		processInInterval(src, 1, block.size() - 1, res);// 对每个区域进行处理，从i=1开始
	}
	std::cout << "Time Consume: " << ((double)(clock()) - start) / CLOCKS_PER_SEC << std::endl;
	return res;
}

void ImageProcess::processInInterval(cv::Mat &src, int startIdx, int endIdx, std::vector<int> &res) {
	int colorDetection = 0;	// 颜色检测
	int lengthDetection = 0;	// 长度检测
	int posDetection = 0;	// 位置检测
	int haveOrNotDetection = 0;	// 有无检测
	int cateDetection = 0;	// 类型检测
	int directDetection = 0;	// 方向检测

	bool colorDetectionRes = true;
	bool lengthDetectionRes = true;
	bool posDetectionRes = true;
	bool haveOrNotDetectionRes = true;
	bool cateDetectionRes = true;
	bool directDetectionRes = true;
	for (int i = startIdx; i <= endIdx; ++i) {	
		colorDetection = param[i][2];
		lengthDetection = param[i][4];
		posDetection = param[i][7];
		haveOrNotDetection = param[i][11];
		cateDetection = param[i][12];
		directDetection = param[i][13];

		colorDetectionRes = true;
		lengthDetectionRes = true;
		posDetectionRes = true;
		haveOrNotDetectionRes = true;
		cateDetectionRes = true;
		directDetectionRes = true;

		// 获取局部roi
		cv::Mat partRoi = src(cv::Range(block[i][1], block[i][3]),
			cv::Range(block[i][0], block[i][2]));
		cv::Mat normPartRoi = norm(cv::Range(block[i][1], block[i][3]),
			cv::Range(block[i][0], block[i][2]));
		// setTrueTargetBlockScale(1.1);
		cv::Mat partRoiClone = partRoi.clone();
		cv::Mat temp = normPartRoi(cv::Range(trueTargetPartAxis[i][1], trueTargetPartAxis[i][3]),
			cv::Range(trueTargetPartAxis[i][0], trueTargetPartAxis[i][2]));
		cv::imwrite(".\\test\\matchMat\\temp" + std::to_string(i) + ".jpg", temp);
		//start = clock();
		cv::Rect rect = getRealTimePosition(partRoiClone, temp);
		//std::cout << "Part match: " << ((double)clock() - start) / CLOCKS_PER_SEC << std::endl;
		cv::rectangle(partRoiClone, rect, cv::Scalar(0, 255, 0), 2, 8, 0);
		cv::imwrite(".\\test\\matchMat\\match" + std::to_string(i) + ".jpg", partRoiClone);
		int xOffset = trueTargetPartAxis[i][0] - rect.x;
		int yOffset = trueTargetPartAxis[i][1] - rect.y;
		//cv::Mat matAfterTranslate = imgTranslate(partRoiClone, xOffset, yOffset, false);
		//cv::imwrite(".\\test\\matchMat\\trans" + std::to_string(i) + ".jpg", partRoiClone);

		//// maskMat显示
		maskMatShow(normPartRoi, partRoi, i, xOffset, yOffset);

		std::cout << "[" << i << "] ";
		// cv::imwrite("./partRoi/" + std::to_string(i) + ".jpg", partRoi);

		if (colorDetection) {	// 颜色检测
			std::cout << "[colorDetection]-[";
			colorDetectionRes = funcColorDetection(partRoi, i, param[i][1], param[i][3], xOffset, yOffset);
			std::cout << " res:" << colorDetectionRes << " ] ";
		}
		if (lengthDetection) {	// 长度检测（针对胶布本身的长度而言）
			std::cout << "[lengthDetection]-[";
			bool isBlack = (param[i][3] == 2 ? true : false);	// 这里仅考虑了黑色，灰色后面考虑
			int lengthNorm = std::atoi(lengthPixel[i][0].c_str());
			int lengthOffset = std::atoi(lengthPixel[i][1].c_str());
			double angle = std::atof(lengthPixel[i][2].c_str());
			lengthDetectionRes = funcLengthDetection(partRoi, isBlack, lengthNorm, lengthOffset, angle);
			std::cout << " res:" << lengthDetectionRes << " ] ";
		}
		if (posDetection) {	// 位置检测
			std::cout << "[posDetection]-[";
			int pixelNorm = posProcessParam[i][0];
			int pixelOffset = posProcessParam[i][1];
			int startIdx = param[i][8];
			cv::Mat startMat = src(cv::Range(block[startIdx][1], block[startIdx][3]), cv::Range(block[startIdx][0], block[startIdx][2]));
			posDetectionRes = funcPosDetection(startMat, param[startIdx][1], param[startIdx][3], cv::Point(block[startIdx][0], block[startIdx][1]),
				partRoi, param[i][1], param[i][3], cv::Point(block[i][0], block[i][1]), pixelNorm, pixelOffset);
			std::cout << " res:" << posDetectionRes << " ] ";
			//cv::imwrite("./src.jpg", src);
		}
		if (haveOrNotDetection) {	// 有无检测
			std::cout << "[haveOrNotDetection]-[";
			haveOrNotDetectionRes = funcHaveOrNotDetection(partRoi, normPartRoi, i, xOffset, yOffset);
			std::cout << " res:" << haveOrNotDetectionRes << " ] ";
		}
		if (cateDetection) {	// 类型检测
			std::cout << "[cateDetection]-[";
			cateDetectionRes = funcCateDetection(partRoi, normPartRoi, i, xOffset, yOffset);
			std::cout << " res:" << cateDetectionRes << " ] ";
		}
		if (directDetection) {	// 方向检测
			std::cout << "[directDetection]-[";
			directDetectionRes = funcDirectDetection(partRoi, normPartRoi, i, xOffset, yOffset);
			std::cout << " res:" << directDetectionRes << " ] ";
		}

		std::cout << "[ " << colorDetectionRes << " " << lengthDetectionRes << " " << posDetectionRes
			<< " " << haveOrNotDetectionRes << " " << cateDetectionRes << " " << directDetectionRes << " ] ";
		res[i] = colorDetectionRes & lengthDetectionRes & posDetectionRes
			& haveOrNotDetectionRes & cateDetectionRes & directDetectionRes;

		std::cout << std::endl;
	}
}

bool ImageProcess::funcColorDetection(cv::Mat &partRoi, int idx, int cateIdx, int targetColor, int xOffset, int yOffset) {
	cv::Mat curMask = maskForTargetBlock[idx];
	bool singleRes = colorIsSaticfiedWithMask(partRoi, curMask, idx, cateIdx, targetColor, xOffset, yOffset);
	if (!singleRes && multiTempMat.count(idx)) {	// 当颜色检测错误并且存在多模板的时候进入
		if (multiTempColorMatch(partRoi, idx, cateIdx, targetColor))
			return true;
	}
	return singleRes;
}

bool ImageProcess::funcLengthDetection(cv::Mat &partRoi, bool isBlack, int lengthPixelNorm, int lengthPixelOffset, double angle) {
	return isSatisfied(partRoi, isBlack, lengthPixelNorm, lengthPixelOffset, angle);
}

bool ImageProcess::funcPosDetection(cv::Mat &startMat, int startCate, int startColor, cv::Point startAxis,
	cv::Mat &endMat, int endCate, int endColor, cv::Point endAxis, int pixelNorm, int pixelOffset) {
	return posIsSatisfied(startMat, startCate, startColor, startAxis,
		endMat, endCate, endColor, endAxis, pixelNorm, pixelOffset);
}

bool ImageProcess::funcHaveOrNotDetection(cv::Mat &partRoi, cv::Mat &normPartRoi, int idx, int xOffset, int yOffset) {
	//double res = ssimDetect(normPartRoi, partRoi);
	double res = 0.0;
	// double res = getSimByTargetColorArea(normPartRoi, partRoi, idx, xOffset, yOffset);
	 std::cout << " similarity:" << res;
	 if (param[idx][1] == 2 && param[idx][3] == 2) {	// 黑胶布的有无通过dft判断(主要是32位置灰线)
		 res = getBlackTapeDFT(normPartRoi, partRoi, idx, xOffset, yOffset);
	 }
	 else {
		 res = getSimByMask(normPartRoi, partRoi, idx, xOffset, yOffset);
	 }
	 //std::cout << "funcHaveOrNotDetection[" << res << "]" << std::endl;
	//std::cout << " similarity:" << res;
	if (res < 0.45)
		return false;

	return true;
}

bool ImageProcess::funcCateDetection(cv::Mat &partRoi, cv::Mat &normPartRoi, int idx, int xOffset, int yOffset) {
	//double res = ssimDetect(normPartRoi, partRoi);
	// double res = getSimByMask(normPartRoi, partRoi, idx, xOffset, yOffset);
	double res = getSimByOrgWithoutMask(normPartRoi, partRoi, idx, xOffset, yOffset);
	std::cout << " similarity:" << res;
	if (param[idx][1] == 1 && param[idx][3] == 0) {	// 对于T18R白色多加一个判断
		double resArea = getSimByTargetColorAreaWithoutMask(normPartRoi, partRoi, idx);
		std::cout << " resArea:" << resArea;
		if (resArea < 0.55) {
			return false;
		}
	}

	if (res < 0.75) {
		if (multiTempMat.count(idx)) {
			double multiTempRes = multiTemplateMatch(normPartRoi, partRoi, idx);
			std::cout << " multiTemplate:" << multiTempRes;
			res = MAX(res, multiTempRes);	// 取最大的
		}
	}

	if (param[idx][1] == 0 && param[idx][3] == 0) {	// 针对灰色花结束带做个限制
		if (res < 0.90)
			return false;
	}
	else {
		if (res < 0.75)
			return false;
	}
	

	return true;
}

bool ImageProcess::funcDirectDetection(cv::Mat &partRoi, cv::Mat &normPartRoi, int idx, int xOffset, int yOffset) {
	//double res = ssimDetect(normPartRoi, partRoi);
	// double res = getSimByMask(normPartRoi, partRoi, idx, xOffset, yOffset);
	double res = 0.0;
	if (param[idx][1] == 3) {	// 针对插头方向检测（不使用多模板）
		// res = getPlugDirection(normPartRoi, partRoi, idx, xOffset, yOffset);
		res = getPlugDirectionTopHat(normPartRoi, partRoi, idx, xOffset, yOffset);
		std::cout << " plugSimilarity:" << res;
	}
	else {
		res = getSimByOrgWithoutMask(normPartRoi, partRoi, idx, xOffset, yOffset);
		//std::cout << "funcDirectDetection[" << res << "]" << std::endl;
		std::cout << " similarity:" << res;
		if (multiTempMat.count(idx)) {
			double multiTempRes = multiTemplateMatch(normPartRoi, partRoi, idx);
			std::cout << " multiTemplate:" << multiTempRes;
			res = MAX(res, multiTempRes);	// 取最大的
		}
	}
	if (res < 0.95)
		return false;

	return true;
}

bool ImageProcess::multiTempColorMatch(cv::Mat &partRoi, int idx, int cateIdx, int targetColor) {
	cv::Mat imgCompCopy;
	cv::cvtColor(partRoi, imgCompCopy, cv::COLOR_BGR2RGB);
	//cv::cvtColor(imgCompCopy, imgCompCopy, cv::COLOR_RGB2BGR);
	std::string trueColor = "";
	switch (cateIdx)
	{
	case 0: {// 花结束带
		if (targetColor < 2) {	// 灰/白
			if (targetColor == 0)
				trueColor = "Gray";
			else
				trueColor = "White";
		}
		else {	// 粉
			trueColor = "Pink";
		}
		break;
	}
	case 1: {// T18R
		switch (targetColor)
		{
		case 0: {	// 白
			trueColor = "White";
			break;
		}
		case 1: {	// 黑
			trueColor = "Black";
			break;
		}
		case 2: {	// 黄
			trueColor = "Yellow";
			break;
		}
		default:
			break;
		}
		break;
	}
	case 2: {// 胶布
			 //// 红(0)、白(1)、黑(2)、黄(3)、绿(4)、灰(5)
		if (targetColor == 0) {	// 红
			trueColor = "Red";
		}
		else if (targetColor == 1 || targetColor == 5) {// 灰/白
			if (targetColor == 5)
				trueColor = "Gray";
			else
				trueColor = "White";
		}
		else if (targetColor == 2) {	// 黑
			trueColor = "Black";
		}
		else if (targetColor == 3) {	// 黄
			trueColor = "Yellow";
		}
		else if (targetColor == 4) {	// 绿
			trueColor = "Green";
		}
		break;
	}
	case 3: {// 插头
		if (targetColor == 0) {	// 黑
			trueColor = "Black";
		}
		else if (targetColor == 1 || targetColor == 4) {// 灰/白
			if (targetColor == 1)
				trueColor = "White";
			else
				trueColor = "Gray";
		}
		else if (targetColor == 2) {	// 粉
			trueColor = "Pink";
		}
		else if (targetColor == 3) {	// 蓝
			trueColor = "Blue";
		}
		else if (targetColor == 5) {	// 绿
			trueColor = "Green";
		}
		else if (targetColor == 6) {	// 黄
			trueColor = "Yellow";
		}
		break;
	}
	default:
		break;
	}
	// 拉伸替换
	cv::Mat gray;
	cv::cvtColor(imgCompCopy, gray, cv::COLOR_RGB2GRAY);
	adjustMatChannel(gray, 0);
	cv::Mat hsv;
	cv::cvtColor(imgCompCopy, hsv, cv::COLOR_RGB2HSV);
	int nRows = hsv.rows;
	int nCols = hsv.cols;
	uchar *pDataMat;
	for (int j = 0; j < nRows; j++)
	{
		pDataMat = gray.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			hsv.at<cv::Vec3b>(j, i)[2] = pDataMat[i];
		}
	}

	for (auto iter = multiTempMat[idx].begin(); iter != multiTempMat[idx].end(); ++iter) {
		cv::Mat thisTemp = (*iter).clone();	// 多模板
		cv::Mat curMask;
		
		cv::Mat thisTempCvt;
		cv::cvtColor(thisTemp, thisTempCvt, cv::COLOR_BGR2RGB);
		cv::Rect targetRect = getTargetColorRect(thisTempCvt, trueColor, cateIdx, -1, curMask);
		
		cv::Mat realTimeMat = partRoi.clone();	// 实时图
		cv::Mat temp = thisTemp(cv::Range(targetRect.y, targetRect.y + targetRect.height),
			cv::Range(targetRect.x, targetRect.x + targetRect.width));
		cv::Rect rect = getRealTimePosition(realTimeMat, temp);
		int xOffset = targetRect.x - rect.x;
		int yOffset = targetRect.y - rect.y;
		
		cv::Mat tmpHSV = imgTranslate(hsv, xOffset, yOffset, false);
		//std::cout << tmpHSV.size() << " " << curMask.size() << std::endl;
		cv::Mat toMatImg = tmpHSV & curMask;
		//cv::imshow("toMatImg", toMatImg);
		//cv::waitKey(0);
		// 分割通道
		std::vector<cv::Mat> mv;
		cv::split(toMatImg, mv);
		float s = 0.3;
		//bool tmpRes = false;
		if (trueColor == "Black") {
			// int avgV = getTargetChannelAvgWithMask(mv[2], 0);
			int avgV = getTargetMaxNumGrayValWithMask(mv[2], 0);
			std::cout << " |avgV:" << avgV << "|";
			//s = 0.4;
			int offsetV = static_cast<int>(static_cast<float>(HSVColorForGetTrueTarget[trueColor].vMax - HSVColorForGetTrueTarget[trueColor].vMin) * s);
			int minThrdV = (HSVColorForGetTrueTarget[trueColor].vMin - offsetV) >= 0 ? (HSVColorForGetTrueTarget[trueColor].vMin - offsetV) : 0;
			int maxThrdV = (HSVColorForGetTrueTarget[trueColor].vMax + offsetV) <= 255 ? (HSVColorForGetTrueTarget[trueColor].vMax + offsetV) : 255;
			if (avgV >= minThrdV && avgV <= maxThrdV) {
				if (cateIdx == 2) {	// 对于黑胶布做额外一次检测
					double thisStd = getDiffFromMatHCh(mv[0], curMask);
					std::cout << " |stdev:" << thisStd << "|";
					if (thisStd < 400.0)	// 测试红胶布的方差大于500
						return true;
				}
				else
					return true;
			}
		}
		else if (trueColor == "White" || trueColor == "Gray") {
			int avgS = getTargetChannelAvgWithMask(mv[1], 0, curMask);
			int avgV = getTargetChannelAvgWithMask(mv[2], 0, curMask);
			//int avgS = getTargetMaxNumGrayValWithMask(mv[1], 0);
			//int avgV = getTargetMaxNumGrayValWithMask(mv[2], 0);
			std::cout << " |avgS:" << avgS;
			std::cout << " avgV:" << avgV << "|";
			if (trueColor == "Gray") s = 0.5;
			int offsetS = static_cast<int>(static_cast<float>(HSVColorForGetTrueTarget[trueColor].sMax - HSVColorForGetTrueTarget[trueColor].sMin) * s);
			int minThrdS = (HSVColorForGetTrueTarget[trueColor].sMin - offsetS) >= 0 ? (HSVColorForGetTrueTarget[trueColor].sMin - offsetS) : 0;
			int maxThrdS = (HSVColorForGetTrueTarget[trueColor].sMax + offsetS) <= 255 ? (HSVColorForGetTrueTarget[trueColor].sMax + offsetS) : 255;
			int offsetV = static_cast<int>(static_cast<float>(HSVColorForGetTrueTarget[trueColor].vMax - HSVColorForGetTrueTarget[trueColor].vMin) * s);
			int minThrdV = (HSVColorForGetTrueTarget[trueColor].vMin - offsetV) >= 0 ? (HSVColorForGetTrueTarget[trueColor].vMin - offsetV) : 0;
			int maxThrdV = (HSVColorForGetTrueTarget[trueColor].vMax + offsetV) <= 255 ? (HSVColorForGetTrueTarget[trueColor].vMax + offsetV) : 255;
			if ((avgS >= minThrdS && avgS <= maxThrdS)	
				&& (avgV >= minThrdV && avgV <= maxThrdV)) {
				return true;
			}
		}
		else {
			//if (idx == 5)
			//	cv::imshow("mv[0]", mv[0]);
			int avgH = getTargetChannelAvgWithMask(mv[0], 0, curMask);
			int avgS = getTargetChannelAvgWithMask(mv[1], 0, curMask);
			//int avgH = getTargetMaxNumGrayValWithMask(mv[0], 0);
			//int avgS = getTargetMaxNumGrayValWithMask(mv[1], 0);
			std::cout << " |avgH:" << avgH;
			std::cout << " avgS:" << avgS << "|";
			double sH = 1.3;	// 彩色饱和度单独一个scale
			int offsetH = static_cast<int>(static_cast<float>(HSVColorForGetTrueTarget[trueColor].hMax - HSVColorForGetTrueTarget[trueColor].hMin) * sH);
			int minThrdH = (HSVColorForGetTrueTarget[trueColor].hMin - offsetH) >= 0 ? (HSVColorForGetTrueTarget[trueColor].hMin - offsetH) : 0;
			int maxThrdH = (HSVColorForGetTrueTarget[trueColor].hMax + offsetH) <= 180 ? (HSVColorForGetTrueTarget[trueColor].hMax + offsetH) : 180;
			int offsetS = static_cast<int>(static_cast<float>(HSVColorForGetTrueTarget[trueColor].sMax - HSVColorForGetTrueTarget[trueColor].sMin) * s);
			int minThrdS = (HSVColorForGetTrueTarget[trueColor].sMin - offsetS) >= 0 ? (HSVColorForGetTrueTarget[trueColor].sMin - offsetS) : 0;
			int maxThrdS = (HSVColorForGetTrueTarget[trueColor].sMax + offsetS) <= 255 ? (HSVColorForGetTrueTarget[trueColor].sMax + offsetS) : 255;
			if ((avgH >= minThrdH && avgH <= maxThrdH)
				&& (avgS >= minThrdS && avgS <= maxThrdS)) {
				return true;
			}
		}
	}
	return false;
}

void ImageProcess::maskMatShow(cv::Mat &imgOrg, cv::Mat &imgComp, int idx, int xOffset, int yOffset) {
	cv::Mat curMask = maskForTargetBlock[idx];
	cv::Mat tempImg = imgOrg & curMask;
	cv::Mat toMatImg = imgTranslate(imgComp, xOffset, yOffset, false);
	toMatImg = toMatImg & curMask;
	cv::imwrite(".\\test\\maskMat\\tempImg" + std::to_string(idx) + ".jpg", tempImg);
	cv::imwrite(".\\test\\maskMat\\toMatImg" + std::to_string(idx) + ".jpg", toMatImg);
}

cv::Rect ImageProcess::getRealTimePosition(cv::Mat &partRoi, cv::Mat &temp) {
	// temp: 模板
	// partRoi: 待匹配图像
	cv::Mat result;
	//cv::matchTemplate(partRoi, temp, result, cv::TM_CCOEFF_NORMED);
	cv::matchTemplate(partRoi, temp, result, cv::TM_CCORR_NORMED);
	//cv::imwrite(".\\test\\matchResult\\result" + std::to_string(idx) + ".jpg", result);
	double minVal = -1, maxVal = 0;
	cv::Point minLoc, maxLoc, matchLoc;
	cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
	matchLoc = maxLoc;
	//matchLoc = minLoc;	// 取大值（视匹配方法而定）
	// std::cout << " maxVal:" << maxVal << std::endl;
	cv::Rect rect = cv::Rect(matchLoc.x, matchLoc.y, temp.cols, temp.rows);
	return rect;
}

void ImageProcess::setTrueTargetBlockScale(double scale) {
	if (scale > 1.0) {	// 只放大
		int thisOffsetWidth = 0;
		int thisOffsetHeight = 0;
		double scaleOffset = (scale - 1.0) / 2.0;
		// std::cout << scaleOffset << std::endl;
		for (int i = 0; i < trueTargetPartAxis.size(); ++i) {
			thisOffsetWidth = static_cast<int>(static_cast<double>(trueTargetPartAxis[i][2] - trueTargetPartAxis[i][0]) * scaleOffset);
			thisOffsetHeight = static_cast<int>(static_cast<double>(trueTargetPartAxis[i][3] - trueTargetPartAxis[i][1]) * scaleOffset);
			// std::cout << trueTargetPartAxis[i][0] << " " << trueTargetPartAxis[i][1] << " " << trueTargetPartAxis[i][2] << " " << trueTargetPartAxis[i][3] << std::endl;
			trueTargetPartAxis[i][0] = (trueTargetPartAxis[i][0] - thisOffsetWidth) > 0 
				? (trueTargetPartAxis[i][0] - thisOffsetWidth) : 0;
			trueTargetPartAxis[i][1] = (trueTargetPartAxis[i][1] - thisOffsetHeight) > 0 
				? (trueTargetPartAxis[i][1] - thisOffsetHeight) : 0;
			trueTargetPartAxis[i][2] = (trueTargetPartAxis[i][2] + thisOffsetWidth) < (block[i][2] - block[i][0]) 
				? (trueTargetPartAxis[i][2] + thisOffsetWidth) : (block[i][2] - block[i][0]);
			trueTargetPartAxis[i][3] = (trueTargetPartAxis[i][3] + thisOffsetHeight) < (block[i][3] - block[i][1]) 
				? (trueTargetPartAxis[i][3] + thisOffsetHeight) : (block[i][3] - block[i][1]);
			// std::cout << trueTargetPartAxis[i][0] << " " << trueTargetPartAxis[i][1] << " " << trueTargetPartAxis[i][2] << " " << trueTargetPartAxis[i][3] << std::endl;
		}
	}
	else
		return;
}

bool ImageProcess::colorIsSaticfiedWithMask(cv::Mat &imgComp, cv::Mat &curMask, int idx, int cateIdx, int targetColor, int xOffset, int yOffset) {
	// cv::Mat curMask = maskForTargetBlock[idx];
	//cv::Mat toMatImg = imgComp & curMask;	// 即时图像计算区域
	//cv::imshow("toMatImg", toMatImg);
	cv::Mat imgCompCopy;
	cv::cvtColor(imgComp, imgCompCopy, cv::COLOR_BGR2RGB);
	//cv::cvtColor(imgCompCopy, imgCompCopy, cv::COLOR_RGB2BGR);
	//if (idx == 20) {
	//	cv::imshow("imgCompCopy", imgCompCopy);
	//	cv::waitKey(0);
	//}
	std::string trueColor = "";
	switch (cateIdx)
	{
	case 0: {// 花结束带
		if (targetColor < 2) {	// 灰/白
			if (targetColor == 0)
				trueColor = "Gray";
			else
				trueColor = "White";
		}
		else {	// 粉
			trueColor = "Pink";
		}
		break;
	}
	case 1: {// T18R
		switch (targetColor)
		{
		case 0: {	// 白
			trueColor = "White";
			break;
		}
		case 1: {	// 黑
			trueColor = "Black";
			break;
		}
		case 2: {	// 黄
			trueColor = "Yellow";
			break;
		}
		default:
			break;
		}
		break;
	}
	case 2: {// 胶布
			 //// 红(0)、白(1)、黑(2)、黄(3)、绿(4)、灰(5)
		if (targetColor == 0) {	// 红
			trueColor = "Red";
		}
		else if (targetColor == 1 || targetColor == 5) {// 灰/白
			if (targetColor == 5)
				trueColor = "Gray";
			else
				trueColor = "White";
		}
		else if (targetColor == 2) {	// 黑
			trueColor = "Black";
		}
		else if (targetColor == 3) {	// 黄
			trueColor = "Yellow";
		}
		else if (targetColor == 4) {	// 绿
			trueColor = "Green";
		}
		break;
	}
	case 3: {// 插头
		if (targetColor == 0) {	// 黑
			trueColor = "Black";
		}
		else if (targetColor == 1 || targetColor == 4) {// 灰/白
			if (targetColor == 1)
				trueColor = "White";
			else
				trueColor = "Gray";
		}
		else if (targetColor == 2) {	// 粉
			trueColor = "Pink";
		}
		else if (targetColor == 3) {	// 蓝
			trueColor = "Blue";
		}
		else if (targetColor == 5) {	// 绿
			trueColor = "Green";
		}
		else if (targetColor == 6) {	// 黄
			trueColor = "Yellow";
		}
		break;
	}
	default:
		break;
	}
	// 拉伸替换
	cv::Mat gray;
	cv::cvtColor(imgCompCopy, gray, cv::COLOR_RGB2GRAY);
	adjustMatChannel(gray, 0);
	cv::Mat hsv;
	cv::cvtColor(imgCompCopy, hsv, cv::COLOR_RGB2HSV);
	int nRows = hsv.rows;
	int nCols = hsv.cols;
	uchar *pDataMat;
	for (int j = 0; j < nRows; j++)
	{
		pDataMat = gray.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			hsv.at<cv::Vec3b>(j, i)[2] = pDataMat[i];
		}
	}
	//if (idx == 5)
	// cv::imshow("imgComp", imgCompCopy);
	// 取出目标
	hsv = imgTranslate(hsv, xOffset, yOffset, false);
	cv::Mat toMatImg = hsv & curMask;
	// 分割通道
	std::vector<cv::Mat> mv;
	cv::split(toMatImg, mv);
	float s = 0.4;
	if (trueColor == "Black") {
		//if (idx == 20 || idx == 30)
		//	getDiffFromMatHCh(mv[0], curMask);
			//cv::imshow("mv[0]", mv[0]);
		// int avgV = getTargetChannelAvgWithMask(mv[2], 0);
		int avgV = getTargetMaxNumGrayValWithMask(mv[2], 0);
		std::cout << " avgV:" << avgV;
		//s = 0.4;
		int offsetV = static_cast<int>(static_cast<float>(HSVColorForGetTrueTarget[trueColor].vMax - HSVColorForGetTrueTarget[trueColor].vMin) * s);
		int minThrdV = (HSVColorForGetTrueTarget[trueColor].vMin - offsetV) >= 0 ? (HSVColorForGetTrueTarget[trueColor].vMin - offsetV) : 0;
		int maxThrdV = (HSVColorForGetTrueTarget[trueColor].vMax + offsetV) <= 255 ? (HSVColorForGetTrueTarget[trueColor].vMax + offsetV) : 255;
		
		if (avgV >= minThrdV && avgV <= maxThrdV) {
			if (cateIdx == 2) {	// 对于黑胶布做额外一次检测
				double thisStd = getDiffFromMatHCh(mv[0], curMask);
				std::cout << " |stdev:" << thisStd << "|";
				if (thisStd < 400.0)	// 测试红胶布的方差大于500
					return true;
			}
			else
				return true;
		}
	}
	else if (trueColor == "White" || trueColor == "Gray") {
		int avgS = getTargetChannelAvgWithMask(mv[1], 0, curMask);
		int avgV = getTargetChannelAvgWithMask(mv[2], 0, curMask);
		//int avgS = getTargetMaxNumGrayValWithMask(mv[1], 0);
		//int avgV = getTargetMaxNumGrayValWithMask(mv[2], 0);
		std::cout << " avgS:" << avgS;
		std::cout << " avgV:" << avgV;
		if (trueColor == "Gray") s = 0.5;
		int offsetS = static_cast<int>(static_cast<float>(HSVColorForGetTrueTarget[trueColor].sMax - HSVColorForGetTrueTarget[trueColor].sMin) * s);
		int minThrdS = (HSVColorForGetTrueTarget[trueColor].sMin - offsetS) >= 0 ? (HSVColorForGetTrueTarget[trueColor].sMin - offsetS) : 0;
		int maxThrdS = (HSVColorForGetTrueTarget[trueColor].sMax + offsetS) <= 255 ? (HSVColorForGetTrueTarget[trueColor].sMax + offsetS) : 255;
		int offsetV = static_cast<int>(static_cast<float>(HSVColorForGetTrueTarget[trueColor].vMax - HSVColorForGetTrueTarget[trueColor].vMin) * s);
		int minThrdV = (HSVColorForGetTrueTarget[trueColor].vMin - offsetV) >= 0 ? (HSVColorForGetTrueTarget[trueColor].vMin - offsetV) : 0;
		int maxThrdV = (HSVColorForGetTrueTarget[trueColor].vMax + offsetV) <= 255 ? (HSVColorForGetTrueTarget[trueColor].vMax + offsetV) : 255;
		if ((avgS >= minThrdS && avgS <= maxThrdS)
			&& (avgV >= minThrdV && avgV <= maxThrdV)) {
			return true;
		}
	}
	else {
		//if (idx == 8)
		//	getDiffFromMatHCh(mv[0], curMask);
		//if (idx == 8)
		//	cv::imshow("mv[0]", mv[0]);
		int avgH = getTargetChannelAvgWithMask(mv[0], 0, curMask);
		int avgS = getTargetChannelAvgWithMask(mv[1], 0, curMask);
		//int avgH = getTargetMaxNumGrayValWithMask(mv[0], 0);
		//int avgS = getTargetMaxNumGrayValWithMask(mv[1], 0);
		std::cout << " avgH:" << avgH;
		std::cout << " avgS:" << avgS;
		double sH = 0.85;	// 彩色饱和度单独一个scale
		int offsetH = static_cast<int>(static_cast<float>(HSVColorForGetTrueTarget[trueColor].hMax - HSVColorForGetTrueTarget[trueColor].hMin) * sH);
		int minThrdH = (HSVColorForGetTrueTarget[trueColor].hMin - offsetH) >= 0 ? (HSVColorForGetTrueTarget[trueColor].hMin - offsetH) : 0;
		int maxThrdH = (HSVColorForGetTrueTarget[trueColor].hMax + offsetH) <= 180 ? (HSVColorForGetTrueTarget[trueColor].hMax + offsetH) : 180;
		int offsetS = static_cast<int>(static_cast<float>(HSVColorForGetTrueTarget[trueColor].sMax - HSVColorForGetTrueTarget[trueColor].sMin) * s);
		int minThrdS = (HSVColorForGetTrueTarget[trueColor].sMin - offsetS) >= 0 ? (HSVColorForGetTrueTarget[trueColor].sMin - offsetS) : 0;
		int maxThrdS = (HSVColorForGetTrueTarget[trueColor].sMax + offsetS) <= 255 ? (HSVColorForGetTrueTarget[trueColor].sMax + offsetS) : 255;
		if ((avgH >= minThrdH && avgH <= maxThrdH)
			&& (avgS >= minThrdS && avgS <= maxThrdS)) {
			return true;
		}
	}
	return false;
}

double ImageProcess::getDiffFromMatHCh(cv::Mat &h_mat, cv::Mat &curMask) {
	cv::Mat curMaskCvt;
	cv::cvtColor(curMask, curMaskCvt, CV_RGB2GRAY);

	std::vector<int> hist(180, 0);
	int nRows = h_mat.rows;
	int nCols = h_mat.cols;
	uchar *pDataMat;
	uchar *pDataMatMask;
	for (int j = 0; j < nRows; j++)
	{
		pDataMat = h_mat.ptr<uchar>(j);
		pDataMatMask = curMaskCvt.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			if (pDataMatMask[i] > 0) {	// 统计大于0的数
				hist[pDataMat[i]] += 1;
			}
		}
	}
	double sum = std::accumulate(hist.begin(), hist.end(), 0.0);
	double mean = sum / hist.size(); //均值

	double accum = 0.0;
	std::for_each(hist.begin(), hist.end(), [&](const double d) {
		accum += (d - mean)*(d - mean);
	});

	double stdev = sqrt(accum / (hist.size() - 1)); //方差
	//std::cout << " mean:" << mean << " stdev:" << stdev << std::endl;

	//for (int i = 0; i < hist.size(); ++i) {
	//	std::cout << hist[i] << " ";
	//}

	

	return stdev;
}

int ImageProcess::getTargetChannelAvgWithMask(cv::Mat &src, int chl, cv::Mat &cmpMask) {
	if (chl != 0) {
		return 0;
		std::cout << "Only channel 0." << std::endl;
	}
	cv::Mat curMaskCvt;
	cv::cvtColor(cmpMask, curMaskCvt, CV_RGB2GRAY);
	int countPixel = 0;
	int nRows = src.rows;
	int nCols = src.cols;
	uchar *pDataMat;
	uchar *pDataMatMask;
	long long sumPixel = 0;
	int ans = 0;
	for (int j = 0; j < nRows; j++)
	{
		pDataMat = src.ptr<uchar>(j);
		pDataMatMask = curMaskCvt.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			if (pDataMatMask[i] > 0) {	// 统计大于0的数
				countPixel += 1;
				sumPixel += pDataMat[i];
			}
		}
	}
	//std::cout << " sumPixel:" << sumPixel << " countPixel:" << countPixel;
	ans = static_cast<int>(static_cast<double>(sumPixel) / countPixel);

	return ans;
}

int ImageProcess::getTargetMaxNumGrayValWithMask(cv::Mat &src, int chl) {
	if (chl != 0) {
		return 0;
		std::cout << "Only channel 0." << std::endl;
	}
	int nRows = src.rows;
	int nCols = src.cols;
	uchar *pDataMat;
	std::vector<int> cnts(256, 0);
	for (int j = 0; j < nRows; j++)
	{
		pDataMat = src.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			if (pDataMat[i] > 0) {	// 统计大于0的数
				cnts[pDataMat[i]] += 1;
			}
		}
	}
	//for (int i = 0; i < 256; ++i)
	//	std::cout << cnts[i] << " ";
	//std::cout << std::endl;
	//std::cout << "idx: " << (max_element(cnts.begin(), cnts.end()) - cnts.begin()) << std::endl;
	//std::cout << "val: " << *max_element(cnts.begin(), cnts.end()) << std::endl;

	return max_element(cnts.begin(), cnts.end()) - cnts.begin();
}

void ImageProcess::cutDstRoiGray(cv::Mat &diffMask, cv::Rect &rect) {
	cv::Mat thrd;
	thrd = diffMask.clone();
	//cv::threshold(diffMask, thrd, 0, 255, CV_THRESH_BINARY);
	//cv::imshow("thrd", thrd);
	//thrd = ~thrd;
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;//存储查找到的第i个轮廓的后[i][0]、前[i][1]、父[i][2]、子轮廓[i][3]
	findContours(thrd, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
	//Mat imageContours0 = Mat::zeros(thrd.size(), CV_8UC1);	//最小外接正矩形画布
	cv::Rect boundRectMax;
	//std::cout << contours.size() << std::endl;
	if (contours.size() > 0) {
		if (contours.size() != 1) {

			int maxArea = INT_MIN;
			for (int i = 0; i < contours.size(); i++)
			{
				cv::Rect boundRect = cv::boundingRect(cv::Mat(contours[i]));
				int thisArea = boundRect.width * boundRect.height;
				if (thisArea > maxArea) {
					maxArea = thisArea;
					boundRectMax = boundRect;
				}
			}
			//diffMask = diffMask(cv::Range(), cv::Range());
		}
		else {
			boundRectMax = cv::boundingRect(cv::Mat(contours[0]));

		}
		rect = boundRectMax;
		//std::cout << rect << std::endl;
		diffMask = diffMask(cv::Range(boundRectMax.y, boundRectMax.y + boundRectMax.height), cv::Range(boundRectMax.x, boundRectMax.x + boundRectMax.width));
		//cv::imshow("diffMask", diffMask);
	}
	else {
		std::cout << "error" << std::endl;
	}

	//imshow("最小正外接矩形", imageContours0);
}

cv::Mat ImageProcess::myDft(cv::Mat &img_src) {
	//2.输入图片扩充尺寸
	int m = cv::getOptimalDFTSize(img_src.rows);
	int n = cv::getOptimalDFTSize(img_src.cols);

	cv::Mat img_padded;
	cv::copyMakeBorder(img_src, img_padded, 0, m - img_src.rows, 0, n - img_src.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
	//std::cout << "扩充图片 高度:  " << img_padded.rows << "   宽度:" << img_padded.cols << std::endl;

	//3.为傅里叶变换结果分配存储空间
	cv::Mat planes[] = { cv::Mat_<float>(img_padded), cv::Mat::zeros(img_padded.size(), CV_32F) };
	cv::Mat complexI;
	cv::merge(planes, 2, complexI);

	//4.执行傅里叶变换
	cv::dft(complexI, complexI);

	//5.将复数转换为幅值
	cv::split(complexI, planes);
	cv::magnitude(planes[0], planes[1], planes[0]);
	cv::Mat magnitudeImage = planes[0];

	//6.进行对数尺度缩放
	//magnitudeImage += cv::Scalar::all(1);
	//cv::log(magnitudeImage, magnitudeImage);  //自然对数

											  //7.重新分布幅度图像象限
	magnitudeImage = magnitudeImage(cv::Rect(0, 0, magnitudeImage.cols & -2, magnitudeImage.rows & -2));
	int cx = magnitudeImage.cols / 2;
	int cy = magnitudeImage.rows / 2;

	//象限分割
	cv::Mat q0(magnitudeImage, cv::Rect(0, 0, cx, cy));
	cv::Mat q1(magnitudeImage, cv::Rect(cx, 0, cx, cy));
	cv::Mat q2(magnitudeImage, cv::Rect(0, cy, cx, cy));
	cv::Mat q3(magnitudeImage, cv::Rect(cx, cy, cx, cy));

	//象限调整
	cv::Mat temp;
	q0.copyTo(temp);
	q3.copyTo(q0);
	temp.copyTo(q3);

	q1.copyTo(temp);
	q2.copyTo(q1);
	temp.copyTo(q2);

	//8.归一化显示
	cv::normalize(magnitudeImage, magnitudeImage, 0, 1, cv::NORM_MINMAX);

	//9.显示频谱图片
	//cv::imshow("频谱幅值", magnitudeImage);

	return magnitudeImage;
}

double ImageProcess::getBlackTapeDFT(cv::Mat &imgOrg, cv::Mat &imgComp, int idx, int xOffset, int yOffset) {
	cv::Mat curMask = maskForTargetBlock[idx].clone();

	cv::Mat temp = imgOrg & curMask;
	cv::Mat toMatch = imgTranslate(imgComp, xOffset, yOffset, false);
	toMatch = toMatch & curMask;
	cv::imwrite(".\\test\\maskMat\\toMatImg" + std::to_string(idx) + ".jpg", toMatch);

	if (curMask.channels() != 1)
		cv::cvtColor(curMask, curMask, CV_BGR2GRAY);
	cv::Mat maskRotate;
	double degree = CalcDegreeBlackSence(curMask, maskRotate);
	//cv::imshow("maskRotate", maskRotate);
	std::cout << " degree:" << degree;

	cv::Mat afterRotateMask;
	cv::Mat afterScaleMask(curMask.size() * 2, curMask.type(), cv::Scalar(0));
	curMask.copyTo(afterScaleMask(cv::Rect(curMask.cols / 2, curMask.rows / 2,
		curMask.cols, curMask.rows)));
	rotateImageBlackSence(afterScaleMask, afterRotateMask, degree);

	cv::Mat afterRotateTemp;
	cv::Mat afterScaleTemp(temp.size() * 2, temp.type(), cv::Scalar(0, 0, 0));
	temp.copyTo(afterScaleTemp(cv::Rect(temp.cols / 2, temp.rows / 2,
		temp.cols, temp.rows)));
	rotateImageBlackSence(afterScaleTemp, afterRotateTemp, degree);
	//cv::imshow("afterRotateTemp", afterRotateTemp);

	cv::Mat afterRotateToMatch;
	cv::Mat afterScaleToMatch(toMatch.size() * 2, toMatch.type(), cv::Scalar(0, 0, 0));
	toMatch.copyTo(afterScaleToMatch(cv::Rect(toMatch.cols / 2, toMatch.rows / 2,
		toMatch.cols, toMatch.rows)));
	rotateImageBlackSence(afterScaleToMatch, afterRotateToMatch, degree);
	//cv::imshow("afterRotateToMatch", afterRotateToMatch);

	cv::Mat tempGrayAfterRT, toMatchGrayAfterRT;
	cv::cvtColor(afterRotateTemp, tempGrayAfterRT, CV_BGR2GRAY);
	cv::cvtColor(afterRotateToMatch, toMatchGrayAfterRT, CV_BGR2GRAY);

	//std::cout << "before tempGrayAfterRT" << tempGrayAfterRT.size() << std::endl;
	//std::cout << "before toMatchGrayAfterRT" << toMatchGrayAfterRT.size() << std::endl;

	cv::Rect minRoi;
	cutDstRoiGray(afterRotateMask, minRoi);
	tempGrayAfterRT = tempGrayAfterRT(cv::Range(minRoi.y, minRoi.y + minRoi.height), cv::Range(minRoi.x, minRoi.x + minRoi.width));
	toMatchGrayAfterRT = toMatchGrayAfterRT(cv::Range(minRoi.y, minRoi.y + minRoi.height), cv::Range(minRoi.x, minRoi.x + minRoi.width));

	//cv::imshow("tempGrayAfterRT", tempGrayAfterRT);
	//cv::imshow("toMatchGrayAfterRT", toMatchGrayAfterRT);

	//std::cout << "after tempGrayAfterRT" << tempGrayAfterRT.size() << std::endl;
	//std::cout << "after toMatchGrayAfterRT" << toMatchGrayAfterRT.size() << std::endl;

	cv::Mat fMat = myDft(tempGrayAfterRT);
	//cv::imshow("频谱幅值fMat", fMat);
	cv::Mat fMatToMatch = myDft(toMatchGrayAfterRT);
	//cv::imshow("频谱幅值fMatToMatch", fMatToMatch);

	//std::cout << "fMat" << fMat.size() << " " << fMat.channels() << " " << fMat.type() << std::endl;
	//std::cout << "fMatToMatch" << fMatToMatch.size() << " " << fMatToMatch.channels() << " " << fMat.type() << std::endl;

	int targetCol = fMat.cols / 2;	// 统计这一列
	int targetRowCen = fMat.rows / 2;	// 中间行

	//for (int j = 0; j < fMat.rows; j++)
	//	std::cout << (fMat.at<float>(j, targetCol)) << " ";
	//std::cout << std::endl;

	//for (int j = 0; j < fMatToMatch.rows; j++)
	//	std::cout << (fMatToMatch.at<float>(j, targetCol)) << " ";
	//std::cout << std::endl;

	// 选取(cen - startLeft, cen - endRight)个像素点做平均
	int startLeft = 10;
	int endRight = 1;
	float fMatCenAvg = 0.0;
	for (int i = targetRowCen - startLeft; i <= targetRowCen - endRight; ++i) {
		fMatCenAvg += fMat.at<float>(i, targetCol);
	}
	fMatCenAvg /= (startLeft - endRight + 1);
	std::cout << " fMatCenAvg:" << fMatCenAvg;

	float fMatToMatchCenAvg = 0.0;
	for (int i = targetRowCen - startLeft; i <= targetRowCen - endRight; ++i) {
		fMatToMatchCenAvg += fMatToMatch.at<float>(i, targetCol);
	}
	fMatToMatchCenAvg /= (startLeft - endRight + 1);
	std::cout << " fMatToMatchCenAvg:" << fMatToMatchCenAvg;

	float scaleAvg = 0.06;	// 允许的正负偏差
	if (fabs(fMatToMatchCenAvg - fMatCenAvg) >= scaleAvg)
		return 0.0;
	return 1.0;
}

void ImageProcess::rotateImageBlackSence(cv::Mat src, cv::Mat& img_rotate, double degree)
{
	//旋转中心为图像中心    
	cv::Point2f center;
	center.x = float(src.cols / 2.0);
	center.y = float(src.rows / 2.0);
	int length = 0;
	length = sqrt(src.cols*src.cols + src.rows*src.rows);
	//计算二维旋转的仿射变换矩阵  
	cv::Mat M = getRotationMatrix2D(center, degree, 1);
	warpAffine(src, img_rotate, M, cv::Size(length, length), 1, 0, cv::Scalar(0, 0, 0));//仿射变换，背景色填充为白色  
}

double ImageProcess::getSimByMask(cv::Mat &imgOrg, cv::Mat &imgComp, int idx, int xOffset, int yOffset) {
	cv::Mat curMask = maskForTargetBlock[idx].clone();

	//cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15), cv::Point(-1, -1));
	//cv::morphologyEx(curMask, curMask, CV_MOP_DILATE, kernel);

	cv::Mat tempImg = imgOrg & curMask;
	cv::Mat toMatImg = imgTranslate(imgComp, xOffset, yOffset, false);
	toMatImg = toMatImg & curMask;
	cv::imwrite(".\\test\\maskMat\\toMatImg" + std::to_string(idx) + ".jpg", toMatImg);
	//return Correlation(tempImg, toMatImg);
	cv::Mat resultImg;
	cv::matchTemplate(toMatImg, tempImg, resultImg, cv::TM_CCOEFF_NORMED);
	// cv::matchTemplate(toMatImg, tempImg, resultImg, cv::TM_CCORR_NORMED);
	// cv::matchTemplate(toMatImg, tempImg, resultImg, cv::TM_SQDIFF_NORMED);
	// return 1.0 - resultImg.at<float>(0);	// TM_SQDIFF_NORMED用这个
	return resultImg.at<float>(0);
}

void ImageProcess::continueTargetBlockScale(cv::Point &startPoint, cv::Point &endPoint, int idx, int xoffset, int yoffset, double scale) {
	if (scale > 1.0) {	// 这里放大实际上有些问题，如果移动后的图像扩大到黑色区域？
		int xDiff = static_cast<int>(static_cast<double>(endPoint.x - startPoint.x) * (scale - 1.0) / 2.0);
		int yDiff = static_cast<int>(static_cast<double>(endPoint.y - startPoint.y) * (scale - 1.0) / 2.0);
		startPoint.x = (startPoint.x - xDiff) > 0 ? (startPoint.x - xDiff) : 0;
		startPoint.y = (startPoint.y - yDiff) > 0 ? (startPoint.y - yDiff) : 0;
		endPoint.x = (endPoint.x + xDiff) < (block[idx][2] - block[idx][0]) ? (endPoint.x + xDiff) : (block[idx][2] - block[idx][0]);
		endPoint.y = (endPoint.y + yDiff) < (block[idx][3] - block[idx][1]) ? (endPoint.y + yDiff) : (block[idx][3] - block[idx][1]);
	}
	else
		return;
}

double ImageProcess::multiTemplateMatch(cv::Mat &curNorm, cv::Mat &curPartRoi, int idx) {
	// 先拿标准图像与多模板图像计算出多模板图像中的目标位置
	cv::Mat temp = curNorm(cv::Range(trueTargetPartAxis[idx][1], trueTargetPartAxis[idx][3]),
		cv::Range(trueTargetPartAxis[idx][0], trueTargetPartAxis[idx][2]));
	double maxAns = 0.0;

	cv::Mat realTimeMat = curPartRoi.clone();
	cv::Rect rectRealTime = getRealTimePosition(realTimeMat, temp);	// 找到了实时图像中的位置
	cv::Mat partRealTimeRoi = realTimeMat(rectRealTime);

	for (auto iter = multiTempMat[idx].begin(); iter != multiTempMat[idx].end(); ++iter) {
		cv::Mat thisTemp = (*iter).clone();
		
		cv::Rect rectTemp = getRealTimePosition(thisTemp, temp);	// 找到了多模板中目标的位置
		
		//cv::rectangle(thisTemp, rectTemp, cv::Scalar(0, 255, 0), 2, 8, 0);
		//cv::rectangle(realTimeMat, rectRealTime, cv::Scalar(0, 255, 0), 2, 8, 0);
		//cv::imwrite("thisTemp.jpg", thisTemp);
		//cv::imwrite("realTimeMat.jpg", realTimeMat);
		cv::Mat partMultiTemp = thisTemp(rectTemp);
		
		cv::Mat resultImg;
		cv::matchTemplate(partRealTimeRoi, partMultiTemp, resultImg, cv::TM_CCORR_NORMED);
		double thisAns = resultImg.at<float>(0);
		if (thisAns > maxAns) {
			maxAns = thisAns;
			//std::string realTimePath = ".\\test\\multiTemp\\partRealTimeRoi" + std::to_string(idx) + ".jpg";
			//std::string multiTempPath = ".\\test\\multiTemp\\partMultiTemp" + std::to_string(idx) + ".jpg";
			//cv::imwrite(realTimePath, partRealTimeRoi);
			//cv::imwrite(multiTempPath, partMultiTemp);
		}
	}
	return maxAns;
}

cv::Mat ImageProcess::nearestNeighRotate(cv::Mat img, float angle)
{
	int len = (int)(sqrtf(pow(img.rows, 2) + pow(img.cols, 2)) + 0.5);

	cv::Mat retMat = cv::Mat::zeros(len, len, CV_8UC1);
	float anglePI = angle * CV_PI / 180;
	int xSm, ySm;

	for (int i = 0; i < retMat.rows; i++)
		for (int j = 0; j < retMat.cols; j++)
		{
			xSm = (int)((i - retMat.rows / 2)*cos(anglePI) - (j - retMat.cols / 2)*sin(anglePI) + 0.5);
			ySm = (int)((i - retMat.rows / 2)*sin(anglePI) + (j - retMat.cols / 2)*cos(anglePI) + 0.5);
			xSm += img.rows / 2;
			ySm += img.cols / 2;

			if (xSm >= img.rows || ySm >= img.cols || xSm <= 0 || ySm <= 0) {
				retMat.at<uchar>(i, j) = 0;
			}
			else {
				retMat.at<uchar>(i, j) = img.at<uchar>(xSm, ySm);
			}
		}

	return retMat;
}

double ImageProcess::getPlugDirectionTopHat(cv::Mat &imgOrg, cv::Mat &imgComp, int idx, int xOffset, int yOffset) {
	cv::Point startPoint = cv::Point(trueTargetPartAxis[idx][0], trueTargetPartAxis[idx][1]);
	cv::Point endPoint = cv::Point(trueTargetPartAxis[idx][2], trueTargetPartAxis[idx][3]);

	// 这里对于坐标的变换实际上是对于配准已经扩大后的区域进行缩放->1.1(配准倍数) * 1.1(这里的倍数)
	continueTargetBlockScale(startPoint, endPoint, idx, xOffset, yOffset, 1.2);

	cv::Mat tempImg = imgOrg(cv::Range(startPoint.y, endPoint.y), cv::Range(startPoint.x, endPoint.x));
	cv::Mat toMatImg = imgTranslate(imgComp, xOffset, yOffset, false);
	toMatImg = toMatImg(cv::Range(startPoint.y, endPoint.y), cv::Range(startPoint.x, endPoint.x));

	cv::imwrite(".\\test\\matchOrgMat\\tempImg" + std::to_string(idx) + ".jpg", tempImg);
	cv::imwrite(".\\test\\matchOrgMat\\toMatImg" + std::to_string(idx) + ".jpg", toMatImg);

	cv::Mat grayTemp, grayToMatch;
	cv::cvtColor(tempImg, grayTemp, cv::COLOR_BGR2GRAY);
	cv::cvtColor(toMatImg, grayToMatch, cv::COLOR_BGR2GRAY);
	cv::blur(grayTemp, grayTemp, cv::Size(3, 3));
	cv::blur(grayToMatch, grayToMatch, cv::Size(3, 3));

	cv::Mat thrdTemp, thrdToMatch;
	cv::threshold(grayTemp, thrdTemp, 128, 255, CV_THRESH_OTSU);
	cv::threshold(grayToMatch, thrdToMatch, 128, 255, CV_THRESH_OTSU);
	//cv::threshold(grayTemp, thrdTemp, 64, 255, CV_THRESH_BINARY);
	//cv::threshold(grayToMatch, thrdToMatch, 64, 255, CV_THRESH_BINARY);
	//cv::imshow("thrdTemp", thrdTemp);
	//cv::imshow("thrdToMatch", thrdToMatch);

	int nRows = grayTemp.rows;
	int nCols = grayTemp.cols;
	// 去除小白斑
	int area = nRows * nCols / 25;
	RemoveSmallRegion(thrdTemp, thrdTemp, area, 1, 1);	// 小于1/100认定为斑点
	RemoveSmallRegion(thrdToMatch, thrdToMatch, area, 1, 1);	// 小于1/100认定为斑点
																//cv::imshow("thrdTemp", thrdTemp);
																//cv::imshow("thrdToMatch", thrdToMatch);

	cv::Mat tmpRt;
	double angleTemp = CalcDegree(thrdTemp, tmpRt);
	//std::cout << "angleTemp:" << angleTemp << std::endl;
	double angleToMatch = CalcDegree(thrdToMatch, tmpRt);
	//std::cout << "angleToMatch:" << angleToMatch << std::endl;

	//float angleTemp = getAngle(thrdTemp);
	//float angleToMatch = getAngle(thrdToMatch);
	//std::cout << "angleTemp:" << angleTemp << std::endl;
	//std::cout << "angleToMatch:" << angleToMatch << std::endl;

	//获得旋转矩阵
	cv::Point2f centerTemp(thrdTemp.cols / 2, thrdTemp.rows / 2);
	cv::Mat rotmTemp = getRotationMatrix2D(centerTemp, angleTemp, 1.0);
	//旋转图像thrdTemp
	cv::Mat dstTemp;
	cv::warpAffine(thrdTemp, dstTemp, rotmTemp, thrdTemp.size(), cv::INTER_LINEAR, 0, cv::Scalar(0));
	//cv::imshow("dstTemp", dstTemp);

	//获得旋转矩阵
	cv::Point2f centerToMatch(thrdToMatch.cols / 2, thrdToMatch.rows / 2);
	cv::Mat rotmToMatch = getRotationMatrix2D(centerToMatch, angleToMatch, 1.0);
	//旋转图像thrdToMatch
	cv::Mat dstToMatch;
	cv::warpAffine(thrdToMatch, dstToMatch, rotmToMatch, thrdToMatch.size(), cv::INTER_LINEAR, 0, cv::Scalar(0));
	//cv::imshow("dstToMatch", dstToMatch);

	// 计算平均值，去除小于的列
	nRows = thrdTemp.rows;
	nCols = thrdTemp.cols;
	std::vector<int> vectTemp(nCols, 0);
	std::vector<int> vectToMatch(nCols, 0);
	for (int j = 0; j < nCols; ++j) {
		for (int i = 0; i < nRows; ++i) {
			if (static_cast<int>(dstTemp.at<uchar>(i, j)) > 0) {
				vectTemp[j] += 1;
			}
		}
	}
	//for (int i = 0; i < vectTemp.size(); ++i)
	//	std::cout << vectTemp[i] << " ";
	//std::cout << std::endl;
	for (int j = 0; j < nCols; ++j) {
		for (int i = 0; i < nRows; ++i) {
			if (static_cast<int>(dstToMatch.at<uchar>(i, j)) > 0) {
				vectToMatch[j] += 1;
			}
		}
	}
	int countColsTemp = 0, countColsToMatch = 0;
	int avgCountTemp = 0, avgCountToMatch = 0;
	for (int i = 0; i < vectTemp.size(); ++i) {
		if (vectTemp[i] > 0) {
			countColsTemp += 1;
			avgCountTemp += vectTemp[i];
		}
	}
	avgCountTemp /= countColsTemp;
	for (int i = 0; i < vectToMatch.size(); ++i) {
		if (vectToMatch[i] > 0) {
			countColsToMatch += 1;
			avgCountToMatch += vectToMatch[i];
		}
	}
	avgCountToMatch /= countColsToMatch;
	// 原图修正
	for (int j = 0; j < nCols; ++j) {
		if (vectTemp[j] < avgCountTemp) {
			for (int i = 0; i < nRows; ++i) {
				dstTemp.at<uchar>(i, j) = 0;
			}
		}
	}
	for (int j = 0; j < nCols; ++j) {
		if (vectToMatch[j] < avgCountToMatch) {
			for (int i = 0; i < nRows; ++i) {
				dstToMatch.at<uchar>(i, j) = 0;
			}
		}
	}
	//cv::imshow("dstTempRemoveLowAvg", dstTemp);
	//cv::imshow("dstToMatchRemoveLowAvg", dstToMatch);

	// 先通过一个简单的闭运算去除白色区域内部的黑色小斑点
	cv::Mat morTemp, morToMatch;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(16, 16), cv::Point(-1, -1));
	cv::morphologyEx(dstTemp, morTemp, CV_MOP_CLOSE, kernel);
	cv::morphologyEx(dstToMatch, morToMatch, CV_MOP_CLOSE, kernel);
	//cv::imshow("morTemp", morTemp);
	//cv::imshow("morToMatch", morToMatch);
	// 通过矩形核提取目标
	kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(21, 21), cv::Point(-1, -1));
	cv::morphologyEx(morTemp, morTemp, CV_MOP_TOPHAT, kernel);
	cv::morphologyEx(morToMatch, morToMatch, CV_MOP_TOPHAT, kernel);
	//cv::imshow("morTempCV_MOP_TOPHAT", morTemp);
	//cv::imshow("morToMatchCV_MOP_TOPHAT", morToMatch);

	// 分左右两块进行比较面积，面积大的即为目标所在一侧
	int tempPos = 0, toMatchPos = 0;
	int tempAreaLeft = 0, tempAreaRight = 0;
	int toMatchAreaLeft = 0, toMatchAreaRight = 0;
	// temp左半部
	for (int j = 0; j < nCols / 2; ++j) {
		for (int i = 0; i < nRows; ++i) {
			if (static_cast<int>(morTemp.at<uchar>(i, j)) > 0) {
				tempAreaLeft += 1;
			}
		}
	}
	// temp右半部
	for (int j = nCols / 2; j < nCols; ++j) {
		for (int i = 0; i < nRows; ++i) {
			if (static_cast<int>(morTemp.at<uchar>(i, j)) > 0) {
				tempAreaRight += 1;
			}
		}
	}
	// toMatch左半部
	for (int j = 0; j < nCols / 2; ++j) {
		for (int i = 0; i < nRows; ++i) {
			if (static_cast<int>(morToMatch.at<uchar>(i, j)) > 0) {
				toMatchAreaLeft += 1;
			}
		}
	}
	// toMatch右半部
	for (int j = nCols / 2; j < nCols; ++j) {
		for (int i = 0; i < nRows; ++i) {
			if (static_cast<int>(morToMatch.at<uchar>(i, j)) > 0) {
				toMatchAreaRight += 1;
			}
		}
	}
	tempPos = tempAreaLeft > tempAreaRight ? 0 : 1;
	toMatchPos = toMatchAreaLeft > toMatchAreaRight ? 0 : 1;

	std::cout << " tempAreaLeft:" << tempAreaLeft << " tempAreaRight:" << tempAreaRight;
	std::cout << " toMatchAreaLeft:" << toMatchAreaLeft << " toMatchAreaRight:" << toMatchAreaRight;

	if (tempPos == toMatchPos)
		return 1.0;

	return 0.0;
}

double ImageProcess::getPlugDirection(cv::Mat &imgOrg, cv::Mat &imgComp, int idx, int xOffset, int yOffset) {
	cv::Point startPoint = cv::Point(trueTargetPartAxis[idx][0], trueTargetPartAxis[idx][1]);
	cv::Point endPoint = cv::Point(trueTargetPartAxis[idx][2], trueTargetPartAxis[idx][3]);

	// 这里对于坐标的变换实际上是对于配准已经扩大后的区域进行缩放->1.1(配准倍数) * 1.1(这里的倍数)
	continueTargetBlockScale(startPoint, endPoint, idx, xOffset, yOffset, 1.2);

	cv::Mat tempImg = imgOrg(cv::Range(startPoint.y, endPoint.y), cv::Range(startPoint.x, endPoint.x));
	cv::Mat toMatImg = imgTranslate(imgComp, xOffset, yOffset, false);
	toMatImg = toMatImg(cv::Range(startPoint.y, endPoint.y), cv::Range(startPoint.x, endPoint.x));

	cv::imwrite(".\\test\\matchOrgMat\\tempImg" + std::to_string(idx) + ".jpg", tempImg);
	cv::imwrite(".\\test\\matchOrgMat\\toMatImg" + std::to_string(idx) + ".jpg", toMatImg);

	cv::Mat grayTemp, grayToMatch;
	cv::cvtColor(tempImg, grayTemp, cv::COLOR_BGR2GRAY);
	cv::cvtColor(toMatImg, grayToMatch, cv::COLOR_BGR2GRAY);
	cv::blur(grayTemp, grayTemp, cv::Size(3, 3));
	cv::blur(grayToMatch, grayToMatch, cv::Size(3, 3));
	cv::Mat cannyTemp, cannyToMatch;
	cv::Canny(grayTemp, cannyTemp, 50, 150);
	cv::Canny(grayToMatch, cannyToMatch, 50, 150);

	int maskIdx = -1;	// mask类型:0上半部、1下半部、2左半部、3右半部、4左上半部、5右下半部、6左下半部、7右上半部
	for (int i = 0; i < plugDirectMaskMsg.size(); ++i) {
		if (plugDirectMaskMsg[i][0] == idx) {
			maskIdx = plugDirectMaskMsg[i][1];
			break;
		}
	}

	cv::Mat mask = cv::Mat::ones(cannyTemp.size(), CV_8UC1);
	mask.setTo(255);

	int nRows = cannyTemp.rows;
	int nCols = cannyTemp.cols;

	//cv::Point p0 = cv::Point(0, nRows - 1);
	//cv::Point p1 = cv::Point(nCols - 1, 0);
	cv::Point p0, p1;

	// 分类讨论两个点
	if (maskIdx == 0) {	// 上半部
		p0 = cv::Point(0, nRows / 2);
		p1 = cv::Point(nCols - 1, nRows / 2);
	}
	else if (maskIdx == 1) {	// 下半部
		p0 = cv::Point(0, nRows / 2);
		p1 = cv::Point(nCols - 1, nRows / 2);
	}
	else if (maskIdx == 2) {	// 左半部
		p0 = cv::Point(nCols / 2, 0);
		p1 = cv::Point(nCols / 2, nRows);
	}
	else if (maskIdx == 3) {	// 右半部
		p0 = cv::Point(nCols / 2, 0);
		p1 = cv::Point(nCols / 2, nRows);
	}
	else if (maskIdx == 4) {	// 左上半部
		p0 = cv::Point(0, nRows - 1);
		p1 = cv::Point(nCols - 1, 0);
	}
	else if (maskIdx == 5) {	// 右下半部
		p0 = cv::Point(0, nRows - 1);
		p1 = cv::Point(nCols - 1, 0);
	}
	else if (maskIdx == 6) {	// 左下半部
		p0 = cv::Point(0, 0);
		p1 = cv::Point(nCols - 1, nRows - 1);
	}
	else if (maskIdx == 7) {	// 右上半部
		p0 = cv::Point(0, 0);
		p1 = cv::Point(nCols - 1, nRows - 1);
	}
	cv::line(mask, p0, p1, 0);

	if (maskIdx == 0) {	// 上半部
		cv::floodFill(mask, cv::Point(nCols - nCols / 4, nRows - nRows / 4), cv::Scalar(0));	// 填充下半部
	}
	else if (maskIdx == 1) {	// 下半部
		cv::floodFill(mask, cv::Point(nCols / 4, nRows / 4), cv::Scalar(0));	// 填充上半部
	}
	else if (maskIdx == 2) {	// 左半部
		cv::floodFill(mask, cv::Point(nCols - nCols / 4, nRows - nRows / 4), cv::Scalar(0));	// 填充右半部
	}
	else if (maskIdx == 3) {	// 右半部
		cv::floodFill(mask, cv::Point(nCols / 4, nRows / 4), cv::Scalar(0));	// 填充左半部
	}
	else if (maskIdx == 4) {	// 左上半部
		cv::floodFill(mask, cv::Point(nCols - nCols / 4, nRows - nRows / 4), cv::Scalar(0));	// 填充右下角
	}
	else if (maskIdx == 5) {	// 右下半部
		cv::floodFill(mask, cv::Point(nCols / 4, nRows / 4), cv::Scalar(0));	// 填充左上角
	}
	else if (maskIdx == 6) {	// 左下半部
		cv::floodFill(mask, cv::Point(nCols - nCols / 4, nRows / 4), cv::Scalar(0));	// 填充右上角
	}
	else if (maskIdx == 7) {	// 右上半部
		cv::floodFill(mask, cv::Point(nCols / 4, nRows - nRows / 4), cv::Scalar(0));	// 填充左下角
	}
	
	cv::Mat maskTemp = cannyTemp & mask;
	cv::Mat maskToMatch = cannyToMatch & mask;

	cv::line(maskTemp, p0, p1, cv::Scalar(255));
	cv::line(maskToMatch, p0, p1, cv::Scalar(255));

	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(-1, -1));
	cv::morphologyEx(maskTemp, maskTemp, CV_MOP_DILATE, kernel);
	cv::morphologyEx(maskToMatch, maskToMatch, CV_MOP_DILATE, kernel);

	if (maskIdx == 0) {	// 上半部
		cv::floodFill(maskTemp, cv::Point(nCols / 5, nRows / 10), cv::Scalar(255));	// 填充左上角
		cv::floodFill(maskTemp, cv::Point(nCols - nCols / 5, nRows / 10), cv::Scalar(255));	// 填充右上角

		cv::floodFill(maskToMatch, cv::Point(nCols / 5, nRows / 10), cv::Scalar(255));	// 填充左上角
		cv::floodFill(maskToMatch, cv::Point(nCols - nCols / 5, nRows / 10), cv::Scalar(255));	// 填充右上角

		cv::floodFill(maskTemp, cv::Point(nCols - nCols / 4, nRows - nRows / 4), cv::Scalar(255));	// 填充右下角
		cv::floodFill(maskToMatch, cv::Point(nCols - nCols / 4, nRows - nRows / 4), cv::Scalar(255));	// 填充右下角
	}
	else if (maskIdx == 1) {	// 下半部
		cv::floodFill(maskTemp, cv::Point(nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充左下角
		cv::floodFill(maskTemp, cv::Point(nCols - nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充右下角

		cv::floodFill(maskToMatch, cv::Point(nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充左下角
		cv::floodFill(maskToMatch, cv::Point(nCols - nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充右下角

		cv::floodFill(maskTemp, cv::Point(nCols / 4, nRows / 4), cv::Scalar(255));	// 填充左上角
		cv::floodFill(maskToMatch, cv::Point(nCols / 4, nRows / 4), cv::Scalar(255));	// 填充左上角
	}
	else if (maskIdx == 2) {	// 左半部
		cv::floodFill(maskTemp, cv::Point(nCols / 5, nRows / 10), cv::Scalar(255));	// 填充左上角
		cv::floodFill(maskTemp, cv::Point(nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充左下角

		cv::floodFill(maskToMatch, cv::Point(nCols / 5, nRows / 10), cv::Scalar(255));	// 填充左上角
		cv::floodFill(maskToMatch, cv::Point(nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充左下角

		cv::floodFill(maskTemp, cv::Point(nCols - nCols / 4, nRows - nRows / 4), cv::Scalar(255));	// 填充右下角
		cv::floodFill(maskToMatch, cv::Point(nCols - nCols / 4, nRows - nRows / 4), cv::Scalar(255));	// 填充右下角
	}
	else if (maskIdx == 3) {	// 右半部
		cv::floodFill(maskTemp, cv::Point(nCols - nCols / 5, nRows / 10), cv::Scalar(255));	// 填充右上角
		cv::floodFill(maskTemp, cv::Point(nCols - nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充右下角

		cv::floodFill(maskToMatch, cv::Point(nCols - nCols / 5, nRows / 10), cv::Scalar(255));	// 填充右上角
		cv::floodFill(maskToMatch, cv::Point(nCols - nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充右下角

		cv::floodFill(maskTemp, cv::Point(nCols / 4, nRows / 4), cv::Scalar(255));	// 填充左上角
		cv::floodFill(maskToMatch, cv::Point(nCols / 4, nRows / 4), cv::Scalar(255));	// 填充左上角
	}
	else if (maskIdx == 4) {	// 左上半部
		cv::floodFill(maskTemp, cv::Point(nCols / 5, nRows / 10), cv::Scalar(255));	// 填充左上角
		cv::floodFill(maskTemp, cv::Point(nCols - nCols / 5, nRows / 10), cv::Scalar(255));	// 填充右上角

		cv::floodFill(maskToMatch, cv::Point(nCols / 5, nRows / 10), cv::Scalar(255));	// 填充左上角
		cv::floodFill(maskToMatch, cv::Point(nCols - nCols / 5, nRows / 10), cv::Scalar(255));	// 填充右上角

		cv::floodFill(maskTemp, cv::Point(nCols - nCols / 4, nRows - nRows / 4), cv::Scalar(255));	// 填充右下角
		cv::floodFill(maskToMatch, cv::Point(nCols - nCols / 4, nRows - nRows / 4), cv::Scalar(255));	// 填充右下角
	}
	else if (maskIdx == 5) {	// 右下半部
		cv::floodFill(maskTemp, cv::Point(nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充左下角
		cv::floodFill(maskTemp, cv::Point(nCols - nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充右下角

		cv::floodFill(maskToMatch, cv::Point(nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充左下角
		cv::floodFill(maskToMatch, cv::Point(nCols - nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充右下角

		cv::floodFill(maskTemp, cv::Point(nCols / 4, nRows / 4), cv::Scalar(255));	// 填充左上角
		cv::floodFill(maskToMatch, cv::Point(nCols / 4, nRows / 4), cv::Scalar(255));	// 填充左上角
	}
	else if (maskIdx == 6) {	// 左下半部
		cv::floodFill(maskTemp, cv::Point(nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充左下角
		cv::floodFill(maskTemp, cv::Point(nCols - nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充右下角

		cv::floodFill(maskToMatch, cv::Point(nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充左下角
		cv::floodFill(maskToMatch, cv::Point(nCols - nCols / 5, nRows - nRows / 10), cv::Scalar(255));	// 填充右下角

		cv::floodFill(maskTemp, cv::Point(nCols - nCols / 4, nRows / 4), cv::Scalar(0));	// 填充右上角
		cv::floodFill(maskToMatch, cv::Point(nCols - nCols / 4, nRows / 4), cv::Scalar(0));	// 填充右上角
	}
	else if (maskIdx == 7) {	// 右上半部
		cv::floodFill(maskTemp, cv::Point(nCols / 5, nRows / 10), cv::Scalar(255));	// 填充左上角
		cv::floodFill(maskTemp, cv::Point(nCols - nCols / 5, nRows / 10), cv::Scalar(255));	// 填充右上角

		cv::floodFill(maskToMatch, cv::Point(nCols / 5, nRows / 10), cv::Scalar(255));	// 填充左上角
		cv::floodFill(maskToMatch, cv::Point(nCols - nCols / 5, nRows / 10), cv::Scalar(255));	// 填充右上角

		cv::floodFill(maskTemp, cv::Point(nCols / 4, nRows - nRows / 4), cv::Scalar(0));	// 填充左下角
		cv::floodFill(maskToMatch, cv::Point(nCols / 4, nRows - nRows / 4), cv::Scalar(0));	// 填充左下角
	}

	maskTemp = ~maskTemp;
	maskToMatch = ~maskToMatch;
	// 去除小白斑
	int area = nRows * nCols / 50;
	RemoveSmallRegion(maskTemp, maskTemp, area, 1, 1);	// 小于1/100认定为斑点
	RemoveSmallRegion(maskToMatch, maskToMatch, area, 1, 1);	// 小于1/100认定为斑点
	// 旋转角度
	float angle = 0.0;

	if (maskIdx == 0) {	// 上半部
		angle = 0.0;
	}
	else if (maskIdx == 1) {	// 下半部
		angle = 0.0;
	}
	else if (maskIdx == 2) {	// 左半部
		angle = 90.0;
	}
	else if (maskIdx == 3) {	// 右半部
		angle = 90.0;
	}
	else if (maskIdx == 4) {	// 左上半部
		angle = 45.0;
	}
	else if (maskIdx == 5) {	// 右下半部
		angle = 45.0;
	}
	else if (maskIdx == 3) {	// 左下半部
		angle = 45.0;
	}
	else if (maskIdx == 4) {	// 右上半部
		angle = 45.0;
	}

	cv::Mat maskTempRt, maskToMatchRt;
	maskTempRt = nearestNeighRotate(maskTemp, angle);
	maskToMatchRt = nearestNeighRotate(maskToMatch, angle);

	cv::morphologyEx(maskTempRt, maskTempRt, CV_MOP_DILATE, kernel);
	cv::morphologyEx(maskToMatchRt, maskToMatchRt, CV_MOP_DILATE, kernel);

	//cv::imshow("maskTempRt" + std::to_string(idx), maskTempRt);
	//cv::imshow("maskToMatchRt" + std::to_string(idx), maskToMatchRt);

	nCols = maskTempRt.cols;
	nRows = maskTempRt.rows;
	std::vector<int> tempVect(nCols, 0);
	std::vector<int> toMatchVect(nCols, 0);
	uchar *pDataMat;
	for (int j = 0; j < nRows; j++)
	{
		pDataMat = maskTempRt.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			if (pDataMat[i] > 0) {
				tempVect[i] += 1;
			}
		}
		pDataMat = maskToMatchRt.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			if (pDataMat[i] > 0) {
				toMatchVect[i] += 1;
			}
		}
	}
	std::vector<int> mutlVect(nCols, 0);
	int pZeroLeft = 0;
	int pZeroRight = nCols - 1;
	for (int i = 0; i < nCols / 2; ++i) {
		if (tempVect[i] > 0) {
			pZeroLeft = i;
			break;
		}
	}
	for (int i = nCols - 1; i > nCols / 2; --i) {
		if (tempVect[i] > 0) {
			pZeroRight = i;
			break;
		}
	}
	// 构造左边系数
	int intevlLeft = (nCols / 2 - pZeroLeft) / 4;
	for (int i = pZeroLeft; i < pZeroLeft + intevlLeft; i++)
		mutlVect[i] = mutlVect[i - 1] + 1;
	for (int i = pZeroLeft + intevlLeft; i < pZeroLeft + 3 * intevlLeft; i++)
		mutlVect[i] = mutlVect[i - 1];
	for (int i = pZeroLeft + 3 * intevlLeft; i < nCols / 2; i++)
		mutlVect[i] = mutlVect[i - 1] - 1;
	// 构造右边系数
	mutlVect[nCols / 2 - 1] = 0;
	int intevlRight = (pZeroRight - nCols / 2) / 4;
	for (int i = nCols / 2; i < nCols / 2 + intevlRight; i++)
		mutlVect[i] = mutlVect[i - 1] + 1;
	for (int i = nCols / 2 + intevlRight; i < nCols / 2 + 3 * intevlRight; i++)
		mutlVect[i] = mutlVect[i - 1];
	for (int i = nCols / 2 + 3 * intevlRight; i < pZeroRight; i++)
		mutlVect[i] = mutlVect[i - 1] - 1;
	// 找最大
	for (int i = 0; i < tempVect.size(); ++i) {
		tempVect[i] *= mutlVect[i];
	}
	int tempMaxIdx = std::max_element(tempVect.begin(), tempVect.end()) - tempVect.begin();
	for (int i = 0; i < toMatchVect.size(); ++i) {
		toMatchVect[i] *= mutlVect[i];
	}
	int toMatchMaxIdx = std::max_element(toMatchVect.begin(), toMatchVect.end()) - toMatchVect.begin();
	// 判断在左边还是右边
	int tempPos = 0, toMatchPos = 0;
	if (tempMaxIdx < nCols / 2)
		tempPos = 0;
	else
		tempPos = 1;
	if (toMatchMaxIdx < nCols / 2)
		toMatchPos = 0;
	else
		toMatchPos = 1;

	if (tempPos == toMatchPos)
		return 1.0;
	else
		return 0.0;
}

double ImageProcess::getSimByOrgWithoutMask(cv::Mat &imgOrg, cv::Mat &imgComp, int idx, int xOffset, int yOffset) {
	cv::Point startPoint = cv::Point(trueTargetPartAxis[idx][0], trueTargetPartAxis[idx][1]);
	cv::Point endPoint = cv::Point(trueTargetPartAxis[idx][2], trueTargetPartAxis[idx][3]);

	// 这里对于坐标的变换实际上是对于配准已经扩大后的区域进行缩放->1.1(配准倍数) * 1.1(这里的倍数)
	continueTargetBlockScale(startPoint, endPoint, idx, xOffset, yOffset, 1.2);

	cv::Mat tempImg = imgOrg(cv::Range(startPoint.y, endPoint.y), cv::Range(startPoint.x, endPoint.x));
	cv::Mat toMatImg = imgTranslate(imgComp, xOffset, yOffset, false);
	toMatImg = toMatImg(cv::Range(startPoint.y, endPoint.y), cv::Range(startPoint.x, endPoint.x));

	cv::imwrite(".\\test\\matchOrgMat\\tempImg" + std::to_string(idx) + ".jpg", tempImg);
	cv::imwrite(".\\test\\matchOrgMat\\toMatImg" + std::to_string(idx) + ".jpg", toMatImg);

	cv::Mat resultImg;
	//if (param[idx][1] == 3) {	// 针对插头方向检测
	//	cv::matchTemplate(toMatImg, tempImg, resultImg, cv::TM_CCOEFF_NORMED);
	//	return resultImg.at<float>(0);
	//}
	cv::matchTemplate(toMatImg, tempImg, resultImg, cv::TM_CCORR_NORMED);
	return resultImg.at<float>(0);
}

double ImageProcess::getSimByTargetColorAreaWithoutMask(cv::Mat &imgOrg, cv::Mat &imgComp, int idx) {
	cv::Mat imgOrgCopy, imgCompCopy;
	cv::cvtColor(imgOrg, imgOrgCopy, cv::COLOR_BGR2RGB);
	cv::cvtColor(imgComp, imgCompCopy, cv::COLOR_BGR2RGB);
	std::string trueColor = "";
	int cateIdx = param[idx][1];
	int targetColor = param[idx][3];
	switch (cateIdx)
	{
	case 0: {// 花结束带
		if (targetColor < 2) {	// 灰/白
			if (targetColor == 0)
				trueColor = "Gray";
			else
				trueColor = "White";
		}
		else {	// 粉
			trueColor = "Pink";
		}
		break;
	}
	case 1: {// T18R
		switch (targetColor)
		{
		case 0: {	// 白
			trueColor = "White";
			break;
		}
		case 1: {	// 黑
			trueColor = "Black";
			break;
		}
		case 2: {	// 黄
			trueColor = "Yellow";
			break;
		}
		default:
			break;
		}
		break;
	}
	case 2: {// 胶布
			 //// 红(0)、白(1)、黑(2)、黄(3)、绿(4)、灰(5)
		if (targetColor == 0) {	// 红
			trueColor = "Red";
		}
		else if (targetColor == 1 || targetColor == 5) {// 灰/白
			if (targetColor == 5)
				trueColor = "Gray";
			else
				trueColor = "White";
		}
		else if (targetColor == 2) {	// 黑
			trueColor = "Black";
		}
		else if (targetColor == 3) {	// 黄
			trueColor = "Yellow";
		}
		else if (targetColor == 4) {	// 绿
			trueColor = "Green";
		}
		break;
	}
	case 3: {// 插头
		if (targetColor == 0) {	// 黑
			trueColor = "Black";
		}
		else if (targetColor == 1 || targetColor == 4) {// 灰/白
			if (targetColor == 1)
				trueColor = "White";
			else
				trueColor = "Gray";
		}
		else if (targetColor == 2) {	// 粉
			trueColor = "Pink";
		}
		else if (targetColor == 3) {	// 蓝
			trueColor = "Blue";
		}
		else if (targetColor == 5) {	// 绿
			trueColor = "Green";
		}
		else if (targetColor == 6) {	// 黄
			trueColor = "Yellow";
		}
		break;
	}
	default:
		break;
	}
	// 拉伸替换
	cv::Mat orgGray, cmpGray;
	cv::cvtColor(imgOrgCopy, orgGray, cv::COLOR_RGB2GRAY);
	adjustMatChannel(orgGray, 0);
	cv::cvtColor(imgCompCopy, cmpGray, cv::COLOR_RGB2GRAY);
	adjustMatChannel(cmpGray, 0);
	cv::Mat orgHsv, cmpHsv;
	cv::cvtColor(imgOrgCopy, orgHsv, cv::COLOR_RGB2HSV);
	cv::cvtColor(imgCompCopy, cmpHsv, cv::COLOR_RGB2HSV);
	int nRows = imgOrgCopy.rows;
	int nCols = imgOrgCopy.cols;
	uchar *pDataMat;
	for (int j = 0; j < nRows; j++)
	{
		pDataMat = orgGray.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			orgHsv.at<cv::Vec3b>(j, i)[2] = pDataMat[i];
		}
		pDataMat = cmpGray.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			cmpHsv.at<cv::Vec3b>(j, i)[2] = pDataMat[i];
		}
	}
	cv::Mat orgMask, cmpMask;
	inRange(orgHsv,
		cv::Scalar(HSVColorForGetTrueTarget[trueColor].hMin, HSVColorForGetTrueTarget[trueColor].sMin, HSVColorForGetTrueTarget[trueColor].vMin),
		cv::Scalar(HSVColorForGetTrueTarget[trueColor].hMax, HSVColorForGetTrueTarget[trueColor].sMax, HSVColorForGetTrueTarget[trueColor].vMax), orgMask);
	inRange(cmpHsv,
		cv::Scalar(HSVColorForGetTrueTarget[trueColor].hMin, HSVColorForGetTrueTarget[trueColor].sMin, HSVColorForGetTrueTarget[trueColor].vMin),
		cv::Scalar(HSVColorForGetTrueTarget[trueColor].hMax, HSVColorForGetTrueTarget[trueColor].sMax, HSVColorForGetTrueTarget[trueColor].vMax), cmpMask);
	// 去除小白斑之前简单膨胀一下
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(-1, -1));
	cv::morphologyEx(orgMask, orgMask, CV_MOP_DILATE, kernel);
	cv::morphologyEx(cmpMask, cmpMask, CV_MOP_DILATE, kernel);
	// 去除小白斑
	int area = orgMask.rows * orgMask.cols / 50;
	RemoveSmallRegion(orgMask, orgMask, area, 1, 1);	// 小于1/100认定为斑点
	RemoveSmallRegion(cmpMask, cmpMask, area, 1, 1);	// 小于1/100认定为斑点
	
	kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(19, 19), cv::Point(-1, -1));
	cv::Mat orgBeforeFloodEdge = orgMask.clone();
	floodFillEdge(orgMask);
	cv::morphologyEx(orgMask, orgMask, CV_MOP_DILATE, kernel);
	cv::Mat cmpBeforeFloodEdge = cmpMask.clone();
	floodFillEdge(cmpMask);
	cv::morphologyEx(cmpMask, cmpMask, CV_MOP_DILATE, kernel);
	//double ssimRes = ssimDetect(orgMask, cmpMask);
	//std::cout << " ssimRes:" << ssimRes << std::endl;

	uchar *pDataMatOrg;
	uchar *pDataMatCmp;
	int countCmp = 0, countOrg = 0;
	for (int j = 0; j < nRows; j++)
	{
		pDataMatOrg = orgMask.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			if (pDataMatOrg[i] > 0) {
				countOrg += 1;
			}
		}
		pDataMatCmp = cmpMask.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			if (pDataMatCmp[i] > 0) {
				countCmp += 1;
			}
		}
		
	}
	if (countCmp < 50 || countOrg < 50) {
		countCmp = 0;
		countOrg = 0;
		getTargetMatWithDefineMask(orgBeforeFloodEdge, idx);
		getTargetMatWithDefineMask(cmpBeforeFloodEdge, idx);
		nRows = orgBeforeFloodEdge.rows;
		nCols = orgBeforeFloodEdge.cols;
		for (int j = 0; j < nRows; j++)
		{
			pDataMatOrg = orgBeforeFloodEdge.ptr<uchar>(j);
			for (int i = 0; i < nCols; i++) {
				if (pDataMatOrg[i] > 0) {
					countOrg += 1;
				}
			}
			pDataMatCmp = cmpBeforeFloodEdge.ptr<uchar>(j);
			for (int i = 0; i < nCols; i++) {
				if (pDataMatCmp[i] > 0) {
					countCmp += 1;
				}
			}

		}
		

		cv::imwrite(".\\test\\haveOrNot\\orgBefFloodEdge" + std::to_string(idx) + ".jpg", orgBeforeFloodEdge);
		cv::imwrite(".\\test\\haveOrNot\\cmpBefFloodEdge" + std::to_string(idx) + ".jpg", cmpBeforeFloodEdge);
		int maxC = MAX(countCmp, countOrg);
		int minC = MIN(countCmp, countOrg);
		// std::cout << minC << " " << maxC << std::endl;
		return static_cast<double>(minC) / maxC;
	}

	cv::imwrite(".\\test\\haveOrNot\\org" + std::to_string(idx) + ".jpg", orgMask);
	cv::imwrite(".\\test\\haveOrNot\\cmp" + std::to_string(idx) + ".jpg", cmpMask);
	int maxC = MAX(countCmp, countOrg);
	int minC = MIN(countCmp, countOrg);
	// std::cout << minC << " " << maxC << std::endl;
	return static_cast<double>(minC) / maxC;
}

void ImageProcess::getTargetMatWithDefineMask(cv::Mat &src, int idx) {	// 目前仅提供单通道
	if (src.channels() != 1) {
		std::cout << "Only one channel." << std::endl;
		return;
	}
	// 目前情况使用mask1，mask1为
	cv::Point startPoint = cv::Point(trueTargetPartAxis[idx][0], trueTargetPartAxis[idx][1]);
	cv::Point endPoint = cv::Point(trueTargetPartAxis[idx][2], trueTargetPartAxis[idx][3]);

	// 这里对于坐标的变换实际上是对于配准已经扩大后的区域进行缩放->1.1(配准倍数) * 1.1(这里的倍数)
	continueTargetBlockScale(startPoint, endPoint, idx, -1, -1, 1.8);
	src = src(cv::Range(startPoint.y, endPoint.y), cv::Range(startPoint.x, endPoint.x));
}

double ImageProcess::getSimByTargetColorArea(cv::Mat &imgOrg, cv::Mat &imgComp, int idx, int xOffset, int yOffset) {
	cv::Mat curMask = maskForTargetBlock[idx].clone();
	//cv::Mat kernelMask = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(31, 31), cv::Point(-1, -1));
	//cv::morphologyEx(curMask, curMask, CV_MOP_DILATE, kernelMask);
	//cv::Mat curMask = cv::Mat::ones(maskForTargetBlock[idx].size(), CV_8UC3);
	//cv::Mat toMatImg = imgComp & curMask;	// 即时图像计算区域
	//cv::imshow("toMatImg", toMatImg);
	cv::Mat imgCompCopy;
	cv::cvtColor(imgComp, imgCompCopy, cv::COLOR_BGR2RGB);
	//cv::cvtColor(imgCompCopy, imgCompCopy, cv::COLOR_RGB2BGR);
	std::string trueColor = "";
	int cateIdx = param[idx][1];
	int targetColor = param[idx][3];
	switch (cateIdx)
	{
	case 0: {// 花结束带
		if (targetColor < 2) {	// 灰/白
			if (targetColor == 0)
				trueColor = "Gray";
			else
				trueColor = "White";
		}
		else {	// 粉
			trueColor = "Pink";
		}
		break;
	}
	case 1: {// T18R
		switch (targetColor)
		{
		case 0: {	// 白
			trueColor = "White";
			break;
		}
		case 1: {	// 黑
			trueColor = "Black";
			break;
		}
		case 2: {	// 黄
			trueColor = "Yellow";
			break;
		}
		default:
			break;
		}
		break;
	}
	case 2: {// 胶布
			 //// 红(0)、白(1)、黑(2)、黄(3)、绿(4)、灰(5)
		if (targetColor == 0) {	// 红
			trueColor = "Red";
		}
		else if (targetColor == 1 || targetColor == 5) {// 灰/白
			if (targetColor == 5)
				trueColor = "Gray";
			else
				trueColor = "White";
		}
		else if (targetColor == 2) {	// 黑
			trueColor = "Black";
		}
		else if (targetColor == 3) {	// 黄
			trueColor = "Yellow";
		}
		else if (targetColor == 4) {	// 绿
			trueColor = "Green";
		}
		break;
	}
	case 3: {// 插头
		if (targetColor == 0) {	// 黑
			trueColor = "Black";
		}
		else if (targetColor == 1 || targetColor == 4) {// 灰/白
			if (targetColor == 1)
				trueColor = "White";
			else
				trueColor = "Gray";
		}
		else if (targetColor == 2) {	// 粉
			trueColor = "Pink";
		}
		else if (targetColor == 3) {	// 蓝
			trueColor = "Blue";
		}
		else if (targetColor == 5) {	// 绿
			trueColor = "Green";
		}
		else if (targetColor == 6) {	// 黄
			trueColor = "Yellow";
		}
		break;
	}
	default:
		break;
	}
	// 拉伸替换
	cv::Mat gray;
	cv::cvtColor(imgCompCopy, gray, cv::COLOR_RGB2GRAY);
	adjustMatChannel(gray, 0);
	cv::Mat hsv;
	cv::cvtColor(imgCompCopy, hsv, cv::COLOR_RGB2HSV);
	int nRows = hsv.rows;
	int nCols = hsv.cols;
	uchar *pDataMat;
	for (int j = 0; j < nRows; j++)
	{
		pDataMat = gray.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			hsv.at<cv::Vec3b>(j, i)[2] = pDataMat[i];
		}
	}
	//if (idx == 5)
	//	cv::imshow("imgComp", imgCompCopy);
	// 取出目标
	hsv = imgTranslate(hsv, xOffset, yOffset, false);
	cv::Mat toMatImg;
	if (trueColor == "Black") {
		cv::Mat curMaskNot = ~curMask;
		toMatImg = hsv | curMaskNot;
	}
	else
		toMatImg = hsv & curMask;
	// 计算即时图的面积
	cv::Mat cmpMask;
	inRange(toMatImg,
		cv::Scalar(HSVColorForGetTrueTarget[trueColor].hMin, HSVColorForGetTrueTarget[trueColor].sMin, HSVColorForGetTrueTarget[trueColor].vMin),
		cv::Scalar(HSVColorForGetTrueTarget[trueColor].hMax, HSVColorForGetTrueTarget[trueColor].sMax, HSVColorForGetTrueTarget[trueColor].vMax), cmpMask);
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7), cv::Point(-1, -1));
	cv::morphologyEx(cmpMask, cmpMask, CV_MOP_DILATE, kernel);

	cv::Mat curMaskGray;
	cv::cvtColor(curMask, curMaskGray, cv::COLOR_RGB2GRAY);
	uchar *pDataMatOrg;
	uchar *pDataMatCmp;
	int countCmp = 0, countOrg = 0;
	for (int j = 0; j < nRows; j++)
	{
		pDataMatCmp = cmpMask.ptr<uchar>(j);
		pDataMatOrg = curMaskGray.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			if (pDataMatCmp[i] > 0) {
				countCmp += 1;
			}
			if (pDataMatOrg[i] > 0) {
				countOrg += 1;
			}
		}
	}
	//for (int j = 0; j < nRows; ++j) {
	//	for (int i = 0; i < nCols; ++i) {
	//		if (curMask.at<uchar>(j, i) > 0) {
	//			//curMask.at<uchar>(j, i) = 128;
	//			countOrg += 1;
	//		}
	//	}
	//}
	cv::imwrite(".\\test\\haveOrNot\\org" + std::to_string(idx) + ".jpg", curMaskGray);
	cv::imwrite(".\\test\\haveOrNot\\cmp" + std::to_string(idx) + ".jpg", cmpMask);
	int maxC = MAX(countCmp, countOrg);
	int minC = MIN(countCmp, countOrg);
	// std::cout << minC << " " << maxC << std::endl;
	return static_cast<double>(minC) / maxC;
}

// 两张图像作相关
double ImageProcess::Correlation(cv::Mat &tmplateImg, cv::Mat &toMatchImg) {
	//cv::imshow("tmplateImg", tmplateImg);
	//cv::imshow("toMatchImg", toMatchImg);
	//cv::waitKey(0);
	cv::Scalar mean, stddev;
	double avg1 = 0, dev1 = 0, avg2 = 0, dev2 = 0;
	int tmpRow = 0, tmpCol = 0;
	tmpRow = tmplateImg.rows;
	tmpCol = tmplateImg.cols;

	// 计算均值和标准差
	cv::meanStdDev(tmplateImg, mean, stddev);
	avg1 = mean.val[0];		// 均值
	dev1 = stddev.val[0];	// 标准差

	// 计算相关系数
	long double dotMul = 0;
	for (int i = 0; i < tmpRow; ++i) {
		for (int j = 0; j < tmpCol; ++j) {
			dotMul += abs((long double(tmplateImg.at<uchar>(i, j)) - avg1)*(long double(tmplateImg.at<uchar>(i, j)) - avg1));
		}
	}
	double dotMulAvg = dotMul / (tmpRow * tmpCol);
	double pearsonCorrelationCoefficientTmp = dotMulAvg / (dev1*dev1);

	cv::meanStdDev(toMatchImg, mean, stddev);
	avg2 = mean.val[0];		// 均值
	dev2 = stddev.val[0];	// 标准差
	dotMul = 0;
	for (int i = 0; i < tmpRow; ++i) {
		for (int j = 0; j < tmpCol; ++j) {
			dotMul += abs((long double(tmplateImg.at<uchar>(i, j)) - avg1)*(long double(toMatchImg.at<uchar>(i, j)) - avg2));
		}
	}
	dotMulAvg = dotMul / (tmpRow * tmpCol);
	double pearsonCorrelationCoefficientCur = dotMulAvg / (dev1*dev2);

	//std::cout << "pearsonCorrelationCoefficientCur = " << pearsonCorrelationCoefficientCur << std::endl;
	//std::cout << "pearsonCorrelationCoefficientTmp = " << pearsonCorrelationCoefficientTmp << std::endl;
	double similarity = pearsonCorrelationCoefficientCur / pearsonCorrelationCoefficientTmp;

	return similarity;
}

bool ImageProcess::posIsSatisfied(cv::Mat &startMat, int startCate, int startColor, cv::Point startAxis,
	cv::Mat &endMat, int endCate, int endColor, cv::Point endAxis, int pixelNorm, int pixelOffset) {
	int distancePixel = 0.0;
	if (startCate != 2 && endCate != 2) {	// 两个都不是胶布直接取中心点
		cv::Rect startTrueTarget = findTrueArea(startMat, startCate, startColor, -1);
		//std::cout << "Find startTrueTarget." << std::endl;
		cv::Rect endTrueTarget = findTrueArea(endMat, endCate, endColor, -1);
		//std::cout << "Find endTrueTarget." << std::endl;

		cv::Point partCenStart, partCenEnd;
		partCenStart = cv::Point(startTrueTarget.x + startTrueTarget.width / 2, startTrueTarget.y + startTrueTarget.height / 2);
		partCenEnd = cv::Point(endTrueTarget.x + endTrueTarget.width / 2, endTrueTarget.y + endTrueTarget.height / 2);

		cv::Point trueStartCen = startAxis + partCenStart;
		cv::Point trueEndCen = endAxis + partCenEnd;
		distancePixel = pow(pow(abs(trueStartCen.x - trueEndCen.x), 2) + pow(abs(trueStartCen.y - trueEndCen.y), 2), 0.5);
	}
	else {
		if (startCate == 2) {
			cv::RotatedRect startTapeTrueTarget = findTapeDetailTrueArea(startMat, startCate, startColor);
			//std::cout << "Find startTapeTrueTarget." << std::endl;
			cv::Rect endTrueTarget = findTrueArea(endMat, endCate, endColor, -1);
			//std::cout << "Find endTrueTarget." << std::endl;

			cv::Point partCenEnd;
			partCenEnd = cv::Point(endTrueTarget.x + endTrueTarget.width / 2, endTrueTarget.y + endTrueTarget.height / 2);
			cv::Point2f P[4];
			startTapeTrueTarget.points(P);
			std::vector<cv::Point> midPoint(4);	// 存放四条边的中点
			cv::Point trueEndCen = endAxis + partCenEnd;
			int minDistanceIdx = 0;
			double minDistancePixel = 100000.0;
			for (int i = 0; i <= 3; ++i) {
				midPoint[i] = static_cast<cv::Point>((P[i] + P[(i + 1) % 4]) / 2);
				cv::Point trueStartCen = startAxis + midPoint[i];
				double thisDistance = pow(pow(abs(trueStartCen.x - trueEndCen.x), 2) + pow(abs(trueStartCen.y - trueEndCen.y), 2), 0.5);
				if (thisDistance < minDistancePixel) {
					minDistancePixel = thisDistance;
					minDistanceIdx = i;
				}
			}
			distancePixel = static_cast<int>(minDistancePixel);
		}
		else if (endCate == 2) {
			cv::Rect startTrueTarget = findTrueArea(startMat, startCate, startColor, -1);
			//std::cout << "Find startTrueTarget." << std::endl;
			cv::RotatedRect endTapeTrueTarget = findTapeDetailTrueArea(endMat, endCate, endColor);
			//std::cout << "Find endTapeTrueTarget." << std::endl;

			cv::Point partCenStart;
			partCenStart = cv::Point(startTrueTarget.x + startTrueTarget.width / 2, startTrueTarget.y + startTrueTarget.height / 2);
			cv::Point2f P[4];
			endTapeTrueTarget.points(P);
			std::vector<cv::Point> midPoint(4);	// 存放四条边的中点
			cv::Point trueStartCen = startAxis + partCenStart;
			int minDistanceIdx = 0;
			double minDistancePixel = 100000.0;
			for (int i = 0; i <= 3; ++i) {
				midPoint[i] = static_cast<cv::Point>((P[i] + P[(i + 1) % 4]) / 2);
				cv::Point trueEndCen = endAxis + midPoint[i];
				double thisDistance = pow(pow(abs(trueEndCen.x - trueStartCen.x), 2) + pow(abs(trueEndCen.y - trueStartCen.y), 2), 0.5);
				if (thisDistance < minDistancePixel) {
					minDistancePixel = thisDistance;
					minDistanceIdx = i;
				}
			}
			distancePixel = static_cast<int>(minDistancePixel);
		}
	}
	std::cout << " pNorm:" << pixelNorm << " pOffset:" << pixelOffset << " tDistance:" << distancePixel;
	if ((pixelNorm - pixelOffset) <= distancePixel && (pixelNorm + pixelOffset) >= distancePixel)
		return true;
	return false;
}

cv::Mat ImageProcess::posDetectAndDisplay(cv::Mat &startMat, int startCate, int startColor, cv::Point startAxis, cv::Mat &endMat, int endCate, int endColor, cv::Point endAxis,
	int posLengthNorm, int posLengthOffset, int &pixelNorm, int &pixelOffset) {
	
	// 上下拼接
	cv::Mat combineMat = cv::Mat::ones(cv::Size(startMat.cols + endMat.cols, startMat.rows + endMat.rows), CV_8UC3);

	// 先默认全部找中心点
	if (startCate != 2 && endCate != 2) {	// 两个都不是胶布直接取中心点
		cv::Rect startTrueTarget = findTrueArea(startMat, startCate, startColor, -1);
		std::cout << "Find startTrueTarget." << std::endl;
		cv::Rect endTrueTarget = findTrueArea(endMat, endCate, endColor, -1);
		std::cout << "Find endTrueTarget." << std::endl;

		cv::Point partCenStart, partCenEnd;
		partCenStart = cv::Point(startTrueTarget.x + startTrueTarget.width / 2, startTrueTarget.y + startTrueTarget.height / 2);
		partCenEnd = cv::Point(endTrueTarget.x + endTrueTarget.width / 2, endTrueTarget.y + endTrueTarget.height / 2);
		cv::rectangle(startMat, startTrueTarget, cv::Scalar(0, 255, 0), 2);
		cv::circle(startMat, partCenStart, 4, cv::Scalar(0, 0, 255), -1);
		cv::rectangle(endMat, endTrueTarget, cv::Scalar(0, 255, 0), 2);
		cv::circle(endMat, partCenEnd, 4, cv::Scalar(0, 0, 255), -1);
		
		// 计算两点间的距离
		cv::Point trueStartCen = startAxis + partCenStart;
		cv::Point trueEndCen = endAxis + partCenEnd;
		double distancePixel = pow(pow(abs(trueStartCen.x - trueEndCen.x), 2) + pow(abs(trueStartCen.y - trueEndCen.y), 2), 0.5);
		pixelNorm = static_cast<int>(distancePixel);
		pixelOffset = static_cast<int>(static_cast<double>(posLengthOffset) / posLengthNorm * distancePixel);
	}
	else {	// 其中有一个是胶布
		if (startCate == 2) {
			cv::RotatedRect startTapeTrueTarget = findTapeDetailTrueArea(startMat, startCate, startColor);
			std::cout << "Find startTapeTrueTarget." << std::endl;
			cv::Rect endTrueTarget = findTrueArea(endMat, endCate, endColor, -1);
			std::cout << "Find endTrueTarget." << std::endl;
			
			cv::Point partCenEnd;
			partCenEnd = cv::Point(endTrueTarget.x + endTrueTarget.width / 2, endTrueTarget.y + endTrueTarget.height / 2);
			cv::rectangle(endMat, endTrueTarget, cv::Scalar(0, 255, 0), 2);
			cv::circle(endMat, partCenEnd, 4, cv::Scalar(0, 0, 255), -1);
			cv::Point2f P[4];
			startTapeTrueTarget.points(P);
			std::vector<cv::Point> midPoint(4);	// 存放四条边的中点
			cv::Point trueEndCen = endAxis + partCenEnd;
			int minDistanceIdx = 0;
			double minDistancePixel = 100000.0;
			for (int i = 0; i <= 3; ++i) {
				midPoint[i] = static_cast<cv::Point>((P[i] + P[(i + 1) % 4]) / 2);
				cv::Point trueStartCen = startAxis + midPoint[i];
				double thisDistance = pow(pow(abs(trueStartCen.x - trueEndCen.x), 2) + pow(abs(trueStartCen.y - trueEndCen.y), 2), 0.5);
				if (thisDistance < minDistancePixel) {
					minDistancePixel = thisDistance;
					minDistanceIdx = i;
				}
			}
			
			for (int i = 0; i <= 3; ++i) {
				cv::line(startMat, P[i], P[(i + 1) % 4], cv::Scalar(0, 255, 0), 2);
			}
			cv::circle(startMat, midPoint[minDistanceIdx], 4, cv::Scalar(0, 0, 255), -1);
			pixelNorm = static_cast<int>(minDistancePixel);
			pixelOffset = static_cast<int>(static_cast<double>(posLengthOffset) / posLengthNorm * minDistancePixel);
		}
		else if (endCate == 2) {
			cv::Rect startTrueTarget = findTrueArea(startMat, startCate, startColor, -1);
			std::cout << "Find startTrueTarget." << std::endl;
			cv::RotatedRect endTapeTrueTarget = findTapeDetailTrueArea(endMat, endCate, endColor);
			std::cout << "Find endTapeTrueTarget." << std::endl;

			cv::Point partCenStart;
			partCenStart = cv::Point(startTrueTarget.x + startTrueTarget.width / 2, startTrueTarget.y + startTrueTarget.height / 2);
			cv::rectangle(startMat, startTrueTarget, cv::Scalar(0, 255, 0), 2);
			cv::circle(startMat, partCenStart, 4, cv::Scalar(0, 0, 255), -1);
			cv::Point2f P[4];
			endTapeTrueTarget.points(P);
			std::vector<cv::Point> midPoint(4);	// 存放四条边的中点
			// std::cout << "startAxis: " << startAxis << std::endl;
			cv::Point trueStartCen = startAxis + partCenStart;
			// std::cout << trueStartCen << std::endl;
			int minDistanceIdx = 0;
			double minDistancePixel = 100000.0;
			// std::cout << "endAxis: " << endAxis << std::endl;
			for (int i = 0; i <= 3; ++i) {
				midPoint[i] = static_cast<cv::Point>((P[i] + P[(i + 1) % 4]) / 2);
				cv::Point trueEndCen = endAxis + midPoint[i];
				// std::cout << trueEndCen << " " << midPoint[i] << std::endl;
				double thisDistance = pow(pow(abs(trueEndCen.x - trueStartCen.x), 2) + pow(abs(trueEndCen.y - trueStartCen.y), 2), 0.5);
				if (thisDistance < minDistancePixel) {
					minDistancePixel = thisDistance;
					minDistanceIdx = i;
				}
			}
			//std::cout << minDistanceIdx << std::endl;
			for (int i = 0; i <= 3; ++i) {
				cv::line(endMat, P[i], P[(i + 1) % 4], cv::Scalar(0, 255, 0), 2);
			}
			cv::circle(endMat, midPoint[minDistanceIdx], 4, cv::Scalar(0, 0, 255), -1);
			pixelNorm = static_cast<int>(minDistancePixel);
			pixelOffset = static_cast<int>(static_cast<double>(posLengthOffset) / posLengthNorm * minDistancePixel);
		}
	}
	// 构图
	std::cout << "Start Concate." << std::endl;
	startMat.copyTo(combineMat(cv::Rect(0, 0, startMat.cols, startMat.rows)));
	endMat.copyTo(combineMat(cv::Rect(startMat.cols, startMat.rows, endMat.cols, endMat.rows)));
	std::cout << "end Concate." << std::endl;

	return combineMat;
}

void ImageProcess::adjustMatChannel(cv::Mat& src, int chl) {
	// 

	if (src.channels() != 1) {
		std::cout << "Only One Channel Src." << std::endl;
		return;
	}

	std::vector <cv::Mat> mv;
	split(src, mv);
	cv::Mat dstChl = mv[chl];

	int nRows = dstChl.rows;
	int nCols = dstChl.cols;
	uchar *pDataMat;
	//Vec3d
	int pixMax = 0, pixMin = 255;
	for (int j = 0; j < nRows; j++)
	{
		pDataMat = dstChl.ptr<uchar>(j);//ptr<>()得到的是一行指针
		for (int i = 0; i < nCols; i++)
		{
			if (pDataMat[i] > pixMax)
				pixMax = pDataMat[i];
			if (pDataMat[i] < pixMin)
				pixMin = pDataMat[i];
		}
	}
	pixMin = 0;
	//对比度拉伸映射
	for (int j = 0; j < nRows; j++)
	{
		pDataMat = dstChl.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++)
		{
			pDataMat[i] = (pDataMat[i] - pixMin) * 255 / (pixMax - pixMin);
		}
	}
	// cout << src.channels() << endl;
	if (src.channels() == 3) {

		for (int j = 0; j < nRows; j++)
		{
			pDataMat = dstChl.ptr<uchar>(j);
			for (int i = 0; i < nCols; i++) {
				src.at<cv::Vec3b>(j, i)[chl] = pDataMat[i];
			}
		}
	}
	else {
		uchar *pDataOriginMat;
		for (int j = 0; j < nRows; j++)
		{
			pDataOriginMat = src.ptr<uchar>(j);
			pDataMat = dstChl.ptr<uchar>(j);
			for (int i = 0; i < nCols; i++) {
				pDataOriginMat[i] = pDataMat[i];
			}
		}
	}

}

void ImageProcess::myFloodFill(cv::Mat &src, int x, int y) {
	src.at<uchar>(x, y) = 0;
	if (x > 0 && src.at<uchar>(x - 1, y) == 255) myFloodFill(src, x - 1, y);
	if (y > 0 && src.at<uchar>(x, y - 1) == 255) myFloodFill(src, x, y - 1);
	if (x < src.rows - 1 && src.at<uchar>(x + 1, y) == 255) myFloodFill(src, x + 1, y);
	if (y < src.cols - 1 && src.at<uchar>(x, y + 1) == 255) myFloodFill(src, x, y + 1);
}

void ImageProcess::floodFillEdge(cv::Mat &src) {
	if (src.type() != CV_8UC1) {
		std::cout << "src.type() != CV_8UC1" << std::endl;
		return;
	}
	int width = src.cols;
	int height = src.rows;
	//std::cout << "rows: " << rows << " cols: " << cols << src.size()<< std::endl;
	cv::Rect ccomp;
	cv::Mat mask = cv::Mat(src.rows + 2, src.cols + 2, CV_8UC1);
	// 上下边界

	for (int i = 0; i < src.cols; ++i) {
		for (int j = 0; j < src.rows; ++j) {
			//cout << static_cast<int>(src.at<uchar>(j, i)) << endl;
			if ((i == 0 || j == 0 || i == src.cols - 1 || j == src.rows - 1) && src.at<uchar>(j, i) == 255) {
				//cv::floodFill(src, cv::Point(j, i), 0, &ccomp, cv::Scalar(5, 5, 5), cv::Scalar(5, 5, 5));
				//src.at<uchar>(j, i) = 255;
				myFloodFill(src, j, i);
			}
		}
	}
}

cv::Rect getMaskRoi(cv::Mat &src) {
	cv::Rect rect;
	rect.x = src.cols / 4;
	rect.y = src.rows / 4;
	rect.width = src.cols / 2;
	rect.height = src.rows / 2;
	return rect;
}
cv::RotatedRect ImageProcess::getTapeDetailRect(cv::Mat &src, std::string targetColor, int cateIdx) {
	if (cateIdx != 2) {
		std::cout << "Only Tape." << std::endl;
		return cv::RotatedRect();
	}
	cv::Mat gray;
	cv::cvtColor(src, gray, cv::COLOR_RGB2GRAY);
	adjustMatChannel(gray, 0);
	cv::Mat hsv;
	cv::cvtColor(src, hsv, cv::COLOR_RGB2HSV);
	int nRows = hsv.rows;
	int nCols = hsv.cols;
	uchar *pDataMat;
	for (int j = 0; j < nRows; j++)
	{
		pDataMat = gray.ptr<uchar>(j);
		for (int i = 0; i < nCols; i++) {
			hsv.at<cv::Vec3b>(j, i)[2] = pDataMat[i];
		}
	}
	cv::Mat mask;
	inRange(hsv,
		cv::Scalar(HSVColorForGetTrueTarget[targetColor].hMin, HSVColorForGetTrueTarget[targetColor].sMin, HSVColorForGetTrueTarget[targetColor].vMin),
		cv::Scalar(HSVColorForGetTrueTarget[targetColor].hMax, HSVColorForGetTrueTarget[targetColor].sMax, HSVColorForGetTrueTarget[targetColor].vMax), mask);
	cv::Mat afterMor;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(-1, -1));
	cv::morphologyEx(mask, afterMor, CV_MOP_OPEN, kernel);
	cv::morphologyEx(afterMor, afterMor, CV_MOP_DILATE, kernel);
	//cv::imshow("afterMor", afterMor);
	floodFillEdge(afterMor);
	kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7), cv::Point(-1, -1));
	cv::morphologyEx(afterMor, afterMor, CV_MOP_DILATE, kernel);
	std::vector<std::vector<cv::Point>>contours;
	std::vector<cv::Vec4i>hierarchy;
	findContours(afterMor, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
	cv::RotatedRect boundRectMax;
	if (contours.size() > 0) {
		if (contours.size() != 1) {
			int maxArea = INT_MIN;
			for (int i = 0; i < contours.size(); i++)
			{
				//cv::Rect boundRect = cv::boundingRect(cv::Mat(contours[i]));
				cv::RotatedRect rect = minAreaRect(contours[i]);
				int thisArea = static_cast<int>(rect.size.area());
				if (thisArea > maxArea) {
					maxArea = thisArea;
					boundRectMax = rect;
				}
			}
		}
		else {
			//boundRectMax = cv::boundingRect(cv::Mat(contours[0]));
			boundRectMax = minAreaRect(contours[0]);
		}
		//cv::imshow("mask", mask);
		//cv::imshow("afterMor", afterMor);
		//cv::waitKey(0);
	}
	else {
		std::cout << "Contours Find Error." << std::endl;
		// 找不到是因为线与目标的颜色产生冲突
		//boundRectMax = getMaskRoi(mask);
		return cv::RotatedRect();
	}
	return boundRectMax;
}

cv::Rect ImageProcess::getTargetColorRect(cv::Mat &src, std::string targetColor, int cateIdx, int curBlockIdx, cv::Mat &getMask) {
	//if (cateIdx != 2) {
		cv::Mat gray;
		cv::cvtColor(src, gray, cv::COLOR_RGB2GRAY);
		adjustMatChannel(gray, 0);
		cv::Mat hsv;
		cv::cvtColor(src, hsv, cv::COLOR_RGB2HSV);
		int nRows = hsv.rows;
		int nCols = hsv.cols;
		uchar *pDataMat;
		for (int j = 0; j < nRows; j++)
		{
			pDataMat = gray.ptr<uchar>(j);
			for (int i = 0; i < nCols; i++) {
				hsv.at<cv::Vec3b>(j, i)[2] = pDataMat[i];
			}
		}
		cv::Mat mask;
		inRange(hsv,
			cv::Scalar(HSVColorForGetTrueTarget[targetColor].hMin, HSVColorForGetTrueTarget[targetColor].sMin, HSVColorForGetTrueTarget[targetColor].vMin),
			cv::Scalar(HSVColorForGetTrueTarget[targetColor].hMax, HSVColorForGetTrueTarget[targetColor].sMax, HSVColorForGetTrueTarget[targetColor].vMax), mask);
		//cv::imshow("mask", mask);
		//if (targetColor == "RedOrPink") {
		//	cv::Mat maskRed;
		//	inRange(hsv, cv::Scalar(0, 43, 46), cv::Scalar(20, 255, 255), maskRed);
		//	mask = mask | maskRed;
		//	cv::imshow("maskRed", maskRed);
		//}
		cv::Mat afterMor, afterMorCloneBeforeff, toSaveMask;
		cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		//cv::morphologyEx(mask, afterMor, CV_MOP_OPEN, kernel);
		//cv::morphologyEx(afterMor, afterMor, CV_MOP_DILATE, kernel);
		cv::morphologyEx(mask, afterMor, CV_MOP_DILATE, kernel);
		//cv::imshow("mask2afterMor", afterMor);
		afterMorCloneBeforeff = afterMor.clone();
		if ((cateIdx == 1 && targetColor == "White")
			|| (cateIdx == 0 && targetColor == "Gray")) {	// 针对白T18
			//cv::imshow("afterMor1", afterMor);
			kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
			cv::morphologyEx(afterMor, afterMor, CV_MOP_ERODE, kernel);
			//cv::imshow("afterMor", afterMor);
		}
		floodFillEdge(afterMor);
		// 去除小白斑
		int area = afterMor.rows * afterMor.cols / 100;
		RemoveSmallRegion(afterMor, afterMor, area, 1, 1);	// 小于1/100认定为斑点
		//cv::imshow("RemoveSmallRegion", afterMor);
		if (cateIdx == 3 && targetColor == "White") {	// 针对白插头
			kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(19, 19));
			cv::morphologyEx(afterMor, toSaveMask, CV_MOP_DILATE, kernel);
			//cv::imshow("afterMor2toSaveMask", toSaveMask);
			kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(23, 23));
			cv::morphologyEx(toSaveMask, toSaveMask, CV_MOP_ERODE, kernel);
		}
		else
		{
			kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));
			cv::morphologyEx(afterMor, toSaveMask, CV_MOP_DILATE, kernel);
			//cv::imshow("afterMor2toSaveMask", toSaveMask);
			kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(19, 19));
			cv::morphologyEx(toSaveMask, toSaveMask, CV_MOP_ERODE, kernel);
		}
		//cv::imshow("toSaveMask2toSaveMask", toSaveMask);
		
		if (curBlockIdx > 0)
			cv::imwrite(".\\mask\\" + curNewModelName + "\\" + std::to_string(curBlockIdx) + ".jpg", toSaveMask);
		else {
			cv::cvtColor(toSaveMask, getMask, CV_GRAY2RGB);
			// getMask = toSaveMask.clone();
		}
		//cv::imshow("floodFillEdge", afterMor);
		kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
		cv::morphologyEx(afterMor, afterMor, CV_MOP_DILATE, kernel);
		//cv::imshow("afterMor2afterMor", afterMor);
		
		std::vector<std::vector<cv::Point>>contours;
		std::vector<cv::Vec4i>hierarchy;
		findContours(afterMor, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
		cv::Rect boundRectMax;
		if (contours.size() > 0) {
			if (contours.size() != 1) {
				int maxArea = INT_MIN;
				for (int i = 0; i < contours.size(); i++)
				{
					cv::Rect boundRect = cv::boundingRect(cv::Mat(contours[i]));
					int thisArea = boundRect.width * boundRect.height;
					if (thisArea > maxArea) {
						maxArea = thisArea;
						boundRectMax = boundRect;
					}
				}
			}
			else {
				boundRectMax = cv::boundingRect(cv::Mat(contours[0]));
			}
			//cv::imshow("mask", mask);
			//cv::imshow("afterMor", afterMor);
			//cv::waitKey(0);
		}
		else {
			//cv::imshow("mask", mask);
			//cv::imshow("afterMor", afterMor);
			//cv::waitKey(0);
			std::cout << "Contours Find Error[1]." << std::endl;
			// 找不到是因为线与目标的颜色产生冲突
			//boundRectMax = getMaskRoi(mask);
			contours.clear();
			hierarchy.clear();
			
			
			kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(11, 11));
			cv::morphologyEx(afterMorCloneBeforeff, toSaveMask, CV_MOP_DILATE, kernel);
			kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));
			cv::morphologyEx(toSaveMask, toSaveMask, CV_MOP_ERODE, kernel);
			if (curBlockIdx > 0)
				cv::imwrite(".\\mask\\" + curNewModelName + "\\" + std::to_string(curBlockIdx) + ".jpg", toSaveMask);
			else {
				//getMask = toSaveMask.clone();
				cv::cvtColor(toSaveMask, getMask, CV_GRAY2RGB);
			}
			kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
			cv::morphologyEx(afterMorCloneBeforeff, afterMorCloneBeforeff, CV_MOP_DILATE, kernel);
			
			findContours(afterMorCloneBeforeff, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
			if (contours.size() > 0) {
				if (contours.size() != 1) {
					int maxArea = INT_MIN;
					for (int i = 0; i < contours.size(); i++)
					{
						cv::Rect boundRect = cv::boundingRect(cv::Mat(contours[i]));
						int thisArea = boundRect.width * boundRect.height;
						if (thisArea > maxArea) {
							maxArea = thisArea;
							boundRectMax = boundRect;
						}
					}
				}
				else {
					boundRectMax = cv::boundingRect(cv::Mat(contours[0]));
				}
			}
			else {
				std::cout << "Contours Find Error[2]." << std::endl;
				boundRectMax = getMaskRoi(mask);
			}
		}
		return boundRectMax;
	//}
	//else {	// 胶布
	//	return cv::Rect();
	//}
}

cv::RotatedRect ImageProcess::findTapeDetailTrueArea(cv::Mat &src, int cateIdx, int color) {
	cv::RotatedRect targetRect;
	cv::Mat srcCopy;
//	cv::imshow("src", src);
	cv::cvtColor(src, srcCopy, cv::COLOR_BGR2RGB);
//	cv::imshow("srcCopy", srcCopy);
	//// 红(0)、白(1)、黑(2)、黄(3)、绿(4)、灰(5)
	if (color == 0) {	// 红
						//targetRect = getTargetColorRect(srcCopy, "RedOrPink", cateIdx);
		targetRect = getTapeDetailRect(srcCopy, "Red", cateIdx);
	}
	else if (color == 1 || color == 5) {// 灰/白
										//targetRect = getTargetColorRect(srcCopy, "GrayOrWhite", cateIdx);
		if (color == 5)
			targetRect = getTapeDetailRect(src, "Gray", cateIdx);
		else
			targetRect = getTapeDetailRect(src, "White", cateIdx);
	}
	else if (color == 2) {	// 黑
		targetRect = getTapeDetailRect(srcCopy, "Black", cateIdx);
	}
	else if (color == 3) {	// 黄
		targetRect = getTapeDetailRect(srcCopy, "Yellow", cateIdx);
	}
	else if (color == 4) {	// 绿
		targetRect = getTapeDetailRect(srcCopy, "Green", cateIdx);
	}
	return targetRect;
}

cv::Rect ImageProcess::findTrueArea(cv::Mat &src, int cateIdx, int color, int curBlockIdx) {
	cv::Rect targetRect;
	cv::Mat srcCopy;
	cv::cvtColor(src, srcCopy, cv::COLOR_BGR2RGB);
	switch (cateIdx)
	{
	case 0: {// 花结束带
		if (color < 2) {	// 灰/白
			//targetRect = getTargetColorRect(srcCopy, "GrayOrWhite", cateIdx);
			if (color == 0)
				targetRect = getTargetColorRect(srcCopy, "Gray", cateIdx, curBlockIdx, cv::Mat());
			else
				targetRect = getTargetColorRect(srcCopy, "White", cateIdx, curBlockIdx, cv::Mat());
		}
		else {	// 粉
			//targetRect = getTargetColorRect(srcCopy, "RedOrPink", cateIdx);
			targetRect = getTargetColorRect(srcCopy, "Pink", cateIdx, curBlockIdx, cv::Mat());
		}
		break;
	}
	case 1: {// T18R
		switch (color)
		{
		case 0: {	// 白
			//targetRect = getTargetColorRect(srcCopy, "GrayOrWhite", cateIdx);
			 targetRect = getTargetColorRect(srcCopy, "White", cateIdx, curBlockIdx, cv::Mat());
			break;
		}
		case 1: {	// 黑
			targetRect = getTargetColorRect(srcCopy, "Black", cateIdx, curBlockIdx, cv::Mat());
			break;
		}
		case 2: {	// 黄
			targetRect = getTargetColorRect(srcCopy, "Yellow", cateIdx, curBlockIdx, cv::Mat());
			break;
		}
		default:
			break;
		}
		break;
	}
	case 2: {// 胶布
		//// 红(0)、白(1)、黑(2)、黄(3)、绿(4)、灰(5)
		if (color == 0) {	// 红
			//targetRect = getTargetColorRect(srcCopy, "RedOrPink", cateIdx);
			targetRect = getTargetColorRect(srcCopy, "Red", cateIdx, curBlockIdx, cv::Mat());
		}
		else if (color == 1 || color == 5) {// 灰/白
			//targetRect = getTargetColorRect(srcCopy, "GrayOrWhite", cateIdx);
			if (color == 5)
				targetRect = getTargetColorRect(srcCopy, "Gray", cateIdx, curBlockIdx, cv::Mat());
			else
				targetRect = getTargetColorRect(srcCopy, "White", cateIdx, curBlockIdx, cv::Mat());
		}
		else if (color == 2) {	// 黑
			targetRect = getTargetColorRect(srcCopy, "Black", cateIdx, curBlockIdx, cv::Mat());
		}
		else if (color == 3) {	// 黄
			targetRect = getTargetColorRect(srcCopy, "Yellow", cateIdx, curBlockIdx, cv::Mat());
		}
		else if (color == 4) {	// 绿
			targetRect = getTargetColorRect(srcCopy, "Green", cateIdx, curBlockIdx, cv::Mat());
		}
		break;
	}
	case 3: {// 插头
		if (color == 0) {	// 黑
			targetRect = getTargetColorRect(srcCopy, "Black", cateIdx, curBlockIdx, cv::Mat());
		}
		else if (color == 1 || color == 4) {// 灰/白
			//targetRect = getTargetColorRect(srcCopy, "GrayOrWhite", cateIdx);
			if (color == 1)
				targetRect = getTargetColorRect(srcCopy, "White", cateIdx, curBlockIdx, cv::Mat());
			else
				targetRect = getTargetColorRect(srcCopy, "Gray", cateIdx, curBlockIdx, cv::Mat());
		}
		else if (color == 2) {	// 粉
			//targetRect = getTargetColorRect(srcCopy, "RedOrPink", cateIdx);
			targetRect = getTargetColorRect(srcCopy, "Pink", cateIdx, curBlockIdx, cv::Mat());
		}
		else if (color == 3) {	// 蓝
			targetRect = getTargetColorRect(srcCopy, "Blue", cateIdx, curBlockIdx, cv::Mat());
		}
		else if (color == 5) {	// 绿
			targetRect = getTargetColorRect(srcCopy, "Green", cateIdx, curBlockIdx, cv::Mat());
		}
		else if (color == 6) {	// 黄
			targetRect = getTargetColorRect(srcCopy, "Yellow", cateIdx, curBlockIdx, cv::Mat());
		}
		break;
	}
	default:
		break;
	}
	//
	return targetRect;
	//// 根据targetRect标记原图
	//cv::rectangle(src, targetRect, cv::Scalar(0, 255, 0), 2);
}

double ImageProcess::ssimDetect(cv::Mat &imgOrg, cv::Mat &imgComp)
{

	double C1 = 6.5025, C2 = 58.5225;
	cv::Mat image_ref = imgOrg.clone();
	cv::Mat image_obj = imgComp.clone();
	int width = image_ref.cols;
	int height = image_ref.rows;
	int width2 = image_obj.cols;
	int height2 = image_obj.rows;
	double mean_x = 0;
	double mean_y = 0;
	double sigma_x = 0;
	double sigma_y = 0;
	double sigma_xy = 0;
	for (int v = 0; v < height; v++)
	{
		for (int u = 0; u < width; u++)
		{
			mean_x += image_ref.at<uchar>(v, u);
			mean_y += image_obj.at<uchar>(v, u);

		}
	}
	mean_x = mean_x / width / height;
	mean_y = mean_y / width / height;
	for (int v = 0; v < height; v++)
	{
		for (int u = 0; u < width; u++)
		{
			sigma_x += (image_ref.at<uchar>(v, u) - mean_x)* (image_ref.at<uchar>(v, u) - mean_x);
			sigma_y += (image_obj.at<uchar>(v, u) - mean_y)* (image_obj.at<uchar>(v, u) - mean_y);
			sigma_xy += abs((image_ref.at<uchar>(v, u) - mean_x)* (image_obj.at<uchar>(v, u) - mean_y));
		}
	}
	sigma_x = sigma_x / (width*height - 1);
	sigma_y = sigma_y / (width*height - 1);
	sigma_xy = sigma_xy / (width*height - 1);
	double fenzi = (2 * mean_x*mean_y + C1) * (2 * sigma_xy + C2);
	double fenmu = (mean_x*mean_x + mean_y * mean_y + C1) * (sigma_x + sigma_y + C2);
	double ssim = fenzi / fenmu;
	return ssim;
}

cv::Mat ImageProcess::findLengthDisplay(cv::Mat &src, int &pixelNorm) {
	cv::Mat matTmp = src.clone();
	int height = matTmp.rows, width = matTmp.cols;//图像的高和宽
	int maxVal = INT_MIN;
	int minVal = INT_MAX;
	int startIdx = 0, endIdx = 0;
	std::vector<long long> projArray(width, 0);
	for (int col = 0; col < width; ++col)
	{
		int count = 0;
		for (int row = 0; row < height; ++row)
		{
			int curPixelVal = static_cast<int>(matTmp.at<uchar>(row, col));
			if (curPixelVal > 0) {
				count += 1;
				projArray[col] += curPixelVal;
			}
		}
		if (count > 0) {
			projArray[col] /= count;
			if (startIdx == 0 && endIdx == 0)
				startIdx = col;
		}
		else {
			projArray[col] = 0;
			if (startIdx != 0 && endIdx == 0)
				endIdx = col;
		}

		maxVal = maxVal > projArray[col] ? maxVal : projArray[col];
		if (projArray[col] > 0) {
			minVal = minVal < projArray[col] ? minVal : projArray[col];
		}
	}

	endIdx = (endIdx == 0 ? width - 1 : endIdx);

	// std::cout << "startIdx: " << startIdx << " endIdx: " << endIdx << std::endl;
	int thrd = (maxVal + minVal) / 2;	// 划分阈值

	cv::Mat projImg(maxVal, width, CV_8U, cv::Scalar(255));
	for (int col = 0; col < width; ++col)
		cv::line(projImg, cv::Point(col, maxVal - projArray[col]), cv::Point(col, maxVal - 1), cv::Scalar::all(0));
	//cv::line(projImg, cv::Point(0, maxVal - thrd), cv::Point(width - 1, maxVal - thrd), cv::Scalar::all(0));
	//cv::imshow("projImg", projImg);

	int leftThrd = 0, rightThrd = 0;
	bool isLargerOrMin = false;	// 中间是大于阈值(true)，还是小于阈值(false)
	isLargerOrMin = projArray[(startIdx + endIdx) / 2] > thrd ? true : false;

	if (isLargerOrMin) {	// 中间大于阈值，找到开始小于阈值的边界
		for (int i = (startIdx + endIdx) / 2; i > startIdx; --i) {
			if (projArray[i] <= thrd) {
				leftThrd = i;
				break;
			}
		}
		for (int i = (startIdx + endIdx) / 2; i < endIdx; ++i) {	// 向右找
			if (projArray[i] <= thrd) {
				rightThrd = i;
				break;
			}
		}
	}
	else {	// 中间小于阈值，找到开始大于阈值的边界
		for (int i = (startIdx + endIdx) / 2; i > startIdx; --i) {
			if (projArray[i] >= thrd) {
				leftThrd = i;
				break;
			}
		}
		for (int i = (startIdx + endIdx) / 2; i < endIdx; ++i) {	// 向右找
			if (projArray[i] >= thrd) {
				rightThrd = i;
				break;
			}
		}
	}

	// std::cout << "leftThrd: " << leftThrd << " rightThrd: " << rightThrd << std::endl;
	cv::line(projImg, cv::Point(leftThrd, maxVal - projArray[leftThrd]), cv::Point(rightThrd, maxVal - projArray[rightThrd]), cv::Scalar::all(0));
	// cv::imshow("projImg", projImg);

	pixelNorm = rightThrd - leftThrd;

	return projImg;
}

std::pair<int, int> ImageProcess::getLengthForDisplay(cv::Mat &src) {
	cv::Mat matTmp = src.clone();
	int height = matTmp.rows, width = matTmp.cols;//图像的高和宽
	int maxVal = INT_MIN;
	int minVal = INT_MAX;
	int startIdx = 0, endIdx = 0;
	std::vector<long long> projArray(width, 0);
	for (int col = 0; col < width; ++col)
	{
		int count = 0;
		for (int row = 0; row < height; ++row)
		{
			int curPixelVal = static_cast<int>(matTmp.at<uchar>(row, col));
			if (curPixelVal > 0) {
				count += 1;
				projArray[col] += curPixelVal;
			}
		}
		if (count > 0) {
			projArray[col] /= count;
			if (startIdx == 0 && endIdx == 0)
				startIdx = col;
		}
		else {
			projArray[col] = 0;
			if (startIdx != 0 && endIdx == 0)
				endIdx = col;
		}

		maxVal = maxVal > projArray[col] ? maxVal : projArray[col];
		if (projArray[col] > 0) {
			minVal = minVal < projArray[col] ? minVal : projArray[col];
		}
	}

	endIdx = (endIdx == 0 ? width - 1 : endIdx);

	// std::cout << "startIdx: " << startIdx << " endIdx: " << endIdx << std::endl;
	int thrd = (maxVal + minVal) / 2;	// 划分阈值

	cv::Mat projImg(maxVal, width, CV_8U, cv::Scalar(255));
	for (int col = 0; col < width; ++col)
		cv::line(projImg, cv::Point(col, maxVal - projArray[col]), cv::Point(col, maxVal - 1), cv::Scalar::all(0));
	//cv::line(projImg, cv::Point(0, maxVal - thrd), cv::Point(width - 1, maxVal - thrd), cv::Scalar::all(0));
	//cv::imshow("projImg", projImg);

	int leftThrd = 0, rightThrd = 0;
	bool isLargerOrMin = false;	// 中间是大于阈值(true)，还是小于阈值(false)
	isLargerOrMin = projArray[(startIdx + endIdx) / 2] > thrd ? true : false;

	if (isLargerOrMin) {	// 中间大于阈值，找到开始小于阈值的边界
		for (int i = (startIdx + endIdx) / 2; i > startIdx; --i) {
			if (projArray[i] <= thrd) {
				leftThrd = i;
				break;
			}
		}
		for (int i = (startIdx + endIdx) / 2; i < endIdx; ++i) {	// 向右找
			if (projArray[i] <= thrd) {
				rightThrd = i;
				break;
			}
		}
	}
	else {	// 中间小于阈值，找到开始大于阈值的边界
		for (int i = (startIdx + endIdx) / 2; i > startIdx; --i) {
			if (projArray[i] >= thrd) {
				leftThrd = i;
				break;
			}
		}
		for (int i = (startIdx + endIdx) / 2; i < endIdx; ++i) {	// 向右找
			if (projArray[i] >= thrd) {
				rightThrd = i;
				break;
			}
		}
	}

	// std::cout << "leftThrd: " << leftThrd << " rightThrd: " << rightThrd << std::endl;
	cv::line(projImg, cv::Point(leftThrd, maxVal - projArray[leftThrd]), cv::Point(rightThrd, maxVal - projArray[rightThrd]), cv::Scalar::all(0));
	//cv::imshow("projImg", projImg);

	return std::make_pair(leftThrd, rightThrd);
}

void ImageProcess::cutDstRoi(cv::Mat &diffMask, cv::Rect &rect) {
	cv::Mat thrd;
	cv::cvtColor(diffMask, thrd, cv::COLOR_RGB2GRAY);
	//cv::threshold(diffMask, thrd, 0, 255, CV_THRESH_BINARY);
	//cv::imshow("thrd", thrd);
	//thrd = ~thrd;
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;//存储查找到的第i个轮廓的后[i][0]、前[i][1]、父[i][2]、子轮廓[i][3]
	findContours(thrd, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
	//Mat imageContours0 = Mat::zeros(thrd.size(), CV_8UC1);	//最小外接正矩形画布
	cv::Rect boundRectMax;
	if (contours.size() > 0) {
		if (contours.size() != 1) {

			int maxArea = INT_MIN;
			for (int i = 0; i < contours.size(); i++)
			{
				cv::Rect boundRect = cv::boundingRect(cv::Mat(contours[i]));
				int thisArea = boundRect.width * boundRect.height;
				if (thisArea > maxArea) {
					maxArea = thisArea;
					boundRectMax = boundRect;
				}
			}
			//diffMask = diffMask(cv::Range(), cv::Range());
		}
		else {
			boundRectMax = cv::boundingRect(cv::Mat(contours[0]));

		}
		rect = boundRectMax;
		diffMask = diffMask(cv::Range(boundRectMax.y, boundRectMax.y + boundRectMax.height), cv::Range(boundRectMax.x, boundRectMax.x + boundRectMax.width));
	}
	else {
		std::cout << "error" << std::endl;
	}

	//imshow("最小正外接矩形", imageContours0);
}

cv::Mat ImageProcess::lengthDetectAndDisplay(cv::Mat &src, bool isBlack, int lengthNorm, int lengthOffset, int &pixelNorm, int &pixelOffset, double &angle) {
	cv::Mat partRoi;
	cv::cvtColor(src, partRoi, cv::COLOR_BGR2RGB);
	cv::cvtColor(partRoi, partRoi, cv::COLOR_RGB2BGR);
	
	cv::Mat srcImg = partRoi.clone();
	cv::cvtColor(srcImg, srcImg, CV_RGB2GRAY);
	cv::Mat thrd(srcImg.size(), srcImg.type());
	cv::threshold(srcImg, thrd, 0, 255, CV_THRESH_OTSU);//大津法

	cv::Mat afterErodeAndDilate;
	cv::Mat structureElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));      //创建结构元
	cv::dilate(thrd, afterErodeAndDilate, structureElement);
	// 去除小黑斑
	int area = afterErodeAndDilate.rows * afterErodeAndDilate.cols / 100;
	RemoveSmallRegion(afterErodeAndDilate, afterErodeAndDilate, area, 0, 1);	// 小于1/100认定为斑点
	//cv::imshow("RemoveSmallRegion", afterErodeAndDilate);

	// 判断是否是黑
	double degree = 0.0;	// 旋转角度

	// 首先根据长宽判断需不需要旋转
	double widthDiffHeight = static_cast<double>(afterErodeAndDilate.cols) / afterErodeAndDilate.rows;
	cv::Mat diffMask;
	cv::Mat tmpRotatePartRoi;
	if (widthDiffHeight >= 0.5 && widthDiffHeight <= 2.0) {	// 认为需要旋转
		cv::Mat afterRotate;
		degree = CalcDegree(afterErodeAndDilate, afterRotate);
		// std::cout << "degree: " << degree << std::endl;
		// 原彩色图扩大并旋转
		cv::Mat afterRotatePartRoi;
		cv::Mat afterScalePartRoi(partRoi.size() * 2, partRoi.type(), cv::Scalar(255, 255, 255));
		partRoi.copyTo(afterScalePartRoi(cv::Rect(partRoi.cols / 2, partRoi.rows / 2, partRoi.cols, partRoi.rows)));
		rotateImage(afterScalePartRoi, afterRotatePartRoi, degree);
		tmpRotatePartRoi = afterRotatePartRoi.clone();
		// 原二值图扩大并旋转
		cv::Mat afterRotatePartBin;
		cv::Mat afterScaleBin(afterErodeAndDilate.size() * 2, afterErodeAndDilate.type(), cv::Scalar(255));
		afterErodeAndDilate.copyTo(afterScaleBin(cv::Rect(afterErodeAndDilate.cols / 2, afterErodeAndDilate.rows / 2,
			afterErodeAndDilate.cols, afterErodeAndDilate.rows)));
		rotateImage(afterScaleBin, afterRotatePartBin, degree);
		// 得到组合图像
		cv::cvtColor(afterRotatePartBin, afterRotatePartBin, CV_GRAY2RGB);
		diffMask = afterRotatePartRoi - afterRotatePartBin;
	}
	else if (widthDiffHeight < 0.5) {
		// 旋转90度
		degree = -90.0;
		// 原彩色图扩大并旋转
		cv::Mat afterRotatePartRoi;
		cv::Mat afterScalePartRoi(cv::Size(partRoi.rows, partRoi.rows) * 2, partRoi.type(), cv::Scalar(255, 255, 255));
		partRoi.copyTo(afterScalePartRoi(cv::Rect(partRoi.rows / 2, partRoi.rows / 2, partRoi.cols, partRoi.rows)));
		//cv::imshow("afterScalePartRoi", afterScalePartRoi);
		rotateImage(afterScalePartRoi, afterRotatePartRoi, degree);
		// 原二值图扩大并旋转
		cv::Mat afterRotatePartBin;
		cv::Mat afterScaleBin(cv::Size(afterErodeAndDilate.rows, afterErodeAndDilate.rows) * 2, afterErodeAndDilate.type(), cv::Scalar(255));
		afterErodeAndDilate.copyTo(afterScaleBin(cv::Rect(afterErodeAndDilate.rows / 2, afterErodeAndDilate.rows / 2,
			afterErodeAndDilate.cols, afterErodeAndDilate.rows)));
		rotateImage(afterScaleBin, afterRotatePartBin, degree);
		cv::cvtColor(afterRotatePartBin, afterRotatePartBin, CV_GRAY2RGB);
		diffMask = afterRotatePartRoi - afterRotatePartBin;
	}
	else
	{
		// 得到组合图像
		cv::cvtColor(afterErodeAndDilate, afterErodeAndDilate, CV_GRAY2RGB);
		diffMask = partRoi - afterErodeAndDilate;
	}
	cv::Rect minRoi;
	//cv::imshow("diffMask", diffMask);
	cutDstRoi(diffMask, minRoi);
	//cv::imshow("cutDstRoi", diffMask);
	cv::Mat diffMaskCP = diffMask.clone();

	// 转化为HSV空间
	cv::cvtColor(diffMask, diffMask, CV_RGB2HSV);
	// 分割三个通道
	std::vector<cv::Mat> mv(3);
	split(diffMask, mv);
	//int pixelNorm = 0;
	int length = 0;
	std::pair<int, int> leftAndRightBoard;
	if (!isBlack) {	// 非黑色采用H
					// projectX = getVerProjImage(mv[0]);
		leftAndRightBoard = getLengthForDisplay(mv[0]);

	}
	else {	// 黑色采用S+V
			// projectX = getVerProjImage(mv[2]);	// 感觉用V更好
		leftAndRightBoard = getLengthForDisplay(mv[2]);
	}
	length = leftAndRightBoard.second - leftAndRightBoard.first;
	
	cv::cvtColor(partRoi, partRoi, CV_RGB2BGR);
	if (degree == 0.0) {
		//std::cout << degree << " " << length << std::endl;
		cv::rectangle(partRoi, cv::Rect(leftAndRightBoard.first + minRoi.x, minRoi.y, length, minRoi.height), cv::Scalar(0, 255, 0), 2);
	}
	else if (degree == -90.0) {
		//std::cout << degree << " " << length << std::endl;
		int originX = minRoi.y - partRoi.rows / 2;
		int originY = partRoi.rows * 1.5 - minRoi.x - leftAndRightBoard.second;
		cv::rectangle(partRoi, cv::Rect(originX, originY, minRoi.height, length), cv::Scalar(0, 255, 0), 2);
	}
	else {
		//std::cout << degree << " " << length << std::endl;
		int originX = minRoi.x + leftAndRightBoard.first;
		int originY = minRoi.y;
		cv::rectangle(tmpRotatePartRoi, cv::Rect(originX, originY, length, minRoi.height), cv::Scalar(0, 255, 0), 2);
		//cv::imshow("tmpRotatePartRoi", tmpRotatePartRoi);
		rotateImage(tmpRotatePartRoi, tmpRotatePartRoi, -degree);
		//partRoi = tmpRotatePartRoi;
		//cv::imwrite("tmpRotatePartRoi.jpg", tmpRotatePartRoi);
		cv::Point leftUp;	// 遍历找到左上角的那个点
		int midCols = tmpRotatePartRoi.cols / 2;
		int midRows = tmpRotatePartRoi.rows / 2;
		//std::cout << midCols << "-" << midCols << std::endl;
		bool hasFind = false;
		for (int i = 0; i < midRows; ++i) {
			for (int j = 0; j < midCols; ++j) {
				//std::cout << i << "+" << j << std::endl;
				if (tmpRotatePartRoi.at<cv::Vec3b>(i, j) != cv::Vec3b(255, 255, 255)) {
					leftUp.x = j;
					leftUp.y = i;
					hasFind = true;
					break;
				}
			}
			if (hasFind)
				break;
		}
		//std::cout << leftUp.x << " " << leftUp.y << std::endl;
		cv::Rect tmpRect = cv::Rect(leftUp.x, leftUp.y, partRoi.cols, partRoi.rows);
		partRoi = tmpRotatePartRoi(tmpRect);

	}

	pixelNorm = length;
	pixelOffset = static_cast<int>(static_cast<double>(lengthOffset) / lengthNorm * pixelNorm);
	angle = degree;

	return partRoi;
	
}

int ImageProcess::getLength(cv::Mat &src) {
	cv::Mat matTmp = src.clone();
	int height = matTmp.rows, width = matTmp.cols;//图像的高和宽
	int maxVal = INT_MIN;
	int minVal = INT_MAX;
	int startIdx = 0, endIdx = 0;
	std::vector<long long> projArray(width, 0);
	for (int col = 0; col < width; ++col)
	{
		int count = 0;
		for (int row = 0; row < height; ++row)
		{
			int curPixelVal = static_cast<int>(matTmp.at<uchar>(row, col));
			if (curPixelVal > 0) {
				count += 1;
				projArray[col] += curPixelVal;
			}
		}
		if (count > 0) {
			projArray[col] /= count;
			if (startIdx == 0 && endIdx == 0)
				startIdx = col;
		}
		else {
			projArray[col] = 0;
			if (startIdx != 0 && endIdx == 0)
				endIdx = col;
		}

		maxVal = maxVal > projArray[col] ? maxVal : projArray[col];
		if (projArray[col] > 0) {
			minVal = minVal < projArray[col] ? minVal : projArray[col];
		}
	}

	endIdx = (endIdx == 0 ? width - 1 : endIdx);

	// std::cout << "startIdx: " << startIdx << " endIdx: " << endIdx << std::endl;
	int thrd = (maxVal + minVal) / 2;	// 划分阈值

	cv::Mat projImg(maxVal, width, CV_8U, cv::Scalar(255));
	for (int col = 0; col < width; ++col)
		cv::line(projImg, cv::Point(col, maxVal - projArray[col]), cv::Point(col, maxVal - 1), cv::Scalar::all(0));
	//cv::line(projImg, cv::Point(0, maxVal - thrd), cv::Point(width - 1, maxVal - thrd), cv::Scalar::all(0));
	//cv::imshow("projImg", projImg);

	int leftThrd = 0, rightThrd = 0;
	bool isLargerOrMin = false;	// 中间是大于阈值(true)，还是小于阈值(false)
	isLargerOrMin = projArray[(startIdx + endIdx) / 2] > thrd ? true : false;

	if (isLargerOrMin) {	// 中间大于阈值，找到开始小于阈值的边界
		for (int i = (startIdx + endIdx) / 2; i > startIdx; --i) {
			if (projArray[i] <= thrd) {
				leftThrd = i;
				break;
			}
		}
		for (int i = (startIdx + endIdx) / 2; i < endIdx; ++i) {	// 向右找
			if (projArray[i] <= thrd) {
				rightThrd = i;
				break;
			}
		}
	}
	else {	// 中间小于阈值，找到开始大于阈值的边界
		for (int i = (startIdx + endIdx) / 2; i > startIdx; --i) {
			if (projArray[i] >= thrd) {
				leftThrd = i;
				break;
			}
		}
		for (int i = (startIdx + endIdx) / 2; i < endIdx; ++i) {	// 向右找
			if (projArray[i] >= thrd) {
				rightThrd = i;
				break;
			}
		}
	}

	// std::cout << "leftThrd: " << leftThrd << " rightThrd: " << rightThrd << std::endl;
	cv::line(projImg, cv::Point(leftThrd, maxVal - projArray[leftThrd]), cv::Point(rightThrd, maxVal - projArray[rightThrd]), cv::Scalar::all(0));
	//cv::imshow("projImg", projImg);

	return rightThrd - leftThrd;
}

bool ImageProcess::isSatisfied(cv::Mat &src, bool isBlack, int lengthNorm, int lengthOffset, double angle) {	// 胶布长度
	cv::Mat partRoi;
	cv::cvtColor(src, partRoi, cv::COLOR_BGR2RGB);
	cv::cvtColor(partRoi, partRoi, cv::COLOR_RGB2BGR);

	cv::Mat srcImg = partRoi.clone();
	cv::cvtColor(srcImg, srcImg, CV_RGB2GRAY);
	cv::Mat thrd(srcImg.size(), srcImg.type());
	cv::threshold(srcImg, thrd, 0, 255, CV_THRESH_OTSU);//大津法

	cv::Mat afterErodeAndDilate;
	cv::Mat structureElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));      //创建结构元
	cv::dilate(thrd, afterErodeAndDilate, structureElement);
	// 去除小黑斑
	int area = afterErodeAndDilate.rows * afterErodeAndDilate.cols / 100;
	RemoveSmallRegion(afterErodeAndDilate, afterErodeAndDilate, area, 0, 1);	// 小于1/100认定为斑点
																				//cv::imshow("RemoveSmallRegion", afterErodeAndDilate);

																				// 判断是否是黑
	double degree = angle;	// 旋转角度

	// 首先根据长宽判断需不需要旋转
	double widthDiffHeight = static_cast<double>(afterErodeAndDilate.cols) / afterErodeAndDilate.rows;
	cv::Mat diffMask;
	if (widthDiffHeight >= 0.5 && widthDiffHeight <= 2.0) {	// 认为需要旋转
		cv::Mat afterRotate;
		//degree = CalcDegree(afterErodeAndDilate, afterRotate);
		// std::cout << "degree: " << degree << std::endl;
		// 原彩色图扩大并旋转
		cv::Mat afterRotatePartRoi;
		cv::Mat afterScalePartRoi(partRoi.size() * 2, partRoi.type(), cv::Scalar(255, 255, 255));
		partRoi.copyTo(afterScalePartRoi(cv::Rect(partRoi.cols / 2, partRoi.rows / 2, partRoi.cols, partRoi.rows)));
		rotateImage(afterScalePartRoi, afterRotatePartRoi, degree);
		// 原二值图扩大并旋转
		cv::Mat afterRotatePartBin;
		cv::Mat afterScaleBin(afterErodeAndDilate.size() * 2, afterErodeAndDilate.type(), cv::Scalar(255));
		afterErodeAndDilate.copyTo(afterScaleBin(cv::Rect(afterErodeAndDilate.cols / 2, afterErodeAndDilate.rows / 2,
			afterErodeAndDilate.cols, afterErodeAndDilate.rows)));
		rotateImage(afterScaleBin, afterRotatePartBin, degree);
		// 得到组合图像
		cv::cvtColor(afterRotatePartBin, afterRotatePartBin, CV_GRAY2RGB);
		diffMask = afterRotatePartRoi - afterRotatePartBin;
	}
	else if (widthDiffHeight < 0.5) {
		// 旋转90度
		//degree = -90.0;
		// 原彩色图扩大并旋转
		cv::Mat afterRotatePartRoi;
		cv::Mat afterScalePartRoi(cv::Size(partRoi.rows, partRoi.rows) * 2, partRoi.type(), cv::Scalar(255, 255, 255));
		partRoi.copyTo(afterScalePartRoi(cv::Rect(partRoi.rows / 2, partRoi.rows / 2, partRoi.cols, partRoi.rows)));
		//cv::imshow("afterScalePartRoi", afterScalePartRoi);
		rotateImage(afterScalePartRoi, afterRotatePartRoi, degree);
		// 原二值图扩大并旋转
		cv::Mat afterRotatePartBin;
		cv::Mat afterScaleBin(cv::Size(afterErodeAndDilate.rows, afterErodeAndDilate.rows) * 2, afterErodeAndDilate.type(), cv::Scalar(255));
		afterErodeAndDilate.copyTo(afterScaleBin(cv::Rect(afterErodeAndDilate.rows / 2, afterErodeAndDilate.rows / 2,
			afterErodeAndDilate.cols, afterErodeAndDilate.rows)));
		rotateImage(afterScaleBin, afterRotatePartBin, degree);
		cv::cvtColor(afterRotatePartBin, afterRotatePartBin, CV_GRAY2RGB);
		diffMask = afterRotatePartRoi - afterRotatePartBin;
	}
	else
	{
		// 得到组合图像
		cv::cvtColor(afterErodeAndDilate, afterErodeAndDilate, CV_GRAY2RGB);
		diffMask = partRoi - afterErodeAndDilate;
	}
	//cv::imshow("diffMask", diffMask);
	cutDstRoi(diffMask);

	// 转化为HSV空间
	cv::cvtColor(diffMask, diffMask, CV_RGB2HSV);
	// 分割三个通道
	std::vector<cv::Mat> mv(3);
	split(diffMask, mv);
	// x轴映射
	//cv::Mat projectX;
	int length = 0;
	if (!isBlack) {	// 非黑色采用H
					// projectX = getVerProjImage(mv[0]);
		length = getLength(mv[0]);
	}
	else {	// 黑色采用S+V
			// projectX = getVerProjImage(mv[2]);	// 感觉用V更好
		length = getLength(mv[2]);
	}
	//cv::imshow("projectX", projectX);

	//cv::waitKey(0);
	std::cout << " length:" << length << " lThrd:" << (lengthNorm - lengthOffset) << " hThrd:" << (lengthNorm + lengthOffset);
	if (length < (lengthNorm - lengthOffset) || length >(lengthNorm + lengthOffset))
		return false;
	return true;
}

void ImageProcess::cutDstRoi(cv::Mat &diffMask) {
	cv::Mat thrd;
	cv::cvtColor(diffMask, thrd, cv::COLOR_RGB2GRAY);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;//存储查找到的第i个轮廓的后[i][0]、前[i][1]、父[i][2]、子轮廓[i][3]
	findContours(thrd, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
	cv::Rect boundRectMax;
	if (contours.size() > 0) {
		if (contours.size() != 1) {
			int maxArea = INT_MIN;
			for (int i = 0; i < contours.size(); i++)
			{
				cv::Rect boundRect = cv::boundingRect(cv::Mat(contours[i]));
				int thisArea = boundRect.width * boundRect.height;
				if (thisArea > maxArea) {
					maxArea = thisArea;
					boundRectMax = boundRect;
				}
			}
		}
		else {
			boundRectMax = cv::boundingRect(cv::Mat(contours[0]));
		}
		diffMask = diffMask(cv::Range(boundRectMax.y, boundRectMax.y + boundRectMax.height), cv::Range(boundRectMax.x, boundRectMax.x + boundRectMax.width));
	}
	else {
		std::cout << "Contours None" << std::endl;
	}
}

//度数转换
double ImageProcess::DegreeTrans(double theta)
{
	double res = theta / CV_PI * 180;
	return res;
}

//逆时针旋转图像degree角度（原尺寸）    
void ImageProcess::rotateImage(cv::Mat src, cv::Mat& img_rotate, double degree)
{
	//旋转中心为图像中心    
	cv::Point2f center;
	center.x = float(src.cols / 2.0);
	center.y = float(src.rows / 2.0);
	int length = 0;
	length = sqrt(src.cols*src.cols + src.rows*src.rows);
	//计算二维旋转的仿射变换矩阵  
	cv::Mat M = getRotationMatrix2D(center, degree, 1);
	warpAffine(src, img_rotate, M, cv::Size(length, length), 1, 0, cv::Scalar(255, 255, 255));//仿射变换，背景色填充为白色  
}
#define ERROR 1234
//通过霍夫变换计算角度
double ImageProcess::CalcDegree(const cv::Mat &srcImage, cv::Mat &dst)
{
	cv::Mat midImage, dstImage;

	Canny(srcImage, midImage, 50, 200, 3);
	cv::cvtColor(midImage, dstImage, CV_GRAY2BGR);

	//通过霍夫变换检测直线
	std::vector<cv::Vec2f> lines;
	HoughLines(midImage, lines, 1, CV_PI / 180, 300, 0, 0);//第5个参数就是阈值，阈值越大，检测精度越高
														   //cout << lines.size() << endl;

														   //由于图像不同，阈值不好设定，因为阈值设定过高导致无法检测直线，阈值过低直线太多，速度很慢
														   //所以根据阈值由大到小设置了三个阈值，如果经过大量试验后，可以固定一个适合的阈值。

	if (!lines.size())
	{
		HoughLines(midImage, lines, 1, CV_PI / 180, 200, 0, 0);
	}
	//cout << lines.size() << endl;

	if (!lines.size())
	{
		HoughLines(midImage, lines, 1, CV_PI / 180, 50, 0, 0);
	}
	//std::cout << lines.size() << std::endl;
	if (!lines.size())
	{
		std::cout << "没有检测到直线！" << std::endl;
		return 0.0;
	}

	float sum = 0;
	//依次画出每条线段
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		cv::Point pt1, pt2;
		//cout << theta << endl;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		//只选角度最小的作为旋转角度
		sum += theta;

		cv::line(dstImage, pt1, pt2, cv::Scalar(55, 100, 195), 1); //Scalar函数用于调节线段颜色

														   // imshow("直线探测效果图", dstImage);
	}
	float average = sum / lines.size(); //对所有角度求平均，这样做旋转效果会更好

	//std::cout << "average theta:" << average << std::endl;

	double angle = DegreeTrans(average) - 90;

	rotateImage(dstImage, dst, angle);
	//imshow("直线探测效果图2", dstImage);
	return angle;
}

//=======函数实现=====================================================================
void ImageProcess::RemoveSmallRegion(cv::Mat &Src, cv::Mat &Dst, int AreaLimit, int CheckMode, int NeihborMode)
{
	int RemoveCount = 0;
	//新建一幅标签图像初始化为0像素点，为了记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查   
	//初始化的图像全部为0，未检查  
	cv::Mat PointLabel = cv::Mat::zeros(Src.size(), CV_8UC1);
	if (CheckMode == 1)//去除小连通区域的白色点  
	{
		//cout << "去除小连通域.";
		for (int i = 0; i < Src.rows; i++)
		{
			for (int j = 0; j < Src.cols; j++)
			{
				if (Src.at<uchar>(i, j) < 10)
				{
					PointLabel.at<uchar>(i, j) = 3;//将背景黑色点标记为合格，像素为3  
				}
			}
		}
	}
	else//去除孔洞，黑色点像素  
	{
		//cout << "去除孔洞";
		for (int i = 0; i < Src.rows; i++)
		{
			for (int j = 0; j < Src.cols; j++)
			{
				if (Src.at<uchar>(i, j) > 10)
				{
					PointLabel.at<uchar>(i, j) = 3;//如果原图是白色区域，标记为合格，像素为3  
				}
			}
		}
	}


	std::vector<cv::Point2i>NeihborPos;//将邻域压进容器  
	NeihborPos.push_back(cv::Point2i(-1, 0));
	NeihborPos.push_back(cv::Point2i(1, 0));
	NeihborPos.push_back(cv::Point2i(0, -1));
	NeihborPos.push_back(cv::Point2i(0, 1));
	if (NeihborMode == 1)
	{
		//cout << "Neighbor mode: 8邻域." << endl;
		NeihborPos.push_back(cv::Point2i(-1, -1));
		NeihborPos.push_back(cv::Point2i(-1, 1));
		NeihborPos.push_back(cv::Point2i(1, -1));
		NeihborPos.push_back(cv::Point2i(1, 1));
	}
	else int a = 0;//cout << "Neighbor mode: 4邻域." << endl;
	int NeihborCount = 4 + 4 * NeihborMode;
	int CurrX = 0, CurrY = 0;
	//开始检测  
	for (int i = 0; i < Src.rows; i++)
	{
		for (int j = 0; j < Src.cols; j++)
		{
			if (PointLabel.at<uchar>(i, j) == 0)//标签图像像素点为0，表示还未检查的不合格点  
			{   //开始检查  
				std::vector<cv::Point2i>GrowBuffer;//记录检查像素点的个数  
				GrowBuffer.push_back(cv::Point2i(j, i));
				PointLabel.at<uchar>(i, j) = 1;//标记为正在检查  
				int CheckResult = 0;

				for (int z = 0; z < GrowBuffer.size(); z++)
				{
					for (int q = 0; q < NeihborCount; q++)
					{
						CurrX = GrowBuffer.at(z).x + NeihborPos.at(q).x;
						CurrY = GrowBuffer.at(z).y + NeihborPos.at(q).y;
						if (CurrX >= 0 && CurrX<Src.cols&&CurrY >= 0 && CurrY<Src.rows)  //防止越界    
						{
							if (PointLabel.at<uchar>(CurrY, CurrX) == 0)
							{
								GrowBuffer.push_back(cv::Point2i(CurrX, CurrY));  //邻域点加入buffer    
								PointLabel.at<uchar>(CurrY, CurrX) = 1;           //更新邻域点的检查标签，避免重复检查    
							}
						}
					}
				}
				if (GrowBuffer.size()>AreaLimit) //判断结果（是否超出限定的大小），1为未超出，2为超出    
					CheckResult = 2;
				else
				{
					CheckResult = 1;
					RemoveCount++;//记录有多少区域被去除  
				}

				for (int z = 0; z < GrowBuffer.size(); z++)
				{
					CurrX = GrowBuffer.at(z).x;
					CurrY = GrowBuffer.at(z).y;
					PointLabel.at<uchar>(CurrY, CurrX) += CheckResult;//标记不合格的像素点，像素值为2  
				}
				//********结束该点处的检查**********    
			}
		}
	}
	CheckMode = 255 * (1 - CheckMode);
	//开始反转面积过小的区域    
	for (int i = 0; i < Src.rows; ++i)
	{
		for (int j = 0; j < Src.cols; ++j)
		{
			if (PointLabel.at<uchar>(i, j) == 2)
			{
				Dst.at<uchar>(i, j) = CheckMode;
			}
			else if (PointLabel.at<uchar>(i, j) == 3)
			{
				Dst.at<uchar>(i, j) = Src.at<uchar>(i, j);

			}
		}
	}
	//cout << RemoveCount << " objects removed." << endl;
}
//=======函数实现=====================================================================
void ImageProcess::setHSVColorForGetTrueTarget(std::unordered_map<std::string, HSVLimit> &new_HSVColorForGetTrueTarget) {
	HSVColorForGetTrueTarget.clear();
	for (auto iter = new_HSVColorForGetTrueTarget.begin(); iter != new_HSVColorForGetTrueTarget.end(); ++iter) {
		HSVColorForGetTrueTarget.insert(std::make_pair(iter->first, iter->second));
	}
}

void ImageProcess::setMaskForTargetBlock(const std::vector<cv::Mat> &new_maskForTargetBlock) {
	maskForTargetBlock.clear();
	for (auto mt : new_maskForTargetBlock)
		maskForTargetBlock.push_back(mt);
}

void ImageProcess::setMultiTempForTargetBlock(const std::map<int, std::list<cv::Mat>> &new_multiTempMat) {
	multiTempMat.clear();
	for (auto iter = new_multiTempMat.begin(); iter != new_multiTempMat.end(); ++iter) {
		int idx = iter->first;
		std::list<cv::Mat> tmpList;
		for (auto iterList = iter->second.begin(); iterList != iter->second.end(); ++iterList) {
			tmpList.push_back(*iterList);
		}
		multiTempMat.insert(std::make_pair(idx, tmpList));
	}
}

void ImageProcess::setCurModelName(std::string modelName) {
	curNewModelName = modelName;
}

void ImageProcess::matchTwoImage(cv::Mat &templateImage, cv::Mat &toMatchImage) {
	cv::Mat templateImageClone;
	cv::Mat toMatchImageClone;
	if (templateImageClone.channels() != 1 && toMatchImageClone.channels() != 1) {
		// 转换成灰度图
		cv::cvtColor(templateImage, templateImageClone, cv::COLOR_RGB2GRAY);
		cv::cvtColor(toMatchImage, toMatchImageClone, cv::COLOR_RGB2GRAY);
	}
	else {
		templateImageClone = templateImage.clone();
		toMatchImageClone = toMatchImage.clone();
	}
	std::vector<cv::KeyPoint> keyPoints_1, keyPoints_2;
	cv::Mat descriptors_1, descriptors_2;
	cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create();
	cv::Ptr<cv::DescriptorExtractor> descriptor = cv::ORB::create();
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");
	// 第一步检测Oriented角点位置
	detector->detect(templateImageClone, keyPoints_1);
	detector->detect(toMatchImageClone, keyPoints_2);
	// 第二步根据角点位置计算BRIEF描述子
	descriptor->compute(templateImageClone, keyPoints_1, descriptors_1);
	descriptor->compute(toMatchImageClone, keyPoints_2, descriptors_2);
	// 第三步对两幅图像中的BRIEF描述子进行匹配，使用Hamming距离
	std::vector<cv::DMatch> matches;
	matcher->match(descriptors_1, descriptors_2, matches);
	// 第四步匹配点对筛选
	double min_dist = 10000, max_dist = 0;
	for (int i = 0; i < descriptors_1.rows; i++) {
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	// 当描述子之间的距离大于两倍最小距离时，即认为匹配有误，但有时距离会非常小，设置经验值30作为下线
	std::vector < cv::DMatch > good_matches;
	for (int i = 0; i < descriptors_1.rows; i++) {
		if (matches[i].distance <= std::max(2 * min_dist, 30.0)) {
			good_matches.push_back(matches[i]);
		}
	}
	//// 第五步绘制匹配结果
	//cv::Mat img_goodmatch;
	//cv::drawMatches(templateImageClone, keyPoints_1, toMatchImageClone, keyPoints_2, good_matches, img_goodmatch);
	// 图像配准
	std::vector<cv::Point2f> imagePoints1, imagePoints2;
	for (int i = 0; i < good_matches.size(); i++) {
		imagePoints1.push_back(keyPoints_1[good_matches[i].queryIdx].pt);
		imagePoints2.push_back(keyPoints_2[good_matches[i].trainIdx].pt);
	}
	cv::Mat homo = cv::findHomography(imagePoints2, imagePoints1, CV_RANSAC);
	CalcCorners(homo, toMatchImageClone);
	cv::Mat imageTransform;
	cv::warpPerspective(toMatchImage, imageTransform, homo, cv::Size(corners.right_bottom.x, corners.right_bottom.y));
	imageTransform.copyTo(toMatchImage);
}

double ImageProcess::CalcDegreeBlackSence(const cv::Mat &srcImage, cv::Mat &dst)
{
	cv::Mat midImage, dstImage;

	Canny(srcImage, midImage, 50, 200, 3);
	cv::cvtColor(midImage, dstImage, CV_GRAY2BGR);

	//通过霍夫变换检测直线
	std::vector<cv::Vec2f> lines;
	HoughLines(midImage, lines, 1, CV_PI / 180, 300, 0, 0);//第5个参数就是阈值，阈值越大，检测精度越高
														   //cout << lines.size() << endl;

														   //由于图像不同，阈值不好设定，因为阈值设定过高导致无法检测直线，阈值过低直线太多，速度很慢
														   //所以根据阈值由大到小设置了三个阈值，如果经过大量试验后，可以固定一个适合的阈值。

	if (!lines.size())
	{
		HoughLines(midImage, lines, 1, CV_PI / 180, 200, 0, 0);
	}
	//cout << lines.size() << endl;

	if (!lines.size())
	{
		HoughLines(midImage, lines, 1, CV_PI / 180, 50, 0, 0);
	}
	//std::cout << lines.size() << std::endl;
	if (!lines.size())
	{
		std::cout << "没有检测到直线！" << std::endl;
		//return ERROR;
	}

	float sum = 0;
	//依次画出每条线段
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		cv::Point pt1, pt2;
		//cout << theta << endl;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		//只选角度最小的作为旋转角度
		sum += theta;

		cv::line(dstImage, pt1, pt2, cv::Scalar(55, 100, 195), 1); //Scalar函数用于调节线段颜色

																   // imshow("直线探测效果图", dstImage);
	}
	float average = sum / lines.size(); //对所有角度求平均，这样做旋转效果会更好

										//std::cout << "average theta:" << average << std::endl;

	double angle = DegreeTrans(average) - 90;

	rotateImageBlackSence(dstImage, dst, angle);
	//imshow("直线探测效果图2", dstImage);
	return angle;
}

void ImageProcess::CalcCorners(const cv::Mat& H, const cv::Mat& src)
{
	double v2[] = { 0, 0, 1 };//左上角
	double v1[3];//变换后的坐标值
	cv::Mat V2 = cv::Mat(3, 1, CV_64FC1, v2);  //列向量
	cv::Mat V1 = cv::Mat(3, 1, CV_64FC1, v1);  //列向量

	V1 = H * V2;
	//左上角(0,0,1)
	//std::cout << "V2: " << V2 << std::endl;
	//std::cout << "V1: " << V1 << std::endl;
	corners.left_top.x = v1[0] / v1[2];
	corners.left_top.y = v1[1] / v1[2];

	//左下角(0,src.rows,1)
	v2[0] = 0;
	v2[1] = src.rows;
	v2[2] = 1;
	V2 = cv::Mat(3, 1, CV_64FC1, v2);  //列向量
	V1 = cv::Mat(3, 1, CV_64FC1, v1);  //列向量
	V1 = H * V2;
	corners.left_bottom.x = v1[0] / v1[2];
	corners.left_bottom.y = v1[1] / v1[2];

	//右上角(src.cols,0,1)
	v2[0] = src.cols;
	v2[1] = 0;
	v2[2] = 1;
	V2 = cv::Mat(3, 1, CV_64FC1, v2);  //列向量
	V1 = cv::Mat(3, 1, CV_64FC1, v1);  //列向量
	V1 = H * V2;
	corners.right_top.x = v1[0] / v1[2];
	corners.right_top.y = v1[1] / v1[2];

	//右下角(src.cols,src.rows,1)
	v2[0] = src.cols;
	v2[1] = src.rows;
	v2[2] = 1;
	V2 = cv::Mat(3, 1, CV_64FC1, v2);  //列向量
	V1 = cv::Mat(3, 1, CV_64FC1, v1);  //列向量
	V1 = H * V2;
	corners.right_bottom.x = v1[0] / v1[2];
	corners.right_bottom.y = v1[1] / v1[2];

}

cv::Mat ImageProcess::imgTranslate(cv::Mat &matSrc, int xOffset, int yOffset, bool bScale)
{
	// 判断是否改变图像大小,并设定被复制ROI
	int nRows = matSrc.rows;
	int nCols = matSrc.cols;
	int nRowsRet = 0;
	int nColsRet = 0;
	cv::Rect rectSrc;
	cv::Rect rectRet;
	if (bScale)
	{
		nRowsRet = nRows + abs(yOffset);
		nColsRet = nCols + abs(xOffset);
		rectSrc.x = 0;
		rectSrc.y = 0;
		rectSrc.width = nCols;
		rectSrc.height = nRows;
	}
	else
	{
		nRowsRet = matSrc.rows;
		nColsRet = matSrc.cols;
		if (xOffset >= 0)
		{
			rectSrc.x = 0;
		}
		else
		{
			rectSrc.x = abs(xOffset);
		}
		if (yOffset >= 0)
		{
			rectSrc.y = 0;
		}
		else
		{
			rectSrc.y = abs(yOffset);
		}
		rectSrc.width = nCols - abs(xOffset);
		rectSrc.height = nRows - abs(yOffset);
	}
	// 修正输出的ROI
	if (xOffset >= 0)
	{
		rectRet.x = xOffset;
	}
	else
	{
		rectRet.x = 0;
	}
	if (yOffset >= 0)
	{
		rectRet.y = yOffset;
	}
	else
	{
		rectRet.y = 0;
	}
	rectRet.width = rectSrc.width;
	rectRet.height = rectSrc.height;
	// 复制图像
	cv::Mat matRet(nRowsRet, nColsRet, matSrc.type(), cv::Scalar(0));
	matSrc(rectSrc).copyTo(matRet(rectRet));
	return matRet;
}

ImageProcess::~ImageProcess()
{
}