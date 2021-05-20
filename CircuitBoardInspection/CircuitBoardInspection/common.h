#pragma once

#include <iostream>
#include <vector>

#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp>

#include <unordered_map>
struct HSVLimit
{
	int hMin = 0;
	int hMax = 180;
	int sMin = 0;
	int sMax = 255;
	int vMin = 0;
	int vMax = 255;
	HSVLimit() {}
	HSVLimit(int _hMin, int _hMax, int _sMin, int _sMax, int _vMin, int _vMax)
		: hMin(_hMin), hMax(_hMax), sMin(_sMin), sMax(_sMax), vMin(_vMin), vMax(_vMax) {}
};