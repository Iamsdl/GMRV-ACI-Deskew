#pragma once
#include "VotingSystem.h"
class UnanimousVoting : public IVoting
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

		int nr = 0;
		float angle = 0;
		float confidence = 0;
		for (it = resultMap.begin(); it != resultMap.end(); it++)
		{
			if (it->second.confidence > maxConfidence / 2)
			{
				angle += it->second.angle;
				confidence += it->second.confidence;

				nr++;
			}
		}
		angle /= nr;
		confidence /= nr;

		return Result(angle, confidence);
	};
	DEFINE_ALGORITHM_NAME(UnanimousVoting)
};