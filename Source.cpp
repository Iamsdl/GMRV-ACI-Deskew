#include <vector>
#include <fstream>
#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ctime>
#include <iostream>

using namespace std;
using namespace cv;

float ComputeMeanValue(vector<int> sumRows)
{
	float med = 0;
	for (size_t j = 0; j < sumRows.size(); j++)
	{
		med += sumRows[j];
	}
	med /= sumRows.size();
	return med;
}

float ComputeVariance(vector<int> sumRows, float med)
{
	float var = 0;
	for (size_t j = 0; j < sumRows.size(); j++)
	{
		var += pow(sumRows[j] - med, 2);
	}
	var /= sumRows.size();
	return var;
}

vector<int> ComputeRowSums(Mat src)
{
	vector<int> sumRows(src.size().height);

	for (size_t j = 0; j < src.size().height; j++)
	{
		for (size_t k = 0; k < src.size().width; k++)
		{
			sumRows[j] += src.at<unsigned char>(j, k) / 255;
		}
	}

	return sumRows;
}

void getPoints(Mat& src_inverted, Mat& drawing)
{
	/// Find contours
	vector<vector<Point> > contours;
	findContours(src_inverted, contours, RETR_LIST, CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	//vector<Point2f>center(contours.size());
	//vector<float>radius(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
		//minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
	}


	/// Draw polygonal contour + bonding rects + circles
	for (int i = 0; i < boundRect.size(); i++)
	{
		//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		//drawContours(points, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		Point2f point = Point2f((boundRect[i].x * 2 + boundRect[i].width) / 2, boundRect[i].y + boundRect[i].height);
		rectangle(drawing, point, point, Scalar(255), 5, 8, 0);
		//circle(points, center[i], (int)radius[i], color, 2, 8, 0);
	}
	/// Show in a window
	//namedWindow("Contours", WINDOW_FULLSCREEN);
	//imshow("Contours", drawing);
}

void DrawHoughLines(Mat& points, Mat& dst, Mat& cdst, Mat& cdstP)
{
	// Edge detection
	//Canny(points, dst, 50, 200, 3);
	// Copy edges to the images that will display the results in BGR
	//cvtColor(points, points, COLOR_RGB2GRAY);
	cvtColor(points, cdst, COLOR_GRAY2BGR);
	cdstP = cdst.clone();
	// Standard Hough Line Transform
	vector<Vec2f> lines; // will hold the results of the detection
	HoughLines(points, lines, 1, CV_PI / 180, 200, 0, 0, CV_PI / 2 - 5 * CV_PI / 180, CV_PI / 2 + 5 * CV_PI / 180); // runs the actual detection
													   // Draw the lines
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 2000 * (-b));
		pt1.y = cvRound(y0 + 2000 * (a));
		pt2.x = cvRound(x0 - 2000 * (-b));
		pt2.y = cvRound(y0 - 2000 * (a));
		line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);
	}
	// Probabilistic Line Transform
	vector<Vec4i> linesP; // will hold the results of the detection
	HoughLinesP(points, linesP, 1, CV_PI / 180, 50, 50, 10); // runs the actual detection
														  // Draw the lines
	for (size_t i = 0; i < linesP.size(); i++)
	{
		Vec4i l = linesP[i];
		line(cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
	}
	// Show results
	//imshow("Source", points);
	//imshow("Detected Lines (in red) - Standard Hough Line Transform", cdst);
	////imshow("Detected Lines (in red) - Probabilistic Line Transform", cdstP);
}

void HoughGetTheta(Mat points, float& theta, float& confidence)
{
	// Edge detection
	//Canny(points, dst, 50, 200, 3);
	//Mat points2;
	//cvtColor(points, points, COLOR_RGB2GRAY);

	// Standard Hough Line Transform
	vector<Vec3f> lines; // will hold the results of the detection
	float thetaStep = 1;
	float toRad = CV_PI / 180;
	HoughLines(points, lines, 1, thetaStep * toRad, 0, 0, 0, CV_PI / 2 - 5 * toRad, CV_PI / 2 + (5 + thetaStep) * toRad); // runs the actual detection

	theta = 0;
	confidence = 0;

	vector<int> hist(10 / thetaStep + 1);

	float numberOfThetas = 0;

	for (size_t i = 0; i < lines.size(); i++)
	{
		lines[i][1] = lines[i][1] * 180 / CV_PI - 90;
		if (lines[i][2] > lines[0][2] / 4)
		{
			numberOfThetas += lines[i][2];
			hist[round((5 + lines[i][1]) / thetaStep)] += lines[i][2];
		}
	}

	theta = lines[0][1];
	for (size_t i = 0; i < lines.size(); i++)
	{
		if (lines[i][2] > lines[0][2] / 4)
		{
			confidence += pow(lines[i][1] - theta, 2);
		}
	}
	confidence /= numberOfThetas;
	//confidence = 1.0f - (confidence / 25.0f);
	confidence = 1.0f - confidence;
}
void rotate_bound(Mat& image, Mat& out_image, float angle)
{
	//grab the dimensions of the imageand then determine the
	//center
	int h = image.size().height;
	int w = image.size().width;
	Point2f center = Point2f(w / 2.0f, h / 2.0f);

	//grab the rotation matrix(applying the negative of the
	//angle to rotate clockwise), then grab the sine and cosine
	//(i.e., the rotation components of the matrix)
	Mat M = getRotationMatrix2D(center, angle, 1.0);
	double cos = abs(M.at<double>(0, 0));
	double sin = abs(M.at<double>(0, 1));
	//compute the new bounding dimensions of the image
	int nW = int((h * sin) + (w * cos));
	int nH = int((h * cos) + (w * sin));

	//adjust the rotation matrix to take into account translation
	M.at<double>(0, 2) += (nW / 2) - center.x;
	M.at<double>(1, 2) += (nH / 2) - center.y;

	//perform the actual rotation and return the image
	return warpAffine(image, out_image, M, Size(nW, nH));
}
void ProjGetTheta(cv::Mat& points, float& theta, float& confidence, int binSize)
{
	Mat rotatedImageTemp = Mat::zeros(points.size(), CV_8UC1);
	confidence = 0;

	for (int i = 0; i <= 10; i++)
	{
		float angle = i - 5;
		rotate_bound(points, rotatedImageTemp, angle);

		//vector<float> sumColumns(rotatedImageTemp.size().width);
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
	}
}

int main()
{
	std::clock_t start;
	double duration;

	start = std::clock();

	int nrOfImages = 3;
	int lowestAngle = -5;
	int highestAngle = 5;

	ofstream out("results.csv");
	out << "imageNr,imageAngle,thetaHough,confidenceHough,thetaProj,confidenceProj,\n";

	for (int imageNr = 1; imageNr <= nrOfImages; imageNr++)
	{
		for (int imageAngle = lowestAngle * 10; imageAngle <= highestAngle * 10; imageAngle++)
		{
			string x = to_string(imageAngle / 10) + ((imageAngle % 10 != 0) ? "." + to_string(abs(imageAngle % 10)) : "");

			// Declare the output variables
			string filename = to_string(imageNr) + "-theta=" + x + ".jpg";
			// Loads an image
			Mat src = imread("rotite/" + filename, IMREAD_GRAYSCALE);

			// Check if image is loaded fine
			if (src.empty()) {
				printf(" Error opening image\n");
				printf(" Program Arguments: [image_name -- default %s] \n", filename);
				return -1;
			}

			// Convert image to gray and blur it
			Mat src_gray;
			src.copyTo(src_gray); //already grayscale
			blur(src_gray, src_gray, Size(3, 3));

			// Threshhold image
			Mat threshold_output;
			threshold(src_gray, threshold_output, 150, 255, THRESH_BINARY);

			//Invert image (white text on black background)
			Mat thresold_output_inverted;
			cv::subtract(Scalar::all(255), threshold_output, thresold_output_inverted);

			//Get image with points from characters
			Mat points = Mat::zeros(src.size(), CV_8UC1);
			getPoints(threshold_output, points);


			//Mat dst, cdst, cdstP;
			//DrawHoughLines(points, dst, cdst, cdstP);

			float thetaHough = 0, confidenceHough = 0;
			HoughGetTheta(points, thetaHough, confidenceHough);

			/*Mat rotatedImage = Mat::zeros(src.size(), CV_8UC3);
			rotate_bound(src, rotatedImage, thetaHough * 180 / CV_PI - 90);*/

			float thetaProj = 0, confidenceProj = 0;
			ProjGetTheta(points, thetaProj, confidenceProj, 10);

			/*Mat src3 = Mat::zeros(src.size(), CV_8UC3);
			rotate_bound(src, src3, thetaProj);*/

			out << to_string(imageNr) << ","
				<< to_string(-imageAngle / 10.0f) << ","
				<< to_string(thetaHough) << ","
				<< to_string(confidenceHough) << ","
				<< to_string(thetaProj) << ","
				<< to_string(confidenceProj) << ",\n";

		}
	}
	out.close();
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	cout << "Finished in " << duration << " seconds";
	cv::waitKey();
	return 0;
}