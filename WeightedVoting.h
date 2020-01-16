#pragma once
#include "VotingSystem.h"
class WeightedVoting : public IVoting
{
public:
	Result Comput(MapNameResult& resultMap) {
		float sumConfidence = 0;
		float angle=0;
		MapNameResult::iterator it;
		for (it = resultMap.begin(); it != resultMap.end(); it++)
		{
			angle += it->second.angle * it->second.confidence;
			sumConfidence += it->second.confidence;
		}
		angle = angle / sumConfidence;
		float confidence = 1;
		for (it = resultMap.begin(); it != resultMap.end(); it++)
		{
			confidence -=(abs(it->second.angle-angle)) * it->second.confidence;
		}
		return Result(angle,confidence);
	};
	DEFINE_ALGORITHM_NAME(WeightedVoting)
};