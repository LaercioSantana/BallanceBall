#ifndef BALANCE_BALL_PLANT_H
#define BALANCE_BALL_PLANT_H

#include "DynamicSystem.hpp"
#include "Camera.hpp"
#include "Serial.hpp"

class BallanceBallPlant : public DynamicSystem{
public:
	BallanceBallPlant(const Camera& camera, const Serial& device);
	int update(double input);
	bool outputAvailable();
private:
	Camera camera;
	Serial device;
};
#endif