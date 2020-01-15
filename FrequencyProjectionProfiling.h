#pragma once
#include "IAlgorithm.h"
class FrequencyProjectionProfiling : public IAlgorithm
{
public:
	FrequencyProjectionProfiling() { binSize = 0; };
	void SetBinSize(int binSize) { this->binSize = binSize; }
	void Compute(const cv::Mat& src, float& angle, float& confidence) override;

public:
	int binSize;
};

