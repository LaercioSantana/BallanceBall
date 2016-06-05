#include "BallanceBallPlant.hpp"

BallanceBallPlant::BallanceBallPlant(const Camera& camera, const Serial& device):
  camera(camera),
  device(device)
{
}

int
BallanceBallPlant::update(double input){
	if(!camera.update())
        return 0;

	if(camera.isLimitsSelected()){
	    #ifdef DEBUG
	    	cout<< camera.getPosition()<<endl;
        #endif
		
		if(!device.getFail()){
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