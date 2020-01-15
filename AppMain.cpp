#include "FrequencyHough.h"
#include "SpatialHough.h"
#include "SpatialProjectionProfiling.h"
#include "FrequencyProjectionProfiling.h"

using namespace std;
using namespace cv;

//int ComputeAllImgs()
//{
//	int nrOfImages = 3;
//	int lowestAngle = -5;
//	int highestAngle = 5;
//
//	ofstream out("results.csv");
//	out << "imageNr,imageAngle,thetaHough,confidenceHough,thetaFourier,confidenceFourier,thetaProj,confidenceProj,\n";
//
//	for (int imageNr = 1; imageNr <= nrOfImages; imageNr++)
//	{
//		for (int imageAngle = lowestAngle * 10; imageAngle <= highestAngle * 10; imageAngle++)
//		{
//			string x = to_string(imageAngle / 10) + ((imageAngle % 10 != 0) ? "." + to_string(abs(imageAngle % 10)) : "");
//
//			// Declare the output variables
//			string filename = to_string(imageNr) + "-theta=" + x + ".jpg";
//			// Loads an image
//			Mat src = imread("rotite/" + filename, IMREAD_GRAYSCALE);
//
//			// Check if image is loaded fine
//			if (src.empty()) {
//				printf(" Error opening image\n");
//				printf(" Program Arguments: [image_name -- default %s] \n", filename);
//				return -1;
//			}
//
//			// Convert image to gray and blur it
//			Mat src_gray;
//			src.copyTo(src_gray); //already grayscale
//			//blur(src_gray, src_gray, Size(3, 3));
//
//
//
//			// Threshhold image + invert
//			Mat threshold_output;
//			threshold(src_gray, threshold_output, 150, 255, THRESH_BINARY_INV);
//
//
//
//			//Get image with points from characters
//			Mat points = Mat::zeros(src.size(), CV_8UC1);
//			getPoints(threshold_output, points);
//
//
//#pragma region DFT
//			Mat planes[2] = { Mat_<float>(threshold_output.clone()), Mat::zeros(threshold_output.size(), CV_32F) };
//			Mat fourierOutput;
//			merge(planes, 2, fourierOutput);
//			cv::dft(fourierOutput, fourierOutput);
//
//			Mat fourierOutputShifted;
//			fftshift(fourierOutput, fourierOutputShifted);
//
//			Mat test;
//
//			Complex2Gray(fourierOutput, test);
//
//
//			double maxx, minn;
//			minMaxLoc(test, &minn, &maxx);
//
//			threshold(test, test, maxx / 5, 1, THRESH_BINARY);
//			test.convertTo(test, CV_32F);
//
//			split(fourierOutput, planes);
//
//			Mat test2;
//
//			cv::multiply(test, planes[0], planes[0]);
//			cv::multiply(test, planes[1], planes[1]);
//			merge(planes, 2, fourierOutput);
//
//
//			cv::idft(fourierOutput, fourierOutput, DFT_SCALE);
//
//			split(fourierOutput, planes);
//			planes[0].convertTo(test2, CV_8U, 255);
//
//			test2 = Scalar(255) - test2;
//
//			/*Mat dst, cdst, cdstP;
//			DrawHoughLines(test, dst, cdst, cdstP);*/
//
//			float thetaFourier = 0, confidenceFourier = 0;
//			FourierGetTheta(test2, thetaFourier, confidenceFourier);
//#pragma endregion
//
//			Mat dst2, cdst2, cdstP2;
//			//DrawHoughLines(points, dst2, cdst2, cdstP2);
//
//			float thetaHough = 0, confidenceHough = 0;
//			Hough::HoughGetTheta(points, thetaHough, confidenceHough);
//
//			/*Mat rotatedImage = Mat::zeros(src.size(), CV_8UC3);
//			rotate_bound(src, rotatedImage, thetaHough * 180 / CV_PI - 90);*/
//
//			float thetaProj = 0, confidenceProj = 0;
//			ProjGetTheta(points, thetaProj, confidenceProj, 10);
//
//			/*Mat src3 = Mat::zeros(src.size(), CV_8UC3);
//			rotate_bound(src, src3, thetaProj);*/
//
//			out << to_string(imageNr) << ","
//				<< to_string(-imageAngle / 10.0f) << ","
//				<< to_string(thetaHough) << ","
//				<< to_string(confidenceHough) << ","
//				<< to_string(thetaFourier) << ","
//				<< to_string(confidenceFourier) << ","
//				<< to_string(thetaProj) << ","
//				<< to_string(confidenceProj) << ",\n";
//		}
//	}
//	out.close();
//}

int main()
{
	std::clock_t start;
	double duration;
	start = std::clock();
	stringstream sstream;
	sstream << fixed << setprecision(2);

	sstream << "valori-HoughConfidenceMod=" << HOUGH_CONFIDENCE_MOD << "ProjConfidenceMod=" << PROJ_CONFIDENCE_MOD << ".csv";
	ofstream out(sstream.str());
	sstream.str(std::string());

	out << fixed << setprecision(2);
	out << "index,angle,houghAngle,houghConfidence,projectionAngle,projectionConfidence,freqHoughAngle,freqHoughConfidence\n";

	IAlgorithm* spatialHough = new SpatialHough();
	IAlgorithm* spatialProjection = new SpatialProjectionProfiling();
	IAlgorithm* frequencyHough = new FrequencyHough();

	for (int i = FIRST_IMAGE_IDX; i <= LAST_IMAGE_IDX; ++i)
	{
		for (float angle = MIN_ANGLE; angle <= MAX_ANGLE; angle += INPUT_STEP)
		{
			/*string inputName = "in/" + to_string(i) + ".png";
			Mat src = imread(inputName);

			sstream << "out/" << i << "-theta=" << angle << ".png";
			string outName = sstream.str();
			sstream.str(std::string());
			Mat out;

			RotateImage(src, out, angle, cv::Scalar(255, 255, 255));
			imwrite(outName, out);*/


			sstream << "out/" << i << "-theta=" << angle << ".png";
			string inputName = sstream.str();
			sstream.str(std::string());

			Mat src = imread(inputName, IMREAD_GRAYSCALE);
			cv::Mat preprocessedSrc = IAlgorithm::Preprocess(src);

			float houghAngle, projectionAngle, freqHoughAngle, freqProjectionAngle;
			float houghConfidence, projectionConfidence, freqHoughConfidence, freqProjectionConfidence;

			spatialHough->Compute(preprocessedSrc, houghAngle, houghConfidence);
			spatialProjection->Compute(preprocessedSrc, projectionAngle, projectionConfidence);
			frequencyHough->Compute(src, freqHoughAngle, freqHoughConfidence);
			out << i << "," << angle << "," << houghAngle << "," << houghConfidence << "," << projectionAngle << "," << projectionConfidence << "," << freqHoughAngle << "," << freqHoughConfidence << "\n";
		}
	}

	out.close();

	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	std::cout << "Finished in " << duration << " seconds";
	cv::waitKey();
	return 0;
}