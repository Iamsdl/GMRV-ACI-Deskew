#pragma once
#include "pch.h"
#include "Utils.h"
#include "FourierTransform.h"
#include "ProjectionProfiling.h"
#include "Hough.h"

class IAlgorithm
{
public:

	virtual void Compute(const cv::Mat& src, float& angle, float& confidence) {};
	static cv::Mat Preprocess(const cv::Mat& src)
	{
		cv::Mat threshold_output;
		cv::threshold(src, threshold_output, 150, 255, cv::THRESH_BINARY_INV);

		//Get image with points from characters
		cv::Mat points = cv::Mat::zeros(src.size(), CV_8UC1);
		GeneratePoints(threshold_output, points);
		return points;
	}

public:
	static ProjectionProfiling projectionProfilingCore;
	virtual const char* GetTypeAsString() const { return "IAlgorithm"; }
	static Hough houghCore;
};
