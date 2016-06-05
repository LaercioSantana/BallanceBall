#ifndef PARALLEL_SYSTEM_H
#define PARALLEL_SYSTEM_H

#include "DynamicSystem.hpp"
#include <vector>

using namespace std;

class ParallelSystem : public DynamicSystem{
public:
	ParallelSystem(DynamicSystem* subSystem);
	ParallelSystem(vector<DynamicSystem*>& subSystems);
	int update(double input);
	bool outputAvailable();
private:
	vector<DynamicSystem*> subSystems;
};
#endif