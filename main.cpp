#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <sys/time.h>
#include <stdio.h>
#include <limits>

using namespace cv;
using namespace std;


VideoCapture *cap; //capture the video from
int iLowH = 95;
int iHighH = 115;

int iLowS = 150;
int iHighS = 255;

int iLowV = 60;
int iHighV = 255;

Scalar colorsRadius(10,50,50);

long int frameCount = 0;
long int lastTime;
long int frameDuration;
long int sampleCount = 0;

Point lastPoint(-1,-1);

char buffer[30];

long int currentTimeMillis(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

void setColorByPixel(Vec3b pixHSV){

    int H = pixHSV.val[0];
    int S = pixHSV.val[1];
    int V = pixHSV.val[2];
    //cout<<H<<":"<<S<<":"<<V<<endl;

    iLowH = H - colorsRadius.val[0] > 0 ? H - colorsRadius.val[0] : 0 ;
    iHighH = H + colorsRadius.val[0] < 179 ? H + 10 : 179;

    iLowS = S - colorsRadius.val[1] > 0 ? S - colorsRadius.val[1] : 0 ;
    iHighS = S + colorsRadius.val[1] < 255 ? S + colorsRadius.val[1] : 255;

    iLowV = V - colorsRadius.val[2] > 0 ? V - colorsRadius.val[2] : 0 ;
    iHighV = V + colorsRadius.val[2] < 255 ? V + colorsRadius.val[2] : 255;

    //cout<<"color has selected <= "<<H<<":"<<S<<":"<<V<<endl; 
    //cout<<iLowH<<":"<<iHighH<<endl;
    //cout<<iLowS<<":"<<iHighS<<endl;
    //cout<<iLowV<<":"<<iHighV<<endl;
}

static void onMouse( int event, int x, int y, int f, void* ){

    if( event != CV_EVENT_LBUTTONDOWN )
        return;

    Mat imgOriginal;
    cap->read(imgOriginal);

    Mat imgHSV;
    cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);
    Vec3b pixHSV =imgHSV.at<Vec3b>(y,x);

    setColorByPixel(pixHSV);

    lastPoint = Point(x,y);
      
}
int showcontours(Mat image){
    //image = imread("shape.JPG", 1);  
    //namedWindow( windowsName, CV_WINDOW_AUTOSIZE );  
    //imshow( "Display window", image );
    
    //Mat gray;
    //cvtColor(image, gray, CV_BGR2GRAY);
    //Canny(gray, gray, 100, 200, 3);
    /// Find contours   
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //RNG rng(12345);
    findContours( image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    //cout<<"contours size:"<<contours.size()<<endl;
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

        //cout << "area:" << dArea<<endl;
        //cout << "x:" << posX << "  y:" << posY << endl;
        //Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        Scalar color = Scalar(255,255,255);
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }     

   // imshow("lsakd", image);
    imshow("Result window", drawing );                                         
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
vector<Point> getContour(Mat image, Point p){
    int contourSelected = 0;
    double distanceSelected = numeric_limits<double>::infinity();
    
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
   // cout<<"contours size:"<<contours.size()<<endl;

    if(contours.size() < 1){
        return vector<Point>();
    }

    for( int i = 0; i< contours.size(); i++ )
    {
        Point contourPosition;
        contourPosition = getCenter(contours[i]);
        double distance = norm(p - contourPosition); 
        if( norm(distance) < norm(distanceSelected) ){
            distanceSelected = distance;
            contourSelected = i;
        }

        //cout << "area:" << dArea<<endl;
        //cout << "x:" << posX << "  y:" << posY << endl;
    }     

    //cout<<"selected:"<< contourSelected<<endl;                                      
    return contours[contourSelected];
}
int main( int argc, char** argv ){
    if(argc == 2)
        cap = new VideoCapture(argv[1]);
    else
        cap = new VideoCapture(0);

    if ( !cap->isOpened() )  // if not success, exit program
    {
        cout << "Cannot open the web cam" << endl;
        return -1;
    }

    namedWindow("Control", WINDOW_NORMAL); //create a window called "Control"
    //namedWindow("Show contours", WINDOW_NORMAL);

    system("echo > temp");
    //Create trackbars in "Control" window
    // createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
    // createTrackbar("HighH", "Control", &iHighH, 179);

    // createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
    // createTrackbar("HighS", "Control", &iHighS, 255);

    // createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
    // createTrackbar("HighV", "Control", &iHighV, 255);

    //int iLastX = -1;
    //int iLastY = -1;


    //Capture a temporary image from the camera
    Mat imgTmp;
    cap->read(imgTmp);

    //Create a black image with the size as the camera output
    Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );

    while (true)
    {
        lastTime = currentTimeMillis();

        Mat imgOriginal;
        bool bSuccess = cap->read(imgOriginal); // read a new frame from video

       // cout<<"loop"<<endl;

        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }


        Mat imgHSV;

        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

        Mat imgThresholded;

        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

        //morphological opening (removes small objects from the foreground)
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        //morphological closing (removes small holes from the foreground)
        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        //Calculate the moments of the thresholded image
        //Moments oMoments = moments(imgThresholded);

        //double dM01 = oMoments.m01;
        //double dM10 = oMoments.m10;
        //double dArea = oMoments.m00;
        vector<Point> contour = getContour(imgThresholded.clone(), lastPoint);

        if(contour.size() != 0){

            //drawContours( imgOriginal, contour, i, color, 2, 8, hierarchy, 0, Point() );
            //cout<<"saasa"<<endl;
            Moments oMoments = moments(contour, false);
            double dArea = oMoments.m00;
            //cout << "dArea: " << dArea << endl;
            // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero
            if (dArea > 1)
            {
                //calculate the position of the ball
               // int posX = dM10 / dArea;
                //int posY = dM01 / dArea;
                
                Point center = getCenter(contour);

                //cout << "contourSelected: " << center << endl;
                if (lastPoint.x >= 0 && lastPoint.y >= 0 && center.x >= 0 && center.y >= 0)
                {
                    imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );
                    //Draw a red line from the previous point to the current point
                    //set new color
                    
                    //////Vec3b pixHSV =imgHSV.at<Vec3b>(center.y,center.x);
                    //////setColorByPixel(pixHSV);
                    sprintf(buffer, "echo %ld %d >> temp", sampleCount++, center.x );
                    system(buffer);

                    
                    
                     drawContour( imgLines, contour, Scalar(0,0,255));
                    circle(imgLines, center, imgTmp.cols/150, Scalar(0,0,255), -1, 4 , 0);
                }

                //cout<<"lastPoint:"<<center<<endl;
                lastPoint = center;
            }
        }

        imshow("Thresholded Image", imgThresholded); //show the thresholded image
        //imshow("Resda", imgThresholded); //show the thresholded image
        //showcontours(imgThresholded);

        imgOriginal = imgOriginal + imgLines;
        imshow("Original", imgOriginal); //show the original image
        setMouseCallback("Original", onMouse, 0 );
        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }

        //time 
        frameDuration = currentTimeMillis()-lastTime;
        //cout<<"FPS:"<<1000/((double) frameDuration)<<endl;
        //cout<<cap->get(CAP_PROP_FPS)<<endl;
        lastTime = currentTimeMillis();

        frameCount++;
        
    }

    return 0;
}



