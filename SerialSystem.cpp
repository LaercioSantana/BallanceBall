#include "SerialSystem.hpp"

SerialSystem::SerialSystem(DynamicSystem* subSystem){
	subSystems.push_back(subSystem);
}

SerialSystem::SerialSystem(vector<DynamicSystem*>& subSystems){
	this->subSystems = subSystems;
}

int
SerialSystem::update(double input){
	for(int i = 0; i < subSystems.size();i++){
		subSystems[i]->update(input);
		input = subSystems[i]->getOutput();
	}
	
	setOutput(input);
}

bool
SerialSystem::outputAvailable(){
	for(int i = 0; i < subSystems.size(); i++){
		if(!subSystems[i]->outputAvailable())
			return false;
	}

	return true;
}