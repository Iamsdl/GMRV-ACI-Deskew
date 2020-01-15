#include "Hough.h"

cv::Mat Hough::ApplyTransform(const cv::Mat& points, float& angle, int threshold, bool draw)
{
	cv::Mat dst;
	cv::cvtColor(points, dst, cv::COLOR_GRAY2BGR);
	// Standard Hough Line Transform
	std::vector<cv::Vec3f> lines; // will hold the results of the detection
	HoughLines(points, lines, 1, STEP * CV_PI / 180, threshold, 0, 0); // runs the actual detection

	angle = (CV_PI / 2 - lines[0][1]) * 180 / CV_PI;
	float drawAngle = lines[0][1];

	int correctVotes = 0;
	int allVotes = 0;
	for (size_t i = 0; i < lines.size(); i++)
	{
		lines[i][1] = (CV_PI / 2 - lines[i][1]) * 180 / CV_PI;
		if (lines[i][1] == lines[0][1])
			correctVotes += lines[i][2];
		allVotes += lines[i][2];
	}

	if (!draw)
		return dst;

	// Draw the lines
	{
		float rho = lines[0][0], theta = drawAngle;
		cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 2000 * (-b));
		pt1.y = cvRound(y0 + 2000 * (a));
		pt2.x = cvRound(x0 - 2000 * (-b));
		pt2.y = cvRound(y0 - 2000 * (a));
		line(dst, pt1, pt2, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
	}
	return dst;
}
void Hough::HoughGetTheta(cv::Mat points, float& theta, float& confidence)
{
	// Standard Hough Line Transform
	std::vector<cv::Vec3f> lines; // will hold the results of the detection
	float thetaStep = 1;
	float toRad = CV_PI / 180;
	HoughLines(points, lines, 1, thetaStep * toRad, 0, 0, 0); // runs the actual detection

	theta = 0;
	confidence = 0;

	std::vector<int> hist(360 / thetaStep + 1);

	float numberOfThetas = 0;

	for (size_t i = 0; i < lines.size(); i++)
	{
		lines[i][1] = lines[i][1] * 180 / CV_PI - 90;
		if (lines[i][2] > lines[0][2] / 2)
		{
			numberOfThetas += lines[i][2];
			hist[round((90 + lines[i][1]) / thetaStep)] += lines[i][2];
		}
	}

	theta = lines[0][1];
	for (size_t i = 0; i < lines.size(); i++)
	{
		if (lines[i][2] > lines[0][2] / 2)
		{
			confidence += pow(lines[i][1] - theta, 2);
		}
	}
	confidence /= numberOfThetas;

	confidence = 1.0f - confidence;
}