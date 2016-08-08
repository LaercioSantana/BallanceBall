#include "DifferentialEquation.hpp"

DifferentialEquation::DifferentialEquation(vector<double> outputCoeffs, vector<double> inputCoeffs):
	outputCoeffs(outputCoeffs),
	inputCoeffs(inputCoeffs),
	dt(0.1)
{
	initialStates.resize(outputCoeffs.size());
	beta.reserve(outputCoeffs.size());
	set(outputCoeffs, inputCoeffs);
}

DifferentialEquation::DifferentialEquation(vector<double> outputCoeffs, vector<double> inputCoeffs, vector<double> initialStates):
	outputCoeffs(outputCoeffs),
	inputCoeffs(inputCoeffs),
	initialStates(initialStates),
	dt(0.1)
{
	beta.reserve(outputCoeffs.size());
	set(outputCoeffs, inputCoeffs, initialStates);
}

void
DifferentialEquation::updateBeta(){
	int n = getOrder();
	for(int i = 0; i < n+1; i++){
		beta[i] = inputCoeffs[i];
		for(int j = i; j >= 1; j--)
			beta[i] -= outputCoeffs[i - j + 1] * beta[j - 1];
	}
}

int
DifferentialEquation::update(double input){
	if(false){
		int n = getOrder();
		mat Y(&initialStates[0], n, 1, false);
		mat BETA(&beta[0], n, 1, true);
		X = Y - BETA * input;
		cout<<"input: "<<BETA<<endl;
		X.print("initial: ");
		firstUpdate = false;
	}

	X = X + (A * X + B * input ) *  dt;
	setOutput(X(0,0) + beta[0]);

	return 1;
}

void
DifferentialEquation::set(vector<double> outputCoeffs, vector<double> inputCoeffs, vector<double> initialStates){
	this->initialStates = initialStates;
	this->set(outputCoeffs, inputCoeffs);
}

void
DifferentialEquation::set(vector<double> outputCoeffs, vector<double> inputCoeffs){
	this->outputCoeffs = outputCoeffs;
	this->inputCoeffs = inputCoeffs;
	updateBeta();

	int n = getOrder();

	//make matrix A
	A = mat(n, n, fill::zeros);
	if(getOrder() > 1)
		A.diag(1).ones();
	for(int i = 0; i < n; i++)
		A(n-1, i) = -outputCoeffs[n - i];

	B = mat(&beta[1], n, 1, true);
	//##need implements getStates from inital states
	X = mat(n, 1, fill::zeros);


	setOutput(0);

	cout<< getOutput() <<endl;
	B.print("B: ");
	X.print("X: ");
	A.print("A: ");

	firstUpdate = true;
}

int
DifferentialEquation::getOrder(){
	return outputCoeffs.size()-1;
}

void
DifferentialEquation::setTimeVariation(double dt){
	this->dt = dt;
}