#include "SpatialProjectionProfiling.h"


void SpatialProjectionProfiling::Compute(const cv::Mat& src, float& angle, float& confidence)
{
	ProjectionProfiling::ApplyTransform(src, angle, confidence, 0);
}
