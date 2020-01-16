#pragma once
#include "VotingSystem.h"
class UnanimousVoting : public IVoting
{
public:
	Result Comput(MapNameResult& resultMap) {
		float angle=0;
		MapNameResult::iterator it;
		for (it = resultMap.begin(); it != resultMap.end(); it++)
		{
			angle += it->second.angle;
		}
		angle = angle/ resultMap.size();
		float confidence = 1;
		for (it = resultMap.begin(); it != resultMap.end(); it++)
		{
			confidence -= (abs(it->second.angle - angle))/resultMap.size();
		}
		return Result(angle,confidence);
	};
	DEFINE_ALGORITHM_NAME(UnanimousVoting)
};