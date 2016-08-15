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
	int write(const string msg, ...);
	int read(const string format, ...);
	void fflush();
	bool getFail();
	string getPath();
	int seek(int offset, int origin);
	FILE *serial;
private:
	bool fail;
	string path;
};
#endif