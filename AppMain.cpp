#include "FrequencyHough.h"
#include "SpatialHough.h"
#include "SpatialProjectionProfiling.h"

#include "BestFirstVoting.h"
#include "WeightedVoting.h"
#include "UnanimousVoting.h"
using namespace std;
using namespace cv;

void RotateAllImages() {
	stringstream sstream;
	sstream << fixed << setprecision(2);

	for (int i = FIRST_IMAGE_IDX; i <= LAST_IMAGE_IDX; ++i)
	{
		for (float angle = MIN_ANGLE; angle <= MAX_ANGLE; angle += INPUT_STEP)
		{
			string inputName = "in/" + to_string(i) + ".png";
			Mat src = imread(inputName);

			sstream << "out/" << i << "-theta=" << angle << ".png";
			string outName = sstream.str();
			sstream.str(std::string());
			Mat out;

			RotateImage(src, out, angle, cv::Scalar(255, 255, 255));
			imwrite(outName, out);
		}
	}
}

void ComputeAllImages(){
	stringstream sstream;
	sstream << fixed << setprecision(2);

	sstream << "valori-HoughConfidenceMod=" << HOUGH_CONFIDENCE_MOD << "ProjConfidenceMod=" << PROJ_CONFIDENCE_MOD << ".csv";
	ofstream out(sstream.str());
	sstream.str(std::string());

	REGISTER_ALGORITHM(BestFirstVoting);
	REGISTER_ALGORITHM(WeightedVoting);
	REGISTER_ALGORITHM(UnanimousVoting);

	out << fixed << setprecision(2);
	out << "index,angle,"
		"houghAngle,houghConfidence,"
		"projectionAngle,projectionConfidence,"
		"freqHoughAngle,freqHoughConfidence,"
		"bestFirstAngle,bestFirstConfidence,"
		"unanimousAngle,unanimousConfidence,"
		"weightedAngle,weightedConfidence\n";

	IAlgorithm* spatialHough = new SpatialHough();
	IAlgorithm* spatialProjection = new SpatialProjectionProfiling();
	IAlgorithm* frequencyHough = new FrequencyHough();

	for (int i = FIRST_IMAGE_IDX; i <= LAST_IMAGE_IDX; ++i)
	{
		for (float angle = MIN_ANGLE; angle <= MAX_ANGLE; angle += INPUT_STEP)
		{
			sstream << "out/" << i << "-theta=" << angle << ".png";
			string inputName = sstream.str();
			sstream.str(std::string());

			Mat src = imread(inputName, IMREAD_GRAYSCALE);
			cv::Mat preprocessedSrc = IAlgorithm::Preprocess(src);

			float houghAngle, projectionAngle, freqHoughAngle;
			float houghConfidence, projectionConfidence, freqHoughConfidence;

			spatialHough->Compute(preprocessedSrc, houghAngle, houghConfidence);
			spatialProjection->Compute(preprocessedSrc, projectionAngle, projectionConfidence);
			frequencyHough->Compute(src, freqHoughAngle, freqHoughConfidence);

			Result hough(houghAngle, houghConfidence);
			Result projectionProfiling(projectionAngle, projectionConfidence);
			Result freqHough(freqHoughAngle, freqHoughConfidence);

			MapNameResult resultsMap;
			resultsMap["hough"] = hough;
			resultsMap["projection"] = projectionProfiling;
			resultsMap["freqHough"] = freqHough;

			MapNameResult votingResult = VotingSystem::ComputeVoting(resultsMap);


			out << i << "," << angle << "," << houghAngle << "," << houghConfidence << ","
				<< projectionAngle << "," << projectionConfidence << ","
				<< freqHoughAngle << "," << freqHoughConfidence << ","
				<< votingResult["BestFirstVoting"].angle << "," << votingResult["BestFirstVoting"].confidence << ","
				<< votingResult["UnanimousVoting"].angle << "," << votingResult["UnanimousVoting"].confidence << ","
				<< votingResult["WeightedVoting"].angle << "," << votingResult["WeightedVoting"].confidence << "\n";
		}
	}

	out.close();
	delete spatialHough;
	delete spatialProjection;
	delete frequencyHough;
}

int main()
{
	std::clock_t start;
	double duration;
	start = std::clock();
	
	//RotateAllImages();
	ComputeAllImages();

	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	std::cout << "Finished in " << duration << " seconds";
	cv::waitKey();
	return 0;
}