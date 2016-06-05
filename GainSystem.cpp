#include "GainSystem.hpp"

GainSystem::GainSystem(double k): 
	k(k)
{
}

int
GainSystem::update(double input){
	setOutput(input * k);
	return 1;
}