#include "Utils.h"
using namespace std;
using namespace cv;

cv::Mat CreateSquareImage(const cv::Mat& input, const cv::Scalar& bgcolor)
{
	cv::Mat output;
	int max = input.cols > input.rows ? input.cols : input.rows;
	cv::Size dstSize = { max,max };

	double h1 = dstSize.width * (input.rows / (double)input.cols);
	double w2 = dstSize.height * (input.cols / (double)input.rows);
	if (h1 <= dstSize.height) {
		cv::resize(input, output, cv::Size(dstSize.width, h1));
	}
	else {
		cv::resize(input, output, cv::Size(w2, dstSize.height));
	}

	int top = (dstSize.height - output.rows) / 2;
	int down = (dstSize.height - output.rows + 1) / 2;
	int left = (dstSize.width - output.cols) / 2;
	int right = (dstSize.width - output.cols + 1) / 2;

	cv::copyMakeBorder(output, output, top, down, left, right, cv::BORDER_CONSTANT, bgcolor);

	return output;
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

vector<int> ComputeVHistogram(Mat src, int binSize)
{
	vector<int> bin_sum(src.rows/binSize+1);
	threshold(src, src, 150, 1, THRESH_BINARY);
	//compute row sums
	for (int i = 0; i < src.rows; ++i)
	{
		for (int j = 0; j < src.cols; ++j)
		{
			bin_sum[i/binSize] += src.at<uchar>(i,j);
		}
	}

	return bin_sum;
}
vector<int> ComputeVHistogram(Mat src)
{
	Mat row_sum;
	threshold(src, src, 150, 1, THRESH_BINARY);
	//compute row sums
	reduce(src, row_sum, 1, REDUCE_SUM, CV_32F);

	//copy sums from Mat to vector
	vector<int> sumRows(src.size().height);
	sumRows.assign((float*)row_sum.data, (float*)row_sum.data + row_sum.total());
	
	return sumRows;
}

void GeneratePoints(Mat& src_inverted, Mat& drawing)
{
	/// Find contours
	vector<vector<Point> > contours;
	findContours(src_inverted, contours, RETR_LIST, CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
	}

	/// Draw polygonal contour + bonding rects + circles
	for (int i = 0; i < boundRect.size(); i++)
	{
		Point2f point = Point2f(boundRect[i].x + boundRect[i].width / 2, boundRect[i].y + boundRect[i].height);
		circle(drawing, point,1,Scalar(255),FILLED,LINE_4,0);
	}
}

void RotateImage(const Mat& image, Mat& out_image, float angle,cv::Scalar emptyColor)
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
	 cv::warpAffine(image, out_image, M,cv::Size(nW, nH),cv::INTER_LINEAR,cv::BORDER_CONSTANT,emptyColor);
}


int ComputeValue_Contrast(int x, int r1, int s1, int r2, int s2)
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

void ContrastStretch(cv::Mat& src, cv::Mat dst, int r1, int s1, int r2, int s2)
{
	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols; x++) {
			for (int c = 0; c < 3; c++) {
				int output = ComputeValue_Contrast(src.at<uchar>(y, x), r1, s1, r2, s2);
				dst.at<uchar>(y, x) = cv::saturate_cast<uchar>(output);
			}
		}
	}
}

void NormalizeAngle(float& angle)
{
	angle = ((int)round(angle)) % 90;
	if (angle > 45)
		angle -= 90;
	if (angle < -45)
		angle += 90;
}