#pragma once
#include "Constants.h"
#include "pch.h"

class Result
{
public:
	Result() { angle = 0; confidence = 0; };
	Result(float angle, float confidence) { this->angle = angle; this->confidence = confidence; }
public:
	float angle;
	float confidence;
};

typedef std::unordered_map<std::string, float> MapVotingAngles;
typedef std::unordered_map<std::string, Result> MapNameResult;

float ComputeVariance(std::vector<int> sumRows, float med);
float ComputeMeanValue(std::vector<int> sumRows);

std::vector<int> ComputeVHistogram(cv::Mat src, int binSize);
std::vector<int> ComputeVHistogram(cv::Mat src);

void GeneratePoints(cv::Mat& src_inverted, cv::Mat& drawing);

void RotateImage(const cv::Mat& image, cv::Mat& out_image, float angle, cv::Scalar emptyColor);

cv::Mat CreateSquareImage(const cv::Mat& input, const cv::Scalar& bgcolor);


int ComputeValue(int x, int r1, int s1, int r2, int s2);

void ContrastStretch(cv::Mat& src, cv::Mat dst, int r1, int s1, int r2, int s2);

void NormalizeAngle(float& angle);
