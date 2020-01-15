#pragma once
#include "IAlgorithm.h"
class FrequencyHough : public IAlgorithm
{
public:
	void Compute(const cv::Mat& src, float& angle, float& confidence) override;
};

