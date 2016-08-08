#include <math.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include "stdio.h"
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <stdarg.h>
#include <sys/ioctl.h>


/*#include <QApplication>
#include <QLabel>
#include <QWidget>*/

//#define DEBUG
#define PI 3.14159265

#include "BallanceBallPlant.hpp"
#include "SerialSystem.hpp"
#include "GainSystem.hpp"
#include "DifferentialEquation.hpp"
#include "CloseLoop.hpp"
#include "ParallelSystem.hpp"
#include "PID.hpp"



void showHelp();
void ReadStuff (FILE * stream, const char * format, ...);

//configuration variables
string videoFile = "";
int webcam = 0;
string sampleFile = "temp.dat";
bool debug = false;
string arduinoDevice = "/dev/ttyUSB0";
int i = 0;
double kp = -70, ki = -0.03, kd = -1;
double input = 1;
int main( int argc, char** argv ){
	//deconding arguments
	string arg;
	for(int i = 0; i < argc; i++){
		arg = string(argv[i]);

		if(arg == "-v")//video input
			videoFile = string(argv[++i]);
		else if(arg == "-s")//sample output
			sampleFile = string(argv[++i]);
		else if(arg == "-ns")
			sampleFile = "";
		else if (arg == "--debug")
			debug = true;
		else if(arg == "-c")
			webcam = std::stoi(argv[++i]);
		else if (arg == "-d")
			arduinoDevice = string(argv[++i]);
		else if(arg == "-h"){
			showHelp();
			return 0;
		}
		else if(arg == "--pid"){
			kp = std::stod(argv[++i]);
			ki = std::stod(argv[++i]);
			kd = std::stod(argv[++i]);
		}
		else if(arg == "-i")
			input = std::stod(argv[++i]);
	}

	arg = string(argv[1]);

	if(arg == "serial"){
		GainSystem s1(2);
		GainSystem s2(3);
		vector<DynamicSystem*> subSystems;
		subSystems.push_back(&s1);
		subSystems.push_back(&s2);
		SerialSystem system(subSystems);
		system.update(2);
		cout<<system.getOutput()<<endl;
	}

	if(arg == "diff"){
		vector<double> a = {1, 4, 5, 2};
		vector<double> b = {2, 1, 1, 2};
		DifferentialEquation s(a, b);
		s.setTimeVariation(0.1);
		double input = 1;
		Serial f("samples.dat");
		for(int i = 0; i < 100; i++){
			s.update(input);
			cout<<s.getOutput()<<endl;
			f.write("%f\n", s.getOutput());

		}
		f.fflush();
	}

	if(arg == "close"){
		vector<double> c = {0, 0, 1};
		vector<double> d = {1, 3, 4};
		DifferentialEquation s1(d, c);
		//DifferentialEquation s2(d, c);
		GainSystem g(2);
		double dt = 0.001;
		double time = 4;
		s1.setTimeVariation(dt);
		//s2.setTimeVariation(dt);

		double input = 1;
		Serial f1("temp1.dat");
		Serial f2("temp2.dat");
		CloseLoop close(&s1);
		vector<DynamicSystem*> sp = {&s1, &g};
		ParallelSystem p(sp);
		for(int i = 0; i < time/dt; i++){

			//s2.update(input);
			close.update(input);

			f1.write("%f\n", close.getOutput());
			//f2.write("%f\n", close.getOutput());
			//input = 0;
			//cout<<s.getOutput()<<endl;

		}
		f1.fflush();
		//f2.fflush();

		system("gnuplot -p -e \"plot './temp1.dat' with lines\"");
		//system("gnuplot -p -e \"plot './temp2.dat' with lines\"");
	}

	if(arg == "pid"){
		vector<double> c = {0, 0, 1};
		vector<double> d = {1, 3, 4};
		DifferentialEquation s1(d, c);
		//DifferentialEquation s2(d, c);
		GainSystem g(20);
		double dt = 1.0/30;
		double time = 4;
		s1.setTimeVariation(dt);
		//s2.setTimeVariation(dt);

		//input = 1;
		Serial f1("temp1.dat");
		Serial f2("temp2.dat");

		PID pid(kp, ki, kd);
		pid.setTimeVariation(dt);

		vector<DynamicSystem*> serialsystems = { &pid, &s1};
		SerialSystem serialSystem(serialsystems);

		CloseLoop close(&serialSystem);
		/*vector<DynamicSystem*> sp = {&s1, &g};
		ParallelSystem p(sp);*/
		for(int i = 0; i < (time/dt); i++){
			cout << (time/dt)<<endl;
			//s2.update(input);
			close.update(input);

			f1.write("%f\n", close.getOutput());
			//f2.write("%f\n", close.getOutput());
			//input = 0;
			//cout<<s.getOutput()<<endl;

		}
		f1.fflush();
		//f2.fflush();

		system("gnuplot -p -e \"plot './temp1.dat' with points\"");
		//system("gnuplot -p -e \"plot './temp2.dat' with lines\"");
	}

	if( arg == "release"){
			Camera* c;
		if(videoFile != "")
		 	c = new Camera(videoFile);
		 else
		 	c = new Camera(webcam);

		Serial s(sampleFile);
		Serial arduino(arduinoDevice, "w");
		arduino.fflush();
		BallanceBallPlant plant(*c, arduino);
		//PID pid(-70.6, -0.03, -1);
		PID pid(kp, ki, kd);
		vector<DynamicSystem*> serialSubsystems = {&pid, &plant};
		SerialSystem openLoop(serialSubsystems);
		CloseLoop closeLoop(&openLoop);

		cout << "kp: " << kp << " ki: " << ki << " kd: " << kd << endl;

		cout << "input: " << input << endl;

		long int lastTime;
		long int delta = 0;
		double time = 0;
		double initTime = currentTimeMillis();

	    while(true){
	    	lastTime = currentTimeMillis() - initTime;
	    	pid.setTimeVariation(delta);

	    	//cout << "BallanceBall: "<< plant.outputAvailable()<<endl;
	    	//cout << "Serial: " << openLoop.outputAvailable()<<endl;

	    	if(!closeLoop.outputAvailable()){
	    	    //cout << "closeLoop not working, set angle 90..." << endl;
	    		arduino.write("%d:\n", 90);
	    	}

		    if(!closeLoop.update(input))
		    	break;


			//cout<<(!s.getFail() && closeLoop.outputAvailable())<<endl;
		    if(!s.getFail() && closeLoop.outputAvailable()){
		       	s.write("%f %f\n", time, plant.getOutput());
		       	s.fflush();
		    }

		    delta = currentTimeMillis() - lastTime - initTime;
		    if(plant.outputAvailable() && delta < 300)
	    		time += (double) delta /1000;

	    }

	}
	if(arg == "sample"){
		Camera* c;
		if(videoFile != "")
		 	c = new Camera(videoFile);
		 else
		 	c = new Camera(webcam);

		Serial s(sampleFile, "w");
		Serial arduinoRead(arduinoDevice, "r");

		long int lastTime;
		long int delta = 0;
		double time = 0;
		double initTime = currentTimeMillis();

		int result;
	    while(true){
	    	lastTime = currentTimeMillis() - initTime;

	    	result = c->update();

	    	if(!result)
	    		break;

	    	if(c->getFail()){
	    		cout << "cam fail!: " << c->getErrorMessage() << endl;
	    		break;
	    	}

	    	int i;
	    	//fseek(arduinoRead.serial, -20, SEEK_END);
	    	arduinoRead.read("%d", &i);
	    	//arduinoRead.fflush();
	    	cout << i << endl;
	    	if(c->isLimitsSelected()){

			    if(!s.getFail()){
			      	s.write("%f %d %f\n", time, i,c->getPosition());
			      	s.fflush();
			    }

	    	}


		    delta = currentTimeMillis() - lastTime - initTime;
		    if(c->isLimitsSelected() && delta < 300)
	    		time += (double) delta /1000;
   		}

	}
	if(arg == "time"){
			Camera* c;
			if(videoFile != "")
			 	c = new Camera(videoFile);
			 else
			 	c = new Camera(webcam);

			Serial s(sampleFile, "w");
			Serial sRead(sampleFile, "r");
			Serial arduino(arduinoDevice);
			Serial arduinoRead(arduinoDevice, "r");
			BallanceBallPlant plant(*c, arduino);

			long int lastTime;
			long int delta = 0;
			double time = 0;
			double initTime = currentTimeMillis();
			FILE *f = fopen(arduinoDevice.c_str(), "r");

			//char sttyCommand[200];
			//sprintf(sttyCommand, "stty -F %s -icanon min 1 time 1", arduinoDevice.c_str());
			//system(sttyCommand);

		    double input;

		    while(true){
		    	lastTime = currentTimeMillis() - initTime;

		    	/*if(time < 1)
		    		input = 1;
		    	else if(time < 2)
		    		input  = -1;
		    	else if(time < 3)
		    		input = 0;
		    	else
		    		input = 1;

		    	input *= 5;
		    	input += 90;*/
		    	//if(plant.outputAvailable())
		    	//	input = 180 * (plant.getOutput() + 0.30);

			    if(!plant.update(input))
			    	break;

			    if(!s.getFail() && plant.outputAvailable()){
			      	int r = 2;
			      	char o[100];
			      	//s.fflush();
			      	//fseek(f, -6, SEEK_END);
			      	//fscanf(f, "%d", &r);
			      	//fflush(f);
			      	//sRead.read("%f", &r);
			      	//arduino.fflush();
			      	//usleep(100000);
			      	//arduinoRead.fflush();
			      	fseek(arduinoRead.serial, -20, SEEK_END);
			      	arduinoRead.read("%d", &r);
			      	//s.read("%f", &r);
			      	s.write("%d %f\n", r, plant.getOutput());
			      	cout << "readed: " << r << endl;
			    }

			    delta = currentTimeMillis() - lastTime - initTime;
			    if(plant.outputAvailable() && delta < 300)
		    		time += (double) delta /1000;
		    	//printf("time: %.3f  input: %.3f  delta: %u \n ", time, input, delta);
	   		}
	   		fclose(f);

    }

	if(arg == "servo"){
		Serial arduino(arduinoDevice);
		cout<< "servo"<<endl;
		int i = 3;
		//while(++i < 10){

			arduino.write("%d:\n", i * 20 % 180);
			//arduino.fflush();
			//usleep(200000);
			printf("%d:\n", i * 20 % 180 );
		//}
	}
	if(arg == "ioctl"){
		string arg;
		int angle = 90;
		for(int i = 0; i < argc; i++){
			arg = string(argv[i]);

			if(arg == "--angle")//video input
				angle = std::stoi(argv[++i]);
		}

		int USB = open( arduinoDevice.c_str(), O_RDWR| O_NOCTTY );

		struct termios tty;
		struct termios tty_old;
		memset (&tty, 0, sizeof tty);

		/* Error Handling */
		if ( tcgetattr ( USB, &tty ) != 0 ) {
		   std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
		}

		/* Save old tty parameters */
		tty_old = tty;

		/* Set Baud Rate */
		cfsetospeed (&tty, (speed_t)B9600);
		cfsetispeed (&tty, (speed_t)B9600);

		/* Setting other Port Stuff */
		tty.c_cflag     &=  ~PARENB;            // Make 8n1
		tty.c_cflag     &=  ~CSTOPB;
		tty.c_cflag     &=  ~CSIZE;
		tty.c_cflag     |=  CS8;

		tty.c_cflag     &=  ~CRTSCTS;           // no flow control
		tty.c_cc[VMIN]   =  1;                  // read doesn't block
		tty.c_cc[VTIME]  =  1;                  // 0.5 seconds read timeout
		tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

		/* Make raw */
		cfmakeraw(&tty);

		/* Flush Port, then applies attributes */
		tcflush( USB, TCIFLUSH );
		if ( tcsetattr ( USB, TCSANOW, &tty ) != 0) {
		   std::cout << "Error " << errno << " from tcsetattr" << std::endl;
		}
		ioctl( USB, TCSANOW, &tty );
		//int i = 70;
		//while((i += 5)  < 130){
			char cmd[10];
			sprintf(cmd, "%d:\n", angle);
			cout << cmd << endl;
			int n_written = 0,
			    spot = 0;

			do {
			    n_written = write( USB, &cmd[spot], 1 );
			    spot += n_written;
			} while (cmd[spot-1] != '\n' && n_written > 0);

			n_written = 0;
			    spot = 0;
			char r[100];
			do {
			    n_written = read( USB, &r[spot], 1 );
			    spot += n_written;
			} while (cmd[spot] != '\n' && n_written > 0);
			//read(USB, r, 9);
			r[spot+1] = '\0';
			cout << "read: " << r << endl;

			//fflush*
		//}
	}

	if(arg == "readFile"){
		FILE *file = fopen(sampleFile.c_str(), "w+");
		Serial s(sampleFile, "r");
		//s.write("%d\n", 30);
		//s.write("%d", 40);
		fprintf(file, "%d %d\n", 30, 40);
		fflush(file);
		int f=0, r=0;
		int f1 = 0, r1 = 0;
		s.read("%d %d", &r, &f);
		fseek( file , 0 ,SEEK_SET );
		fprintf(file, "%d\n", 77 );
		fscanf(file, "%d %d", &r1, &f1);
		cout << "r: " << r << "f: " << f << endl;
		cout << "r1: " << r1 << "f1: " << f1 << endl;
		fclose(file);
	}

	if(arg == "serialRead"){
		Serial arduino(arduinoDevice);
		int d = 0;
		arduino.read("%d", &d);
		printf("%d\n", d);
	}

	if(arg == "listCam"){
		//VideoCapture *capture = CaptureFromCAM( CV_CAP_DSHOW );
 		//capture->VI.listDevices();
	}
	if(arg == "cam"){
		Camera* c;
		if(videoFile != "")
		 	c = new Camera(videoFile);
		 else
		 	c = new Camera(webcam);

		 while(true){
		 	if(!c->update())
		 		break;
		 }
	}
	if(arg == "qt"){
		/*QApplication app(argc, argv);
		QLabel hello("<center>Welcome to my first WikiHow Qt program</center>");
		hello.setWindowTitle("My First WikiHow Qt Program");
		hello.resize(400, 400);
		hello.show();
		return app.exec();*/
	}
    return 0;
}

void showHelp(){
	cout<<"####### BallanceBall By Santana\n"<<endl;
	cout<<" options"<<endl;
	cout<<" -v \t\t\t Set video input. Usage 'app -v /path/videoFile'"<<endl;
	cout<<" -c \t\t\t Set webcam input. Usage 'app -c webcamNumber', example to choose default webcam 'app -c 0'"<<endl;
	cout<<" -s \t\t\t Set sample output file. Usage 'app -s /path/sampleFile'"<<endl;
	cout<<" -ns \t\t\t No save samples in any file. Usage 'app -ns'"<<endl;
	cout<<" -d \t\t\t Set arduino serial file. Usage 'app -d /path/serialFile'. Obs: '/dev/ttyUSB0' is default value of the serialFile."<<endl;
	cout<<" -h \t\t\t Show this help message."<<endl;
}

/*void ReadStuff (FILE * stream, const char * format, ...)
{
  va_list args;
  va_start (args, format);
  vfscanf (stream, format, args);
  va_end (args);
}*/