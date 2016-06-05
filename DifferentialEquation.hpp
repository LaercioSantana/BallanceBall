
#ifndef DIFFERENTIAL_EQUATION_H
#define DIFFERENTIAL_EQUATION

#include "DynamicSystem.hpp"
#include <vector>
#include <armadillo>

using namespace std;
using namespace arma;

//not work to gain, a0 != 1 and to length(num) >= den
class DifferentialEquation : public DynamicSystem{
public:
	DifferentialEquation(vector<double> outputCoeffs, vector<double> inputCoeffs);
	DifferentialEquation(vector<double> outputCoeffs, vector<double> inputCoeffs, vector<double> initialStates);
	int update(double input);
	void set(vector<double> outputCoeffs, vector<double> inputCoeffs);
	void set(vector<double> outputCoeffs, vector<double> inputCoeffs, vector<double> initialStates);
	int getOrder();
	void setTimeVariation(double dt);
private:
	vector<double> outputCoeffs;
	vector<double> inputCoeffs;
	vector<double> initialStates;
	vector<double> beta;
	mat A;
	mat X;
	mat B;
	double dt;
	bool firstUpdate;
	void updateBeta();
};
#endif