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
		Point2f point = Point2f(boundRect[i].x + boundRect[i].width / 2, boundRect[i].y + boundRect[i].height);
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
	vector<Vec3f> lines; // will hold the results of the detection
	HoughLines(points, lines, 1, CV_PI / 180, 0, 0, 0); // runs the actual detection
	// Draw the lines
	for (size_t i = 0; i < 1; i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 2000 * (-b));
		pt1.y = cvRound(y0 + 2000 * (a));
		pt2.x = cvRound(x0 - 2000 * (-b));
		pt2.y = cvRound(y0 - 2000 * (a));
		line(cdst, pt1, pt2, Scalar(0, 0, 255), 1, LINE_AA);
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
	HoughLines(points, lines, 1, thetaStep * toRad, 0, 0, 0); // runs the actual detection

	theta = 0;
	confidence = 0;

	vector<int> hist(360 / thetaStep + 1);

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
	//confidence = 1.0f - (confidence / 25.0f);
	confidence = 1.0f - confidence;
}
void FourierGetTheta(Mat points, float& theta, float& confidence)
{
	// Edge detection
	//Canny(points, dst, 50, 200, 3);
	//Mat points2;
	//cvtColor(points, points, COLOR_RGB2GRAY);

	// Standard Hough Line Transform
	vector<Vec3f> lines; // will hold the results of the detection
	float thetaStep = 1;
	float toRad = CV_PI / 180;
	HoughLines(points, lines, 1, thetaStep * toRad, 0, 0, 0); // runs the actual detection

	theta = 0;
	confidence = 0;

	vector<int> hist(360 / thetaStep + 1);

	float numberOfThetas = 0;

	for (size_t i = 0; i < lines.size(); i++)
	{
		lines[i][1] = lines[i][1] * 180 / CV_PI;
		if (lines[i][2] > lines[0][2] / 2)
		{
			numberOfThetas += lines[i][2];
			hist[round((lines[i][1]) / thetaStep)] += lines[i][2];
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
	//confidence = 1.0f - (confidence / 25.0f);
	confidence = 1.0f - confidence;
	theta -= 90;
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

void fftshift(const Mat& inputImg, Mat& outputImg)
{
	outputImg = inputImg.clone();
	int cx = outputImg.cols / 2;
	int cy = outputImg.rows / 2;
	Mat q0(outputImg, Rect(0, 0, cx, cy));
	Mat q1(outputImg, Rect(cx, 0, cx, cy));
	Mat q2(outputImg, Rect(0, cy, cx, cy));
	Mat q3(outputImg, Rect(cx, cy, cx, cy));
	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
}

void Complex2Gray(const Mat& inputImg, Mat& outputImg)
{
	Mat RI[2];
	split(inputImg, RI);
	cv::multiply(RI[0], RI[0], RI[0]);
	cv::multiply(RI[1], RI[1], RI[1]);
	cv::add(RI[0], RI[1], outputImg);
	cv::sqrt(outputImg, outputImg);
	double minn, maxx;
	cv::minMaxLoc(outputImg, &minn, &maxx);
	outputImg.convertTo(outputImg, CV_8U, 255 / maxx);
	int dilation_size = 0;
	Mat element = getStructuringElement(MORPH_ELLIPSE,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));
	/// Apply the dilation operation
	dilate(outputImg, outputImg, element);
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
	out << "imageNr,imageAngle,thetaHough,confidenceHough,thetaFourier,confidenceFourier,thetaProj,confidenceProj,\n";

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
			//blur(src_gray, src_gray, Size(3, 3));



			// Threshhold image + invert
			Mat threshold_output;
			threshold(src_gray, threshold_output, 150, 255, THRESH_BINARY_INV);



			//Get image with points from characters
			Mat points = Mat::zeros(src.size(), CV_8UC1);
			getPoints(threshold_output, points);


#pragma region DFT
			Mat planes[2] = { Mat_<float>(threshold_output.clone()), Mat::zeros(threshold_output.size(), CV_32F) };
			Mat fourierOutput;
			merge(planes, 2, fourierOutput);
			cv::dft(fourierOutput, fourierOutput);

			Mat fourierOutputShifted;
			fftshift(fourierOutput, fourierOutputShifted);

			Mat test;

			Complex2Gray(fourierOutput, test);


			double maxx, minn;
			minMaxLoc(test, &minn, &maxx);

			threshold(test, test, maxx/5, 1, THRESH_BINARY);
			test.convertTo(test, CV_32F);

			split(fourierOutput, planes);

			Mat test2;

			cv::multiply(test, planes[0], planes[0]);
			cv::multiply(test, planes[1], planes[1]);
			merge(planes, 2, fourierOutput);


			cv::idft(fourierOutput, fourierOutput, DFT_SCALE);

			split(fourierOutput, planes);
			planes[0].convertTo(test2, CV_8U, 255);

			test2 = Scalar(255) - test2;

			/*Mat dst, cdst, cdstP;
			DrawHoughLines(test, dst, cdst, cdstP);*/

			float thetaFourier = 0, confidenceFourier = 0;
			FourierGetTheta(test2, thetaFourier, confidenceFourier);
#pragma endregion

			Mat dst2, cdst2, cdstP2;
			DrawHoughLines(points, dst2, cdst2, cdstP2);

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
				<< to_string(thetaFourier) << ","
				<< to_string(confidenceFourier) << ","
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