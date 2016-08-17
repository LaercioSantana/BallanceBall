#ifndef CAMERA_H
#define  CAMERA_H
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/video.hpp>
#include <opencv2/video/background_segm.hpp>
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
#define WITH_HELPER 4

//#define DEBUG
//#define DEBUG_BACKGROUND

using namespace cv;
using namespace std;

class Camera{
public:
	Camera(const string pathVideo);
	Camera(int webcamCode);
	double getPosition();
	int getFail();
	string getErrorMessage();
	int update();
	bool isLimitsSelected();
	static void onTrackDebug(int p, void* data);
	Scalar getColorSelected();
	vector<Scalar> getColorsLimits();
private:
	VideoCapture *cap; //capture the video from

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

	double xReal;

	double realR;
	char buffer[30];

	Mat imgLines;
	Mat imgOriginal;

	Ptr<BackgroundSubtractor> pMOG2;

	double positionXInfo;
	double fpsInfo;

	void initVariables();
	vector<Scalar> getColorRangeHSV(const Scalar& color, const Scalar& colorsRadius);
	void setColorByPixel(Vec3b pixHSV, Scalar& color);
	void selectObject(const Mat& source, vector<Point>& object,Point p, Scalar& colorSelected);
	void onMouse( int event, int x, int y);
	int showcontours(const Mat& image);
	void drawContour(Mat img, vector<Point> contour, Scalar externalColor);
	Point getCenter(const vector<Point>& contour);
	vector<Point> getContour(Mat image, Point p, int comparationType);
	vector< vector<Point> > getContours(Mat image, int quant);
	void getNewCoor(Point l1, Point l2, Mat& o, Mat& rotation);
	void selectedColorHSV(const Mat& sourceHSV, Mat& destination, Scalar color);
	void setErrorMessage(const string errorMessage);
	static void onMouseStatic(int event, int x, int y, int, void* userdata);
	bool readImg(Mat& mat, bool removeBackground=false, double rate=0);
	bool removeBackground(Mat& img, const Mat& background);
	void updateBackgroundModel(double rate=1);
	void drawInfo(Mat& img, double fps, double positionX);
};
#endif