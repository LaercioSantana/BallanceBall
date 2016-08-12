#include "CloseLoop.hpp"

CloseLoop::CloseLoop(DynamicSystem* openLoopSystem):
	openLoopSystem(openLoopSystem)
{
	setWeights(1,-1);
}

int
CloseLoop::update(double reference){
	double input = 0;
	if(openLoopSystem->outputAvailable()){
		input =  reference - openLoopSystem->getOutput();

		setOutput(openLoopSystem->getOutput());
	}

	int status = openLoopSystem->update(input);

	return status;
}

bool
CloseLoop::outputAvailable(){
	return openLoopSystem->outputAvailable();
}

void
CloseLoop::setWeights(double referenceWeight, double outputWeight){
	this->referenceWeight = referenceWeight;
	this->outputWeight = outputWeight;
}