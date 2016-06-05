#include "CloseLoop.hpp"

CloseLoop::CloseLoop(DynamicSystem* openLoopSystem):
	openLoopSystem(openLoopSystem)	
{	
	setWeights(1,-1);
}

int
CloseLoop::update(double reference){
	if(openLoopSystem->outputAvailable()){
		double input =  reference - openLoopSystem->getOutput();
		//cout << "i: " << input << endl;
		//cout << "o: " << openLoopSystem->getOutput() << endl;
		
		openLoopSystem->update(input);
		setOutput(openLoopSystem->getOutput());
	}
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