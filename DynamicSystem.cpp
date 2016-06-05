#include "DynamicSystem.hpp"

double
DynamicSystem::getOutput(){
	return output;
}

void
DynamicSystem::setOutput(double output){
	this->output = output;
}

bool
DynamicSystem::outputAvailable(){
	return true;
}