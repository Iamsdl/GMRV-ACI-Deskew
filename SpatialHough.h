#pragma once
#include "Hough.h"
#include "IAlgorithm.h"
class SpatialHough : public IAlgorithm
{
public:
	void Compute(const cv::Mat& src, float& angle, float& confidence) override;
};