#include <cstdio>
#include <iostream>
#include <stdarg.h>

using namespace std;

class Serial{
public:
	Serial(const string device);
	~Serial();
	void write(const string msg, ...);
	string read();
	void fflush();
private:
	FILE *serial;
};