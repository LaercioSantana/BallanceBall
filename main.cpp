#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <sys/time.h>
#include <stdio.h>
#include <limits>
#include <math.h>
#include <stdlib.h>
#include <unistd.h> 


#define DISTANCE 1
#define FORM 2
#define DISTANCE_AND_FORM 3
#define PI 3.14159265

using namespace cv;
using namespace std;

vector<Point> getContour(Mat image, Point p, int comparationType);
void selectedColorHSV(Mat sourceHSV, Mat& destination, Scalar color);
void drawContour(Mat img, vector<Point> contour, Scalar externalColor);

VideoCapture *cap; //capture the video from

FILE *file;

Scalar colorsRadius(15,60,60);
Scalar colorSelected(108,162,151);
vector<Scalar> limitsColors;

long int frameCount = 0;
long int lastTime;
long int frameDuration;
long int sampleCount = 0;

//variables for comparation
Point lastPoint(-1,-1);
vector<Point> lastPointLimits;
double lastArea = -1;
vector<Point> object;

double realR = 0.6;

char buffer[30];

long int currentTimeMillis(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

vector<Scalar> getColorRangeHSV(Scalar color, Scalar colorsRadius){
    Scalar low(0,0,0,0), high(0,0,0,0);

    low[0] = color[0] - colorsRadius.val[0] > 0 ? color[0] - colorsRadius.val[0] : 0 ;
    high[0] = color[0] + colorsRadius.val[0] < 179 ? color[0] + 10 : 179;

    low[1] = color[1] - colorsRadius.val[1] > 0 ? color[1] - colorsRadius.val[1] : 0 ;
    high[1] = color[1] + colorsRadius.val[1] < 255 ? color[1] + colorsRadius.val[1] : 255;

    low[2] = color[2] - colorsRadius.val[2] > 0 ? color[2] - colorsRadius.val[2] : 0 ;
    high[2] = color[2] + colorsRadius.val[2] < 255 ? color[2] + colorsRadius.val[2] : 255;

    vector<Scalar> range;
    range.push_back(low);
    range.push_back(high);
    
    return range;
}

void setColorByPixel(Vec3b pixHSV, Scalar& color){

    int H = pixHSV.val[0];
    int S = pixHSV.val[1];
    int V = pixHSV.val[2];

    color = Scalar(H, S, V);

    //cout<<"color has selected <= "<<color<<endl; 
}
void selectObject(Mat source, vector<Point>& object,Point p, Scalar& colorSelected){
   Mat imgHSV;
   cvtColor(source, imgHSV, COLOR_BGR2HSV);

   Vec3b pixHSV = imgHSV.at<Vec3b>(p.y,p.x);
   setColorByPixel(pixHSV, colorSelected); 

   Mat imgThresholded;
   selectedColorHSV(imgHSV, imgThresholded, colorSelected);
   object =  getContour(imgThresholded, p, DISTANCE);
}

static void onMouse( int event, int x, int y, int f, void* ){

    if( event != CV_EVENT_LBUTTONDOWN && event != CV_EVENT_RBUTTONDOWN)
        return;

    

    Mat imgOriginal;
    cap->read(imgOriginal);


    if(event == CV_EVENT_LBUTTONDOWN){
        lastPoint = Point(x,y);
        selectObject(imgOriginal, object, Point(x,y), colorSelected);
    } 
    else if(event == CV_EVENT_RBUTTONDOWN){
        vector<Point> limit;
        lastPointLimits.push_back(Point(x,y));
        Scalar color;
        selectObject(imgOriginal, limit, Point(x,y), color);
        limitsColors.push_back(color);  
    } 
      
}
int showcontours(Mat image){

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
 
    findContours( image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    /// Draw contours
    Mat drawing = Mat::zeros( image.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {

        Moments oMoments = moments(contours[i], false);

        double dM01 = oMoments.m01;
        double dM10 = oMoments.m10;
        double dArea = oMoments.m00;

        int posX = dM10 / dArea;
        int posY = dM01 / dArea;

        Scalar color = Scalar(255,255,255);
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }     

    //imshow("Result window", drawing );                                         
    return 0;
}
void drawContour(Mat img, vector<Point> contour, Scalar externalColor){
    vector<std::vector<cv::Point> > contourVec;
    contourVec.push_back(contour);

    drawContours(img,contourVec,0, externalColor);
}
Point getCenter(vector<Point> contour){
    Moments oMoments = moments(contour, false);

    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;

    return Point(dM10 / dArea, dM01 / dArea);
}
vector<Point> getContour(Mat image, Point p, int comparationType ){
    int contourSelected = 0;
    double distanceSelected = numeric_limits<double>::infinity();
    
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( image, contours, hierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    if(contours.size() < 1){
        return vector<Point>();
    }

    for( int i = 0; i< contours.size(); i++ )
    {
        if(comparationType == DISTANCE){
            Point contourPosition;
            contourPosition = getCenter(contours[i]);
            double distance = norm(p - contourPosition); 
            if( norm(distance) < norm(distanceSelected) ){
                distanceSelected = distance;
                contourSelected = i;
            }
        }
        if(comparationType == FORM){
            double distance = matchShapes(object, contours[i], CV_CONTOURS_MATCH_I1, 0);
            if( distance < distanceSelected ){
                distanceSelected = distance;
                contourSelected = i;
            }            
        }
    }     

    return contours[contourSelected];
}
vector< vector<Point> > getContours(Mat image, int quant){
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat mask = Mat::zeros( image.size(), CV_8U ) + 255;
    Point p(0,0);

    Mat d;// = Mat::zeros(image.size(), CV_8U);

    for(int i = 0; i < quant; i++){
        contours.push_back(getContour(image.clone(), p, FORM));
        drawContours(mask, contours, i, Scalar(0, 0, 0), CV_FILLED);

        bitwise_and(image, image, d, mask);
        image = d.clone();        
    }

    return contours;
}

void getNewCoor(Point l1, Point l2, Mat& o, Mat& rotation){
    Point r = l2 - l1;
    o = (Mat(l1) + Mat(l2))/2;
    double theta = atan2 (r.y,r.x);
    rotation = (Mat_<double>(2,2) << cos(theta), sin(theta), -sin(theta), cos(theta));
}
void selectedColorHSV(Mat sourceHSV, Mat& destination, Scalar color){
    vector<Scalar> range;
    range = getColorRangeHSV(color, colorsRadius);

    Mat imgThresholded;
    inRange(sourceHSV, range[0], range[1], imgThresholded); //Threshold the image

    //morphological opening (removes small objects from the foreground)
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    //morphological closing (removes small holes from the foreground)
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    destination = imgThresholded;
}
int main( int argc, char** argv ){

    
    file = fopen("/dev/ttyUSB0","w");  //Opening device file
    
    if(argc == 2)
        cap = new VideoCapture(argv[1]);
    else
        cap = new VideoCapture(0);

    if ( !cap->isOpened() )  // if not success, exit program
    {
        cout << "Cannot open the web cam" << endl;
        return -1;
    }

    system("echo > temp.dat");

    //Capture a temporary image from the camera
    Mat imgTmp;
    cap->read(imgTmp);

    //Create a black image with the size as the camera output
    Mat imgLines;

    while (true)
    {
        frameCount++;
        lastTime = currentTimeMillis();

        Mat imgOriginal;
        bool bSuccess = cap->read(imgOriginal); // read a new frame from video


        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }

        imgLines = Mat::zeros( imgOriginal.size(), CV_8UC3 );
       
        Mat imgHSV;
        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
 
/////////////////////////////////////////////////////////////////////////////////
        if(limitsColors.size() > 0){

            Mat colorLimitSelected;
            vector<Point> contour;
            Point center;
            vector<Point> limitsNULL;
            double radius = imgOriginal.size().width/15;
            for(int i = 0; i < limitsColors.size() && i < 2; i++){

                selectedColorHSV(imgHSV, colorLimitSelected, limitsColors[i]);
                contour = getContour(colorLimitSelected, lastPointLimits[i], DISTANCE);
                center = getCenter(contour);

                if(norm(center - lastPointLimits[i]) < radius){
                    lastPointLimits[i] = center;
                    //selectObject(imgOriginal, limitsNULL, center, limitsColors[i]);
                }
               
                circle(imgLines, lastPointLimits[i], imgOriginal.cols/150, Scalar(255,0,0), -1, 4 , 0);
            }            
        }

/////////////////////////////////////////////////////////////////////////////////
        Mat imgThresholded;
        selectedColorHSV(imgHSV, imgThresholded, colorSelected);
        vector<Point> contour = getContour(imgThresholded.clone(), lastPoint, DISTANCE);

        if(contour.size() != 0){

            Moments oMoments = moments(contour, false);
            double dArea = oMoments.m00;
            if (dArea > 1)
            {
                
                Point center = getCenter(contour);

                if (lastPoint.x >= 0 && lastPoint.y >= 0 && center.x >= 0 && center.y >= 0)
                {
                                        
                    if(lastPointLimits.size() >= 2){
                        Mat o, rotation;
                        getNewCoor(lastPointLimits[0], lastPointLimits[1], o, rotation);

                        Mat pnew = (Mat_<int>(2,1) << center.x, center.y);
                        pnew = (pnew - o);
                        pnew.convertTo(pnew, CV_64F);
                        pnew = rotation * pnew;
                        double x = pnew.at<double>(0,0);
                        
                        //write
                        Point r = lastPointLimits[1] - lastPointLimits[0];
                        char str[30];
                        sprintf(str,"x: %0.3f m", x * (realR/r.x));
                        putText(imgLines, str, center - Point(100,70), FONT_HERSHEY_SIMPLEX, 1,  Scalar(0,0,255), 3);

                        circle(imgLines, Point(o) , imgTmp.cols/150, Scalar(0,255,0), -1, 4 , 0);
                        double xReal = x * (realR/r.x);
                        sprintf(buffer, "echo %ld %f >> temp.dat", sampleCount++, xReal );
                        system(buffer);
                        
                        int angle = (int) ((xReal+0.3)*180/0.6);
                        fprintf(file,"%d:%d$\n",angle, angle); //Writing to the 
                        //printf("%d:%d$\n",angle, angle);

                    }
                    //if(norm(lastPoint - center) < 20)
                   //     selectObject(imgOriginal, object, center, colorSelected);
                    drawContour( imgLines, contour, Scalar(0,0,255));
                    circle(imgLines, center, imgTmp.cols/150, Scalar(0,0,255), -1, 4 , 0);
                }

                lastPoint = center;
            }
        }

        imshow("Thresholded Image", imgThresholded); //show the thresholded image

        imgOriginal = imgOriginal + imgLines;
        imshow("Original", imgOriginal); //show the original image
        setMouseCallback("Original", onMouse, 0 );
        
        int key = waitKey(10);
        if ( key == 27) 
        {
            cout << "esc key is pressed by user" << endl;
            break;   
        }
        else if (key == 32) 
        {
            waitKey(0);
           // break;   
        }

        //time 
        frameDuration = currentTimeMillis()-lastTime;


        //cout<<"FPS: "<<((double) 1000/frameDuration)<<endl;
        
    }
    fclose(file);
    return 0;
}


