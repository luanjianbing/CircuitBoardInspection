#pragma execution_character_set("utf-8")
#include "ProcessDisplay.h"

ProcessDisplay::ProcessDisplay( QWidget *parent)
{
	ui.setupUi(this);
	setFixedSize(this->width(), this->height());

	m_ImageProcessTest = new ImageProcess();

	// buttonModify
	connect(ui.buttonModify, SIGNAL(released()), this, SLOT(onButtonModify()));
	// buttonSure
	connect(ui.buttonSure, SIGNAL(released()), this, SLOT(onButtonSure()));
}


ProcessDisplay::~ProcessDisplay() {

	delete m_ImageProcessTest;
}

void ProcessDisplay::setCurModelName(std::string modelName) {
	m_ImageProcessTest->setCurModelName(modelName);
}

void ProcessDisplay::onButtonModify() {
	ui.lineEditX1->setEnabled(true);
	ui.lineEditY1->setEnabled(true);
	ui.lineEditX2->setEnabled(true);
	ui.lineEditY2->setEnabled(true);

	if (!isOnModify)
		isOnModify = true;
}

void ProcessDisplay::onButtonSure() {
	if (isOnModify) {
		ui.lineEditX1->setDisabled(true);
		ui.lineEditY1->setDisabled(true);
		ui.lineEditX2->setDisabled(true);
		ui.lineEditY2->setDisabled(true);

		int curBlockIdx = ui.labelBlockIndex->text().toInt();
		// 更新坐标
		int afterModifyX1 = ui.lineEditX1->text().toInt();
		int afterModifyY1 = ui.lineEditY1->text().toInt();
		int afterModifyX2 = ui.lineEditX2->text().toInt();
		int afterModifyY2 = ui.lineEditY2->text().toInt();

		if ((0 < afterModifyX1 && afterModifyX1 < matTemplateSizeX)
			&& (0 < afterModifyY1 && afterModifyY1 < matTemplateSizeY)
			&& (0 < afterModifyX2 && afterModifyX2 < matTemplateSizeX)
			&& (0 < afterModifyY2 && afterModifyY2 < matTemplateSizeY)
			&& (afterModifyX1 < afterModifyX2)
			&& (afterModifyY1 < afterModifyY2)) {

			block[curBlockIdx][0] = afterModifyX1;
			block[curBlockIdx][1] = afterModifyY1;
			block[curBlockIdx][2] = afterModifyX2;
			block[curBlockIdx][3] = afterModifyY2;
			// 发送信号
			emit signalLandMarkChange(curBlockIdx, block[curBlockIdx][0], block[curBlockIdx][1], block[curBlockIdx][2], block[curBlockIdx][3]);

			if (curBlockIdx != 0)
				updateDisplayByBlockIdx(curBlockIdx);

		}
		else {
			QMessageBox::warning(NULL, "警告", "坐标修改异常！", QMessageBox::Yes);
		}

		isOnModify = false;
	}
}

void ProcessDisplay::setBlockMsg(std::vector<std::vector<int>> &landMarkRun) {
	block.clear();
	block.resize(landMarkRun.size());
	for (int i = 0; i < block.size(); ++i) {
		block[i].resize(landMarkRun[i].size());
		for (int j = 0;  j < block[i].size(); ++j) {
			block[i][j] = landMarkRun[i][j];
		}
	}

}

void ProcessDisplay::setMatMsg(cv::Mat norm) {

	matTemplate = norm.clone();

	matTemplateSizeX = matTemplate.cols;
	matTemplateSizeY = matTemplate.rows;
}

void ProcessDisplay::updateDisplayByBlockIdx(int blockIdx) {
	//ui.lineEditX1->setDisabled(true);
	//ui.lineEditY1->setDisabled(true);
	//ui.lineEditX2->setDisabled(true);
	//ui.lineEditY2->setDisabled(true);
	ui.lineEditX1->setText(QString::fromStdString(std::to_string(block[blockIdx][0])));
	ui.lineEditY1->setText(QString::fromStdString(std::to_string(block[blockIdx][1])));
	ui.lineEditX2->setText(QString::fromStdString(std::to_string(block[blockIdx][2])));
	ui.lineEditY2->setText(QString::fromStdString(std::to_string(block[blockIdx][3])));

	cv::Mat matPart;
	matTemplate.copyTo(matPart);
	matPart = matPart(cv::Range(block[blockIdx][1], block[blockIdx][3]), cv::Range(block[blockIdx][0], block[blockIdx][2]));
	//cv::imshow("matPart", matPart);
	//cv::waitKey(0);
	cv::cvtColor(matPart, matPart, cv::COLOR_BGR2RGB);
	QImage imgForDrawQImage = QImage(matPart.data, matPart.cols, matPart.rows, matPart.step, QImage::Format_RGB888);
	ui.labelDisplay->setPixmap(QPixmap::fromImage(imgForDrawQImage).scaled(ui.labelDisplay->width(), ui.labelDisplay->height(), Qt::KeepAspectRatio));

	//std::string tmp = "<html><head/><body><p><span style=\" font-size:24pt;\">";
	//tmp += std::to_string(blockIdx);
	//tmp += "</span></p></body></html>";
	//ui.labelBlockIndex->setText(QApplication::translate("ProcessResultDisplayClass", tmp.c_str(), nullptr));
	ui.labelBlockIndex->setText(QString::fromStdString(std::to_string(blockIdx)));
}

void ProcessDisplay::setParamMsg(std::vector<std::vector<int>> &vectParamForNewModel) {
	param.clear();
	param.resize(vectParamForNewModel.size());
	for (int i = 0; i < param.size(); ++i) {
		param[i].resize(vectParamForNewModel[i].size());
		for (int j = 0; j < param[i].size(); ++j) {
			param[i][j] = vectParamForNewModel[i][j];
		}
	}
}

// 根据每个block的类型，首先做一次相关的算法检测，并显示
void ProcessDisplay::updateAlgorithmDisplay(int cateIdx, int colorIdx, int detectIdx) {
	int curBlockIdx = ui.labelBlockIndex->text().toInt();
	cv::Mat matPart;
	matTemplate.copyTo(matPart);
	// cv::imshow("partRoi", matPart);
	matPart = matPart(cv::Range(block[curBlockIdx][1], block[curBlockIdx][3]), cv::Range(block[curBlockIdx][0], block[curBlockIdx][2]));
	
	// 插入可选颜色
	// detectIdx: 0		颜色
	// detectIdx: 1		长度
	// detectIdx: 2		位置
	// detectIdx: 3		有无
	// detectIdx: 4		类型
	// detectIdx: 5		方向


	switch (detectIdx)
	{
		case 0: {	// 

			break;
		}
		case 1: {	// 长度，只针对胶布
			if (cateIdx == 2) {
				bool isBlack = (param[curBlockIdx][3] == 2 ? true : false);	// 这里仅考虑了黑色，灰色后面考虑
				int pixelNorm = 0;
				int pixelOffset = 0;
				double angle;
				//m_ImageProcessTest->lengthDetectAndDisplay(matPart, isBlack, param[curBlockIdx][5], param[curBlockIdx][6], pixelNorm, pixelOffset);
				//emit signalLengthParamModify(curBlockIdx, pixelNorm, pixelOffset);
				//cv::waitKey(0);
				cv::Mat sample = m_ImageProcessTest->lengthDetectAndDisplay(matPart, isBlack, param[curBlockIdx][5], param[curBlockIdx][6], pixelNorm, pixelOffset, angle);
				// cv::cvtColor(sample, sample, cv::COLOR_BGR2RGB);
				QImage imgForDrawQImage = QImage(sample.data, sample.cols, sample.rows, sample.step, QImage::Format_RGB888);
				ui.labelDisplay->setPixmap(QPixmap::fromImage(imgForDrawQImage).scaled(ui.labelDisplay->width(), ui.labelDisplay->height(), Qt::KeepAspectRatio));
				emit signalLengthParamModify(curBlockIdx, pixelNorm, pixelOffset, angle);
			}
			break;
		}
		case 2: {	// 位置检测
			// 到这里已经得到了本次显示的局部图
			int pixelNorm = 0;
			int pixelOffset = 0;
			int startIdx = param[curBlockIdx][8];
			cv::Mat startMat;
			matTemplate.copyTo(startMat);
			//std::cout << "startIdx: " << startIdx << std::endl;
			startMat = startMat(cv::Range(block[startIdx][1], block[startIdx][3]), cv::Range(block[startIdx][0], block[startIdx][2]));
			//cv::imshow("startMat", startMat);
			//cv::imshow("matPart", matPart);
			cv::Mat posCombine = m_ImageProcessTest->posDetectAndDisplay(startMat, param[startIdx][1], param[startIdx][3], cv::Point(block[startIdx][0], block[startIdx][1]),
				matPart, param[curBlockIdx][1], param[curBlockIdx][3], cv::Point(block[curBlockIdx][0], block[curBlockIdx][1]), param[curBlockIdx][9], param[curBlockIdx][10], pixelNorm, pixelOffset);
			cv::cvtColor(posCombine, posCombine, cv::COLOR_BGR2RGB);
			QImage imgForDrawQImage = QImage(posCombine.data, posCombine.cols, posCombine.rows, posCombine.step, QImage::Format_RGB888);
			ui.labelDisplay->setPixmap(QPixmap::fromImage(imgForDrawQImage).scaled(ui.labelDisplay->width(), ui.labelDisplay->height(), Qt::KeepAspectRatio));
			emit signalTwoTargetDistance(curBlockIdx, pixelNorm, pixelOffset);
			break;
		}
		case 3: {
			break;
		}
		case 4: {
			break;
		}
		case 5: {
			break;
		}
		default: {	// 只检测显示缩小目标物区域
			cv::Rect targetRect = m_ImageProcessTest->findTrueArea(matPart, cateIdx, colorIdx, curBlockIdx);
			cv::rectangle(matPart, targetRect, cv::Scalar(0, 255, 0), 2);
			cv::cvtColor(matPart, matPart, cv::COLOR_BGR2RGB);
			QImage imgForDrawQImage = QImage(matPart.data, matPart.cols, matPart.rows, matPart.step, QImage::Format_RGB888);
			ui.labelDisplay->setPixmap(QPixmap::fromImage(imgForDrawQImage).scaled(ui.labelDisplay->width(), ui.labelDisplay->height(), Qt::KeepAspectRatio));

			cv::Point leftUp, rightBotton, cen;
			// leftUp = cv::Point(block[curBlockIdx][0] + targetRect.x, block[curBlockIdx][1] + targetRect.y);	// 这里保存的是实际的坐标信息
			leftUp = cv::Point(targetRect.x, targetRect.y);	// 这里保存的是相对局部的坐标信息
			rightBotton = cv::Point(leftUp.x + targetRect.width, leftUp.y + targetRect.height);
			cen = cv::Point(leftUp.x + targetRect.width / 2, leftUp.y + targetRect.height / 2);

			emit signalTrueTargetPartAxis(curBlockIdx, leftUp.x, leftUp.y, rightBotton.x, rightBotton.y, cen.x, cen.y);
			break;
		}
	}

	//switch (cateIdx)
	//{
	//case 0: {	// 花结束带

	//	break;
	//}
	//case 1: {	// T18R

	//	break;
	//}
	//case 2: {	// 胶布
	//	switch (detectIdx)
	//	{
	//		case 1: {
	//			bool isBlack = (param[curBlockIdx][3] == 2 ? true : false);	// 这里仅考虑了黑色，灰色后面考虑
	//			int pixelNorm = 0;
	//			int pixelOffset = 0;
	//			double angle;
	//			//m_ImageProcessTest->lengthDetectAndDisplay(matPart, isBlack, param[curBlockIdx][5], param[curBlockIdx][6], pixelNorm, pixelOffset);
	//			//emit signalLengthParamModify(curBlockIdx, pixelNorm, pixelOffset);
	//			//cv::waitKey(0);
	//			cv::Mat sample = m_ImageProcessTest->lengthDetectAndDisplay(matPart, isBlack, param[curBlockIdx][5], param[curBlockIdx][6], pixelNorm, pixelOffset, angle);
	//			QImage imgForDrawQImage = QImage(sample.data, sample.cols, sample.rows, sample.step, QImage::Format_RGB888);
	//			ui.labelDisplay->setPixmap(QPixmap::fromImage(imgForDrawQImage).scaled(ui.labelDisplay->width(), ui.labelDisplay->height(), Qt::KeepAspectRatio));
	//			emit signalLengthParamModify(curBlockIdx, pixelNorm, pixelOffset, angle);
	//			break;
	//		}
	//		default:
	//			break;
	//	}
	//	break;
	//}
	//case 3: {	// 插头

	//	break;
	//}
	//default:
	//	break;
	//}
}

void ProcessDisplay::setHSVColorForGetTrueTarget(std::unordered_map<std::string, HSVLimit> &HSVColorForGetTrueTarget) {
	m_ImageProcessTest->setHSVColorForGetTrueTarget(HSVColorForGetTrueTarget);
}