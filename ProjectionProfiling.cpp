#include "ProjectionProfiling.h"

void ProjectionProfiling::ApplyTransform(const cv::Mat& points, float& theta, float& confidence, int binSize)
{
	cv::Mat rotatedImageTemp = cv::Mat::zeros(points.size(), CV_8UC1);
	float varMax = 0;
	int varSum = 0;
	for (float angle = MIN_ANGLE; angle <= MAX_ANGLE; angle += STEP)
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
		confidence = varMax / varSum;
	}/*
	for (int i = 0; i <= binSize; i++)
	{
		float angle = i - 5;
		rotate_bound(points, rotatedImageTemp, angle,Scalar(0));

		vector<int> sumRows = ComputeRowSums(rotatedImageTemp);
		vector<int> b(sumRows.size());

		for (size_t i = 0; i < sumRows.size(); i++)
		{
			b[i] = sumRows[i] - sumRows[i] % binSize;
		}

		float medb = ComputeMeanValue(b);
		float varb = ComputeVariance(b, medb);

		float med = ComputeMeanValue(sumRows);
		float var = ComputeVariance(sumRows, med);

		float confidenceTemp = varb / var;

		if (confidenceTemp > confidence)
		{
			confidence = confidenceTemp;
			theta = angle;
		}
	}*/
}
