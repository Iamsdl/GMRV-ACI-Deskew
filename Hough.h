#pragma once
#include "Utils.h"

class Hough
{
public:
    static cv::Mat ApplyTransform(const cv::Mat& points, float& angle, float& confidence, int threshold, int offset, bool draw);
};

