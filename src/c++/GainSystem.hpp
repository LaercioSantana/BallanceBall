#ifndef GAIN_SYSTEM_H
#define GAIN_SYSTEM_H

#include "DynamicSystem.hpp"

class GainSystem : public DynamicSystem{
public:
	GainSystem(double k);
	int update(double input);
private:
	double k;
};
#endif

