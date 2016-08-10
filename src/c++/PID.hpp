
#ifndef PID_H
#define PID_H

#include "DynamicSystem.hpp"
#include <limits>
#include <math.h>
#include <string>

using namespace std;

class PID : public DynamicSystem{
public:
	PID(double kp, double ki, double kd);
	int update(double input);
	void set(double kp, double ki, double kd);
	void setLimits(double min, double max);
	void setTimeVariation(double dt);
	double getMax();
	double getMin();
	double get(string k);	
private:
	double kp;
	double ki;
	double kd;
	double max;
	double min;
	double dt;
	double lastInput;
	double sum;
};
#endif