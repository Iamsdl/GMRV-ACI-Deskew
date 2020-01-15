#pragma once
#include "IAlgorithm.h"
#include "ProjectionProfiling.h"
class SpatialProjectionProfiling : public IAlgorithm
{
public:
	SpatialProjectionProfiling() { binSize = 0; };
	void SetBinSize(int binSize) { this->binSize = binSize; }
	void Compute(const cv::Mat& src,float& angle, float& confidence) override;

public:
	int binSize;
};

