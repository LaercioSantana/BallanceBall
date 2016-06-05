#include <math.h>
#include <unistd.h>
#include <iostream> 
#include <string>


#include "BallanceBallPlant.hpp"
#include "SerialSystem.hpp"
#include "GainSystem.hpp"
#include "DifferentialEquation.hpp"
#include "CloseLoop.hpp"

#define PI 3.14159265
#define DEBUG

void showHelp();

//configuration variables
string videoFile = "";
int webcam = 0;
string sampleFile = "temp.dat";
bool debug = false;
string arduinoDevice = "/dev/ttyUSB0";
int i = 0;

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
	}

	Camera* c;
	if(videoFile != "")
	 	c = new Camera(videoFile);
	 else
	 	c = new Camera(webcam);
    
	Serial s(sampleFile);
	Serial arduino(arduinoDevice);
	BallanceBallPlant plant(*c, arduino);

    while(true){
    	i++;
	    if(!plant.update(i % 180))
	    	break;

	    if(!s.getFail() && plant.outputAvailable())
	        	s.write("%f\n", plant.getOutput());

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