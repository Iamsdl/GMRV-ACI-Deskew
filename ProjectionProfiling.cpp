#include "ProjectionProfiling.h"

void ProjectionProfiling::ApplyTransform(const cv::Mat& points, float& theta, float& confidence, int binSize)
{
	cv::Mat rotatedImageTemp = cv::Mat::zeros(points.size(), CV_8UC1);
	float varMax = 0;
	float varSum = 0;
	for (float angle = MIN_ANGLE; angle <= MAX_ANGLE; angle += STEP_ANGLE)
	{
		RotateImage(points, rotatedImageTemp, angle, cv::Scalar(0));
		std::vector<int> sumRows;
		if (binSize > 0)
			sumRows = ComputeVHistogram(rotatedImageTemp, binSize);
		else
			sumRows = ComputeVHistogram(rotatedImageTemp);

		float medb = ComputeMeanValue(sumRows);
		float varb = ComputeVariance(sumRows, medb);

		varSum += varb;

		if (varb > varMax)
		{
			varMax = varb;
			theta = -angle;
		}
	}
	confidence = 1 - (abs(0.5f - varMax / varSum) * PROJ_CONFIDENCE_MOD);
}
