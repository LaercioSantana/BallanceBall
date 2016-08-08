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
	    	//cout<< camera.getPosition()<<endl;
        #endif

		if(!device.getFail()){
			//cout << "input : " << input<< endl;
			input += 90;
			if(input > 180)
				input = 180;
			else if(input < 0)
				input = 0;

	        device.write("%d:\n", (int) input );
	       // cout << (int) input << endl;
		}

		setOutput(camera.getPosition());
	}

	return 1;
}

bool
BallanceBallPlant::outputAvailable(){
	return camera.isLimitsSelected();
}