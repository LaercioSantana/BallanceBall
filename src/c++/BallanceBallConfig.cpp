#include "BallanceBallConfig.hpp"

BallanceBallConfig::BallanceBallConfig(const string& configFile):
	configFile(configFile)
{
	pidValues = vector<double> {0, 0, 0};
	colorSelected = vector<int> {0, 0, 0};
	colorsLimits = vector<vector<int> > {{0, 0, 0}, {0, 0, 0}};
}

void
BallanceBallConfig::read(){
	char s[100]; 
	Serial file(configFile, "r");
	file.seek(0, SEEK_SET);
	while(file.read("%s", s) != EOF){
		if(strcmp(s, "INPUT") == 0){
			file.read("%lf", &this->input);
		}
		else if(strcmp(s, "PID") == 0){
			file.read("%lf %lf %lf", &this->pidValues[0], &this->pidValues[1], &this->pidValues[2]);
		}
		else if(strcmp(s, "COLOR_SELECTD") == 0){
			file.read("%d %d %d", &this->colorSelected[0], &this->colorSelected[1], &this->colorSelected[2]);
		}
		else if(strcmp(s, "COLORS_LIMITS") == 0){
			file.read("%d %d %d", &this->colorsLimits[0][0], &this->colorsLimits[0][1], &this->colorsLimits[0][2]);
			file.read("%d %d %d", &this->colorsLimits[1][0], &this->colorsLimits[1][1], &this->colorsLimits[1][2]);
		}
	}
}

void
BallanceBallConfig::write(){
	char s[100]; 
	Serial file(configFile, "w");

	file.write("INPUT %g \n", input);
	file.write("PID %g %g %g \n", this->pidValues[0], this->pidValues[1], this->pidValues[2]);
	file.write("COLOR_SELECTD %d %d %d \n", this->colorSelected[0], this->colorSelected[1], this->colorSelected[2]);
	file.write("COLORS_LIMITS %d %d %d ", this->colorsLimits[0][0], this->colorsLimits[0][1], this->colorsLimits[0][2]);
	file.write("%d %d %d \n", this->colorsLimits[1][0], this->colorsLimits[1][1], this->colorsLimits[1][2]);
}

double
BallanceBallConfig::getInput(){
	return input;
}

void
BallanceBallConfig::setInput(double input){
	this->input = input;
}

vector<double>
BallanceBallConfig::getPIDValues(){
	return pidValues;
}

void
BallanceBallConfig::setPIDValues(vector<double> values){
	this->pidValues = values;
}

void
BallanceBallConfig::setColorSelected(vector<int> colorSelected){
	this->colorSelected = colorSelected;
}

vector<int>
BallanceBallConfig::getColorSelected(){
	return colorSelected;
}

void
BallanceBallConfig::setColorLimits(vector<vector<int> > colorsLimits ){
	this->colorsLimits = colorsLimits;
}

vector<vector<int> >
BallanceBallConfig::getColorLimits(){
	return colorsLimits;
}