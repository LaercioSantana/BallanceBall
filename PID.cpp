#include "PID.hpp"
#include <iostream>

using namespace std;

PID::PID(double kp, double ki, double kd):
	kp(kp),
	ki(ki),
	kd(kd),
	sum(0),
	lastInput(0),
	dt(0.1)
{
	
	setLimits(numeric_limits<double>::min(), numeric_limits<double>::max());
}

int
PID::update(double input){
	double di = (input - lastInput) / dt;
	lastInput = input;

	sum += input * dt;
	//cout<< input << " : " << sum << " : " << di << endl;
	setOutput(kp * input + ki * sum + kp * (di));

	return 1;
}

void
PID::set(double kp, double ki, double kd){
	this->kp = kp;
	this->ki = ki;
	this->kd = kd;
}

void
PID::setTimeVariation(double dt){
	this->dt = dt;
}

void 
PID::setLimits(double min, double max){
	this->min = min;
	this->max = max;
}

double 
PID::getMax(){
	return this->max;
}

double 
PID::getMin(){
	return this->max;
}