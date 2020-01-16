#pragma once
#include "IVoting.h"

#define REGISTER_ALGORITHM(TYPE) VotingSystem::Register(#TYPE, [](void) -> IVoting * { return new TYPE(); })
#define DEFINE_ALGORITHM_NAME(TYPE) const char * GetTypeAsString() const override { return #TYPE; }

class VotingSystem
{
public:
	typedef std::unordered_map<std::string, IVoting*> MapNameVoting;


	static void Initialize() {  }
	static void Register(const std::string& name, std::function<IVoting * (void)> instanceCreator)
	{
		MapNameVoting& nameMap = GetNameMap();
		nameMap[name] = instanceCreator();
	}

	static MapNameResult ComputeVoting(MapNameResult resultMap)
	{
		MapNameResult result;
		MapNameVoting& nameMap = GetNameMap();
		MapNameVoting::iterator it;
		for (it = nameMap.begin(); it != nameMap.end(); ++it)
		{
			std::string name = it->first;
			Result angle = it->second->Comput(resultMap);
			result[name] = angle;
		}
		return result;
	}

	inline static MapNameVoting& GetNameMap()
	{
		static MapNameVoting map;
		return map;
	}
};

