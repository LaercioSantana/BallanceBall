#ifndef BALLANCE_BALL_CONFIG
#define BALLANCE_BALL_CONFIG

#include <iostream>
#include <vector>
#include "Serial.hpp"
#include "string.h"

using namespace std;

class BallanceBallConfig{
public:
	BallanceBallConfig(const string& configFile);
	void read();
	void write();
	double getInput();
	void setInput(double input);
	vector<double> getPIDValues();
	void setPIDValues(vector<double> values);
	void setColorSelected(vector<int> colorSelected);
	vector<int> getColorSelected();
	void setColorLimits(vector<vector<int> > colorsLimits );
	vector<vector<int> > getColorLimits();
private:
	string configFile;

	double input;
	vector<double> pidValues;
	vector<int> colorSelected;
	vector<vector<int> > colorsLimits;
};
#endif
