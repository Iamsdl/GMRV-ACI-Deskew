#pragma once
#include "Constants.h"
#include "pch.h"

float ComputeVariance(std::vector<int> sumRows, float med);
float ComputeMeanValue(std::vector<int> sumRows);

std::vector<int> ComputeVHistogram(cv::Mat src, int binSize);
std::vector<int> ComputeVHistogram(cv::Mat src);

void GeneratePoints(cv::Mat& src_inverted, cv::Mat& drawing);

void RotateImage(const cv::Mat& image, cv::Mat& out_image, float angle, cv::Scalar emptyColor);