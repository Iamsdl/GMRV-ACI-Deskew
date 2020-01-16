#pragma once
#include "VotingSystem.h"
class BestFirstVoting : public IVoting
{
public:
	Result Comput(MapNameResult& resultMap) {
		float maxConfidence = 0;
		float angle;
		MapNameResult::iterator it;
		for (it = resultMap.begin(); it != resultMap.end(); it++)
		{
			if (it->second.confidence > maxConfidence)
			{
				maxConfidence = it->second.confidence;
				angle = it->second.angle;
			}
		}
		return Result(angle,maxConfidence);
	};
	DEFINE_ALGORITHM_NAME(BestFirstVoting)
};

