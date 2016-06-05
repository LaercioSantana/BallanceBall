#ifndef SERIAL_SYSTEM_H
#define SERIAL_SYSTEM_H

#include "DynamicSystem.hpp"
#include <vector>

using namespace std;

class SerialSystem : public DynamicSystem{
public:
	SerialSystem(DynamicSystem* subSystem);
	SerialSystem(vector<DynamicSystem*>& subSystems);
	int update(double input);
	bool outputAvailable();
private:
	vector<DynamicSystem*> subSystems;
};
#endif