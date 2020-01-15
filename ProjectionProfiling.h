#pragma once
#include "pch.h"
#include "Utils.h"
class ProjectionProfiling
{
public:
	ProjectionProfiling() {};
	static void ApplyTransform(const cv::Mat& points, float& theta, float& confidence, int binSize);
};

