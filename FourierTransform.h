#pragma once
#include "Utils.h"
class FourierTransform
{
private:
    static cv::Mat ShiftImage(const cv::Mat& inputImg);
public:
	static cv::Mat ApplyTransform(const cv::Mat& src);
};

