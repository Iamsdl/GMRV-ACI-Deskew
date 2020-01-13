#include "Utils.h"
using namespace std;
using namespace cv;

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

void DrawHoughLines(Mat& points, Mat& dst, float& angle, int threshold, float step)
{
	cvtColor(points, dst, COLOR_GRAY2BGR);

	// Standard Hough Line Transform
	vector<Vec3f> lines; // will hold the results of the detection
	HoughLines(points, lines, 1, step*CV_PI / 180, threshold, 0, 0); // runs the actual detection
	
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
		line(dst, pt1, pt2, Scalar(0, 0, 255), 1, LINE_AA);
	}

	angle = (CV_PI/2-lines[0][1])* 180 / CV_PI;

}

void HoughGetTheta(Mat points, float& theta, float& confidence)
{
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

int ComputeAllImgs()
{
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

			threshold(test, test, maxx / 5, 1, THRESH_BINARY);
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
			//DrawHoughLines(points, dst2, cdst2, cdstP2);

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
}


int main()
{
	std::clock_t start;
	double duration;
	start = std::clock();

	/*for (float angle = -10; angle <= 10; angle += 0.1)
	{
		char buff[20];
		string fileName;
		sprintf_s(buff,"out/angle=%.1f.png", angle);
		fileName = buff;
		Mat out;
		Mat in;
		in = imread("in/input1.PNG");
		rotate_bound(in, out, angle);
		imwrite(fileName,out);
	}
	*/

	ofstream out("resultsHough StepAdjust.csv");
	out << "imageAngle,thetaHough\n";
		for (float imageAngle = -10; imageAngle <=10; imageAngle+=0.1)
		{
			imageAngle = round(imageAngle * 10) / 10;
			char buff[20];
			string filename;
			sprintf_s(buff, "angle=%.1f.png", imageAngle);
			filename = buff;
			// Loads an image
			Mat src = imread("out/" + filename, IMREAD_GRAYSCALE);
			// Check if image is loaded fine
			if (src.empty()) {
				printf(" Error opening image\n");
				printf(" Program Arguments: [image_name -- default %s] \n", filename);
				continue;
			}
			// Threshhold image + invert
			Mat threshold_output;
			threshold(src, threshold_output, 150, 255, THRESH_BINARY_INV);

			//Get image with points from characters
			Mat points = Mat::zeros(src.size(), CV_8UC1);
			getPoints(threshold_output, points);

			Mat dst2;
			float angle = 0,angle2=0,conf;
			DrawHoughLines(points, dst2, angle, 50,0.1);
			//HoughGetTheta(points, angle2, conf);
			int a = 1;
			out << imageAngle << ","<< to_string(angle) << "\n";
		}

	out.close();
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	std::cout << "Finished in " << duration << " seconds";
	cv::waitKey();
	return 0;
}