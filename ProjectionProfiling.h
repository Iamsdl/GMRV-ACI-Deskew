#pragma once
#include "Algorithm.h"
class ProjectionProfiling :
	public Algorithm
{
public:
	void Compute(const cv::Mat& src, cv::Mat& dst) override;
};

