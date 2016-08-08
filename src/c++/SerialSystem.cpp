#include "SerialSystem.hpp"

SerialSystem::SerialSystem(DynamicSystem* subSystem){
	subSystems.push_back(subSystem);
}

SerialSystem::SerialSystem(vector<DynamicSystem*>& subSystems){
	this->subSystems = subSystems;
}

int
SerialSystem::update(double input){
	int status = 1;
	for(int i = 0; i < subSystems.size();i++){
		int u = subSystems[i]->update(input);
		input = subSystems[i]->getOutput();
		if(u == 0)
			status = 0;
	}

	setOutput(input);
	return status;
}

bool
SerialSystem::outputAvailable(){
	for(int i = 0; i < subSystems.size(); i++){
		if(!subSystems[i]->outputAvailable())
			return false;
	}

	return true;
}