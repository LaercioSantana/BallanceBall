#include "Serial.hpp"

Serial::Serial(const string device){
	serial = fopen(device.c_str(),"w");
}

Serial::~Serial(){
	fclose(serial);
	cout<<"close serial"<<endl;
}

void
Serial::write(const string format, ...){
	va_list ap;
	va_start (ap, format.c_str());
	vfprintf(serial,format.c_str(), ap);
	va_end (ap);
}

string
Serial::read(){
	return "";
}

void
Serial::fflush(){
	std::fflush(serial);
}
