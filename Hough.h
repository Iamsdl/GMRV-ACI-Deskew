#pragma once
#include "Utils.h"

class Hough
{
public:
    static cv::Mat ApplyTransform(const cv::Mat& points, float& angle, float& confidence, int threshold,bool draw);
	static void HoughGetTheta(cv::Mat points, float& theta, float& confidence);
};

