#include "BallanceBallPlant.hpp"

BallanceBallPlant::BallanceBallPlant(const Camera& camera, const Serial& device):
  camera(camera),
  device(device),
  originAngle(90), maxAngle(180), minAngle(0)
{
}

int
BallanceBallPlant::update(double input){
	if(!camera.update())
        return 0;

	if(camera.isLimitsSelected()){
		if(!device.getFail()){
			input += this->originAngle;
			if(input > this->maxAngle)
				input = this->maxAngle;
			else if(input < this->minAngle)
				input = this->minAngle;

			device.write("%d:\n", (int) input );
		}

		setOutput(camera.getPosition());
	}

	return 1;
}

bool
BallanceBallPlant::outputAvailable(){
	return camera.isLimitsSelected();
}

void
BallanceBallPlant::setOriginAngle(int value){
	if(value < this->maxAngle && value > this->minAngle)
		this->originAngle = value;
}

int
BallanceBallPlant::getOriginAngle(){
	return this->originAngle;
}

void
BallanceBallPlant::setMaxAngle(int value){
	this->maxAngle = value;
}

int
BallanceBallPlant::getMaxAngle(){
	return this->maxAngle;
}

void
BallanceBallPlant::setMinAngle(int value){
	this->minAngle = value;
}

int
BallanceBallPlant::getMinAngle(){
	return this->minAngle;
}
