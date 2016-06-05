#ifndef CLOSE_LOOP_H
#define CLOSE_LOOP_H

#include "DynamicSystem.hpp"
#include <iostream>

using namespace std;

class CloseLoop : public DynamicSystem{
public:
	CloseLoop(DynamicSystem* openLoopSystem);
	int update(double reference);
	bool outputAvailable();
	void setWeights(double referenceWeight, double outputWeight);
private:
	DynamicSystem* openLoopSystem;
	double referenceWeight;
	double outputWeight;
};
#endif
