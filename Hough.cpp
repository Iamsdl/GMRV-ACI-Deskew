#include "Hough.h"

cv::Mat Hough::ApplyTransform(const cv::Mat& points, float& angle, float& confidence, int threshold, bool draw)
{
	cv::Mat dst;
	cv::cvtColor(points, dst, cv::COLOR_GRAY2BGR);
	// Standard Hough Line Transform
	std::vector<cv::Vec3f> lines; // will hold the results of the detection
	HoughLines(points, lines, 1, STEP_ANGLE * CV_PI / 180, threshold, 0, 0, (90 + MIN_ANGLE - STEP_ANGLE) * CV_PI / 180, (90 + MAX_ANGLE + STEP_ANGLE) * CV_PI / 180); // runs the actual detection

	float drawAngle = lines[0][1];

	for (size_t i = 0; i < lines.size(); i++)
	{
		lines[i][1] = (CV_PI / 2 - lines[i][1]) * 180 / CV_PI;
		NormalizeAngle(lines[i][1]);
	}

	int correctVotes = 0;
	int allVotes = 0;
	for (size_t i = 0; i < lines.size(); i++)
	{
		if (lines[i][2] > lines[0][2] / HOUGH_CONFIDENCE_MOD)
		{
			if (abs(lines[i][1] - lines[0][1]) < 0.001f)
				correctVotes += lines[i][2];
			allVotes += lines[i][2];
		}
	}
	confidence = (float)correctVotes / allVotes;
	angle = lines[0][1];

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