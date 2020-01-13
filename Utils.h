#pragma once
#include "pch.h"
float ComputeMeanValue(std::vector<int> sumRows);
std::vector<int> ComputeRowSums(cv::Mat src);

void getPoints(cv::Mat& src_inverted, cv::Mat& drawing);

void rotate_bound(cv::Mat& image, cv::Mat& out_image, float angle);
