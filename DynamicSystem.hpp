#ifndef DYNAMIC_SYSTEM_H
#define DYNAMIC_SYSTEM_H

class DynamicSystem{
public:
	double getOutput();
	virtual int update(double input) = 0;
	virtual bool outputAvailable();
private:
	double output;
protected:
	void setOutput(double output);
};
#endif