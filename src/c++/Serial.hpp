#ifndef SERIAL_H
#define SERIAL_H

#include <cstdio>
#include <iostream>
#include <stdarg.h>

using namespace std;

class Serial{
public:
	Serial(const string device);
	Serial(const string device, const string mode);
	~Serial();
	void write(const string msg, ...);
	void read(const string format, ...);
	void fflush();
	bool getFail();
	string getPath();
	FILE *serial;
private:
	bool fail;
	string path;
};
#endif