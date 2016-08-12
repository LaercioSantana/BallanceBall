#include "Camera.hpp"

Camera::Camera(const string pathVideo){

    cap = new VideoCapture(pathVideo);

    fail = 0;
    if ( !cap->isOpened() )  // if not success, exit program
    {
    	fail = 1;
        setErrorMessage("Cannot open video file");
        return;
    }

    initVariables();
}

Camera::Camera(int webcamCode){
    cap = new VideoCapture(webcamCode);
    cap->set(CAP_PROP_FRAME_WIDTH,854);
    cap->set(CAP_PROP_FRAME_HEIGHT,480);

    fail = 0;
    if ( !cap->isOpened() )  // if not success, exit program
    {
    	fail = 1;
        setErrorMessage("Cannot open the web cam");
        return;
    }

    initVariables();
}
#ifdef DEBUG
void
Camera::onTrackDebug(int p, void* data){
  double* colorsRadius = (double*) data;
  *colorsRadius = p;
}
#endif
void
Camera::initVariables(){
    colorsRadius = Scalar(15,60,60);
    colorSelected = Scalar(108,162,151);

    frameCount = 0;
    sampleCount = 0;
    lastPoint = Point(-1,-1);
    lastArea = -1;

    realR = 0.6;
    xReal = 0;

    Mat imgSample; 
    readImg(imgSample);
    imgOriginal = imgSample;

    //init windows for improves speed and fix delay in first update call
    imshow("Original", imgSample); //show the original image
    #ifdef DEBUG
    imshow("Thresholded Image", imgSample); //show the thresholded image
    createTrackbar("H radius", "Original", NULL, 179, Camera::onTrackDebug, (void*) &colorsRadius[0]);
    setTrackbarPos("H radius", "Original", (int) colorsRadius[0]);
    createTrackbar("S radius", "Original", NULL, 255, Camera::onTrackDebug, (void*) &colorsRadius[1]);
    setTrackbarPos("S radius", "Original", (int) colorsRadius[1]);
    createTrackbar("V radius", "Original", NULL, 255, Camera::onTrackDebug, (void*) &colorsRadius[2]);
    setTrackbarPos("V radius", "Original", (int) colorsRadius[2]);
    #endif

    pMOG2 = createBackgroundSubtractorMOG2();
    pMOG2->apply(imgSample, imgSample, 1);
}

vector<Scalar>
Camera::getColorRangeHSV(const Scalar& color, const Scalar& colorsRadius){
    Scalar low(0,0,0,0), high(0,0,0,0);

    low[0] = color[0] - colorsRadius.val[0] > 0 ? color[0] - colorsRadius.val[0] : 0 ;
    high[0] = color[0] + colorsRadius.val[0] < 179 ? color[0] + colorsRadius.val[0] : 179;

    low[1] = color[1] - colorsRadius.val[1] > 0 ? color[1] - colorsRadius.val[1] : 0 ;
    high[1] = color[1] + colorsRadius.val[1] < 255 ? color[1] + colorsRadius.val[1] : 255;

    low[2] = color[2] - colorsRadius.val[2] > 0 ? color[2] - colorsRadius.val[2] : 0 ;
    high[2] = color[2] + colorsRadius.val[2] < 255 ? color[2] + colorsRadius.val[2] : 255;

    vector<Scalar> range;
    range.push_back(low);
    range.push_back(high);

    return range;
}

void
Camera::setColorByPixel(Vec3b pixHSV, Scalar& color){

    int H = pixHSV.val[0];
    int S = pixHSV.val[1];
    int V = pixHSV.val[2];

    color = Scalar(H, S, V);

    #ifdef DEBUG
    cout<<"color has selected <= "<<color<<endl;
    #endif
}

void
Camera::selectObject(const Mat& source, vector<Point>& object,Point p, Scalar& colorSelected){
   Mat imgHSV;
   cvtColor(source, imgHSV, COLOR_BGR2HSV);

   Vec3b pixHSV = imgHSV.at<Vec3b>(p.y,p.x);
   setColorByPixel(pixHSV, colorSelected);

   Mat imgThresholded;
   selectedColorHSV(imgHSV, imgThresholded, colorSelected);
   object =  getContour(imgThresholded, p, DISTANCE);
}

void
Camera::onMouse( int event, int x, int y){

    if( event != CV_EVENT_LBUTTONDOWN && event != CV_EVENT_RBUTTONDOWN)
        return;

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

int
Camera::showcontours(const Mat& image){

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

    return 0;
}

void
Camera::drawContour(Mat img, vector<Point> contour, Scalar externalColor){
    vector<std::vector<cv::Point> > contourVec;
    contourVec.push_back(contour);

    drawContours(img,contourVec,0, externalColor);
}

Point
Camera::getCenter(const vector<Point>& contour){
    Moments oMoments = moments(contour, false);

    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;

    return Point(dM10 / dArea, dM01 / dArea);
}

vector<Point>
Camera::getContour(Mat image, Point p, int comparationType){
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

vector< vector<Point> >
Camera::getContours(Mat image, int quant){
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat mask = Mat::zeros( image.size(), CV_8U ) + 255;
    Point p(0,0);

    Mat d;

    for(int i = 0; i < quant; i++){
        contours.push_back(getContour(image.clone(), p, FORM));
        drawContours(mask, contours, i, Scalar(0, 0, 0), CV_FILLED);

        bitwise_and(image, image, d, mask);
        image = d.clone();
    }

    return contours;
}

void
Camera::getNewCoor(Point l1, Point l2, Mat& o, Mat& rotation){
    Point r = l2 - l1;
    o = (Mat(l1) + Mat(l2))/2;
    double theta = atan2 (r.y,r.x);
    rotation = (Mat_<double>(2,2) << cos(theta), sin(theta), -sin(theta), cos(theta));
}

void
Camera::selectedColorHSV(const Mat& sourceHSV, Mat& destination, const Scalar color){
    vector<Scalar> range = getColorRangeHSV(color, colorsRadius);

    Mat imgThresholded;
    inRange(sourceHSV, range[0], range[1], imgThresholded); //Threshold the image

    //morphological opening (removes small objects from the foreground)
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(8, 8)) );
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    //morphological closing (removes small holes from the foreground)
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(8, 8)) );
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    destination = imgThresholded;
}

int
Camera::update(){

        frameCount++;
        lastTime = currentTimeMillis();

        bool bSuccess = readImg(imgOriginal); // read a new frame from video

        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            return 0;//break;
        }

        imgLines = Mat::zeros( imgOriginal.size(), CV_8UC3 );

        Mat imgHSV;
        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

        //find and draws limits
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

                    if(isLimitsSelected()){
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

                        circle(imgLines, Point(o) , imgOriginal.cols/150, Scalar(0,255,0), -1, 4 , 0);
                        xReal = x * (realR/r.x);
                    }

                    drawContour( imgLines, contour, Scalar(0,0,255));
                    circle(imgLines, center, imgOriginal.cols/150, Scalar(0,0,255), -1, 4 , 0);
                }

                lastPoint = center;
            }
        }

#ifdef DEBUG
        imshow("Thresholded Image", imgThresholded); //show the thresholded image
#endif
        char str[10];
        sprintf(str,"FPS: %0.1f ", (double) 1000/frameDuration);
        putText(imgLines, str, Point(50,50), FONT_HERSHEY_SIMPLEX, 1,  Scalar(255,0,0), 3);

        imgOriginal = imgOriginal + imgLines;
        imshow("Original", imgOriginal); //show the original image
        setMouseCallback("Original", onMouseStatic, this );

        //force fps
        frameDuration = currentTimeMillis()-lastTime;
        double correctFrameTime =  1/cap->get(CV_CAP_PROP_FPS);
        int key = (correctFrameTime*1000 -  frameDuration) > 1 ?  waitKey( correctFrameTime * 1000 - frameDuration) : waitKey(1);

        if(key == 27){
            cout << "esc key is pressed by user" << endl;
            return 0;
        }
        else if (key == 32){
            waitKey(0);
        }
        else if(key == 'b'){
            updateBackgroundModel();
        }

        #ifdef DEBUG_BACKGROUND
        Mat unbackgrounded;
        readImg(unbackgrounded, true, -1);
        erode(unbackgrounded, unbackgrounded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
        dilate( unbackgrounded, unbackgrounded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
        vector<Point> backContour = getContour(unbackgrounded & imgThresholded, lastPoint, DISTANCE);
        drawContour( imgOriginal, backContour, Scalar(0,255,0));
        imshow("background", unbackgrounded);
        #endif

        frameDuration = currentTimeMillis() - lastTime;

        return 1;
}

void
Camera::setErrorMessage(const string errorMessage){
	this->errorMessage = errorMessage;
}

int
Camera::getFail(){
	return fail;
}

string
Camera::getErrorMessage(){
	return errorMessage;
}

void
Camera::onMouseStatic(int event, int x, int y, int, void* userdata){
    Camera* camera = reinterpret_cast<Camera*>(userdata);
    camera->onMouse(event, x, y);
}

double Camera::getPosition(){
    return xReal;
}

bool
Camera::isLimitsSelected(){
    return lastPointLimits.size() >= 2;
}

bool
Camera::readImg(Mat& mat, bool removeBackground, double rate){
    bool success = cap->read(mat);

    if(removeBackground && success)
        pMOG2->apply(mat, mat, rate);

    return success;
}

void
Camera::updateBackgroundModel(double rate){
    Mat img;
    cap->read(img);
    pMOG2->apply(img, img, rate);
}