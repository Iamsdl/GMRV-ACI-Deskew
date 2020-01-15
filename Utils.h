#pragma once
#include "Constants.h"
#include "pch.h"

float ComputeVariance(std::vector<int> sumRows, float med);
float ComputeMeanValue(std::vector<int> sumRows);

std::vector<int> ComputeVHistogram(cv::Mat src, int binSize);
std::vector<int> ComputeVHistogram(cv::Mat src);

void GeneratePoints(cv::Mat& src_inverted, cv::Mat& drawing);

void RotateImage(const cv::Mat& image, cv::Mat& out_image, float angle, cv::Scalar emptyColor);

cv::Mat CreateSquareImage(const cv::Mat& input, const cv::Scalar& bgcolor);


int ComputeValue(int x, int r1, int s1, int r2, int s2);

void ContrastStretch(cv::Mat& src, cv::Mat dst, int r1, int s1, int r2, int s2);