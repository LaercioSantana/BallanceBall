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
	
	double output = kp * input + ki * sum + kd * (di);
	if(output > getMax())
		output = getMax();
	else if(output < getMin())
		output = getMin();

	setOutput(output);

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
	return this->min;
}

double
PID::get(string k){
	if(k == "kp")
		return this->kp;
	if(k == "ki")
		return this->ki;
	return this->kd; 
}