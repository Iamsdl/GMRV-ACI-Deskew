#include "FrequencyHough.h"

int ComputeValue(int x, int r1, int s1, int r2, int s2)
{
    float result;
    if (0 <= x && x <= r1) {
        result = s1 / r1 * x;
    }
    else if (r1 < x && x <= r2) {
        result = ((s2 - s1) / (r2 - r1)) * (x - r1) + s1;
    }
    else if (r2 < x && x <= 255) {
        result = ((255 - s2) / (255 - r2)) * (x - r2) + s2;
    }
    return (int)result;
}

void ContrastStrach(cv::Mat& src, cv::Mat dst, int r1, int s1, int r2, int s2)
{
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            for (int c = 0; c < 3; c++) {
                int output = ComputeValue(src.at<uchar>(y, x), r1, s1, r2, s2);
                dst.at<uchar>(y, x) = cv::saturate_cast<uchar>(output);
            }
        }
    }
}

void FrequencyHough::Compute(const cv::Mat& src, float& angle, float& confidence)
{
	cv::Mat freqSrc = FourierTransform::ApplyTransform(src);

    ContrastStrach(freqSrc,freqSrc,70,0,140,255);
	cv::equalizeHist(freqSrc, freqSrc);
	cv::threshold(freqSrc, freqSrc, 0, 255, cv::THRESH_OTSU);

	cv::Mat out = Hough::ApplyTransform(freqSrc, angle, 0, true);

	confidence = 0;
}

