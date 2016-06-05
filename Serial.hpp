#ifndef SERIAL_H
#define SERIAL_H

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
	bool getFail();
	string getPath();
private:
	bool fail;
	FILE *serial;
	string path;
};
#endif