#include "FourierTransform.h"
cv::Mat FourierTransform::ShiftImage(const cv::Mat& inputImg)
{
	cv::Mat outputImg = inputImg.clone();
	int cx = outputImg.cols / 2;
	int cy = outputImg.rows / 2;
	cv::Mat q0(outputImg, cv::Rect(0, 0, cx, cy));
	cv::Mat q1(outputImg, cv::Rect(cx, 0, cx, cy));
	cv::Mat q2(outputImg, cv::Rect(0, cy, cx, cy));
	cv::Mat q3(outputImg, cv::Rect(cx, cy, cx, cy));
	cv::Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
	return outputImg;
}
cv::Mat FourierTransform::ApplyTransform(const cv::Mat& src) {

	cv::Mat planes[] = { cv::Mat_<float>(src)/cv::Scalar(255), cv::Mat::zeros(src.size(), CV_32F) };
	cv::Mat complexI;
	cv::merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
	cv::dft(complexI, complexI);            // this way the result may fit in the source matrix

	/*cv::Mat test;
	cv::dft(complexI, test, cv::DFT_INVERSE | cv::DFT_COMPLEX_INPUT | cv::DFT_REAL_OUTPUT | cv::DFT_SCALE);*/

	split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude

	cv::Mat magI = planes[0];
	magI += cv::Scalar::all(1);                    // switch to logarithmic scale
	log(magI, magI);
	// crop the spectrum, if it has an odd number of rows or columns
	magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));
	// rearrange the quadrants of Fourier image  so that the origin is at the image center
	magI = ShiftImage(magI);

	normalize(magI, magI, 0, 1, cv::NORM_MINMAX);

	cv::Mat out;

	double min, max;
	cv::minMaxLoc(magI, &min, &max);
	magI.convertTo(out, CV_8U, 255 / max);

	return out;
}