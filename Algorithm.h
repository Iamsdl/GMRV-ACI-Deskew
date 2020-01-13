#pragma once
#include "pch.h"
class Algorithm
{
public:
	virtual void Compute(const cv::Mat& src, cv::Mat& dst);
};

