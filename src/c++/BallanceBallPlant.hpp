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
	void setOriginAngle(int value);
	int getOriginAngle();
	void setMaxAngle(int value);
	int getMaxAngle();
	void setMinAngle(int value);
	int getMinAngle();
	Camera camera;
private:
	Serial device;
	int originAngle;
	int maxAngle;
	int minAngle;
};
#endif