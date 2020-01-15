#include "SpatialHough.h"

void SpatialHough::Compute(const cv::Mat& src, float& angle, float& confidence)
{
	Hough::ApplyTransform(src, angle, confidence, 10, false);
}