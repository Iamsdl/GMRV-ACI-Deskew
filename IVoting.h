#pragma once
#include "Utils.h"
class IVoting
{
public:
	virtual Result Comput(MapNameResult& resultMap) { return Result(); };
	virtual const char* GetTypeAsString() const { return "IVoting"; }
};

