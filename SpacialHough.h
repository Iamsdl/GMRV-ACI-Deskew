#pragma once
#include "Algorithm.h"
class SpacialHough :
	public Algorithm
{
public:
	void Compute(const cv::Mat& src, cv::Mat& dst) override;
};

