#include "FrequencyHough.h"


void FrequencyHough::Compute(const cv::Mat& src, float& angle, float& confidence)
{
	cv::Mat squareSrc = CreateSquareImage(src, cv::Scalar(255));
	cv::Mat freqSrc = FourierTransform::ApplyTransform(squareSrc);

	ContrastStretch(freqSrc, freqSrc, 70, 0, 140, 255);
	cv::equalizeHist(freqSrc, freqSrc);
	cv::threshold(freqSrc, freqSrc, 0, 255, cv::THRESH_OTSU);

	Hough::ApplyTransform(freqSrc, angle, confidence, 0, 180,true);
}