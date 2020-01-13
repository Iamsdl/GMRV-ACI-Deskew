#include "Utils.h"
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

void rotate_bound(Mat& image, Mat& out_image, float angle)
{
	//grab the dimensions of the imageand then determine the
	//center
	Mat img;
	bitwise_not(image, img);

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
	 warpAffine(img, out_image, M, Size(nW, nH));
	 bitwise_not(out_image, out_image);

	
}
