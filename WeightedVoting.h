#pragma once
#include "VotingSystem.h"
class WeightedVoting : public IVoting
{
public:
	Result Comput(MapNameResult& resultMap) {
		MapNameResult::iterator it;

		float maxConfidence = 0;
		for (it = resultMap.begin(); it != resultMap.end(); it++)
		{
			if (it->second.confidence > maxConfidence)
				maxConfidence = it->second.confidence;
		}

		float sumConfidence = 0;
		float angle = 0;
		float confidence = 0;
		for (it = resultMap.begin(); it != resultMap.end(); it++)
		{
			if (it->second.confidence > maxConfidence / 2)
			{
				angle += it->second.angle * it->second.confidence;
				confidence += it->second.confidence * it->second.confidence;
				sumConfidence += it->second.confidence;
			}
		}
		angle /= sumConfidence;
		confidence /= sumConfidence;

		return Result(angle, confidence);
	};
	DEFINE_ALGORITHM_NAME(WeightedVoting)
};