#include "Serial.hpp"

Serial::Serial(const string device):
	path(device)
{
	serial = fopen(device.c_str(),"w+");
	if(serial == NULL)
		cout << "Can't open file: " << device.c_str() << endl;
}

Serial::Serial(const string device, const string mode):
	path(device)
{
	serial = fopen(device.c_str(), mode.c_str());
	if(serial == NULL)
		cout << "Can't open file: " << device.c_str() << endl;
}

Serial::~Serial(){
	fclose(serial);
}

int
Serial::write(const string format, ...){
	va_list ap;
	va_start (ap, format.c_str());
	int result = vfprintf(serial,format.c_str(), ap);
	va_end (ap);

	return result;
}

int
Serial::read(const string format, ...){
	va_list ap;
	va_start (ap, format.c_str());
	int result = vfscanf(serial,format.c_str(), ap);
	va_end (ap);

	return result;
}

void
Serial::fflush(){
	std::fflush(serial);
}

bool
Serial::getFail(){
	return serial == NULL;
}

string
Serial::getPath(){
	return path;
}

int
Serial::seek(int offset, int origin){
	return fseek(serial, offset, origin);
}