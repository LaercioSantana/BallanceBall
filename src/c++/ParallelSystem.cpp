#include "ParallelSystem.hpp"

ParallelSystem::ParallelSystem(DynamicSystem* subSystem){
	subSystems.push_back(subSystem);
}

ParallelSystem::ParallelSystem(vector<DynamicSystem*>& subSystems){
	this->subSystems = subSystems;
}

int
ParallelSystem::update(double input){
	double output = 0;
	for(int i = 0; i < subSystems.size();i++){
		subSystems[i]->update(input);
		output += subSystems[i]->getOutput();
	}

	setOutput(output);
}

bool
ParallelSystem::outputAvailable(){
	for(int i = 0; i < subSystems.size(); i++){
		if(!subSystems[i]->outputAvailable())
			return false;
	}

	return true;
}