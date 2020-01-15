#include "FrequencyHough.h"

void FrequencyHough::Compute(const cv::Mat& src, float& angle, float& confidence)
{
	cv::Mat freqSrc = FourierTransform::ApplyTransform(src);

	cv::Mat out = Hough::ApplyTransform(freqSrc,angle,0,true);

	confidence = 0;
}

