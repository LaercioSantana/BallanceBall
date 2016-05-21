#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>
#include <limits>
#include <math.h>
#include <stdlib.h>
#include <unistd.h> 
#include <cstdio>

#include "Util.hpp"


#define DISTANCE 1
#define FORM 2
#define DISTANCE_AND_FORM 3
#define PI 3.14159265

using namespace cv;
using namespace std;

class Camera{
public:
	Camera(const string pathVideo, const string pathOutputData);
	Camera(int webcamCode, const string pathOutputData);
	Point getPosition();
	int getFail();
	string getErrorMessage();
	int update();
private:
	VideoCapture *cap; //capture the video from
	string pathOutputData;

	int fail;
	string errorMessage;

	Scalar colorsRadius;
	Scalar colorSelected;
	vector<Scalar> limitsColors;
	long int frameCount;
	long int lastTime;
	long int frameDuration;
	long int sampleCount;
	
	//for comparation
	Point lastPoint;
	vector<Point> lastPointLimits;
	double lastArea;
	vector<Point> object;

	double realR;
	char buffer[30];

	Mat imgLines;

	void initVariables();
	vector<Scalar> getColorRangeHSV(Scalar color, Scalar colorsRadius);
	void setColorByPixel(Vec3b pixHSV, Scalar& color);
	void selectObject(Mat source, vector<Point>& object,Point p, Scalar& colorSelected);
	void onMouse( int event, int x, int y);
	int showcontours(Mat image);
	void drawContour(Mat img, vector<Point> contour, Scalar externalColor);
	Point getCenter(vector<Point> contour);
	vector<Point> getContour(Mat image, Point p, int comparationType );
	vector< vector<Point> > getContours(Mat image, int quant);
	void getNewCoor(Point l1, Point l2, Mat& o, Mat& rotation);
	void selectedColorHSV(Mat sourceHSV, Mat& destination, Scalar color);
	void setErrorMessage(const string errorMessage);
	static void onMouseStatic(int event, int x, int y, int, void* userdata);
};