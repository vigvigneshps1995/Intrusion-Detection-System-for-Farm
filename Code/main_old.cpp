#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include "Data.h"
#include <unistd.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;

int ThresholdValue = 20;					//our sensitivity value to be used in the absdiff() function
int BlurValue = 250;						//
int morph_size = 7;							//Used for Image closing

double CameraViewWidth;
double CameraViewHeight;

/*bool detectBlur(Mat frame)
{
	int scale = 1;
  	int delta = 0;
  	int ddepth = CV_16S;
  	int variance;
  	
  	Scalar mean,stddev;
	Mat grad_x, grad_y;
	
	GaussianBlur( frame, frame, Size(3,3), 0, 0, BORDER_DEFAULT );
  	cvtColor( frame, frame, CV_BGR2GRAY );
  	Sobel( frame, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
  	convertScaleAbs( grad_x, grad_x );
  	Sobel( frame, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
  	convertScaleAbs( grad_y, grad_y );
  	addWeighted( grad_x, 0.5, grad_y, 0.5, 0, frame );
  	
  	meanStdDev(frame,mean,stddev);
  	variance = pow(stddev.val[0],2);
  	
  	cout << variance <<endl;
	
	if(variance <= BlurValue)
		return true;
	else
		return false;
}*/

bool detectBlur(Mat frame)
{
    Mat Gx, Gy;
    Sobel( frame, Gx, CV_32F, 1, 0 );
    Sobel( frame, Gy, CV_32F, 0, 1 );
    double normGx = norm( Gx );
    double normGy = norm( Gy );
    double sumSq = normGx * normGx + normGy * normGy;
    //cout << static_cast<float>( 1. / ( sumSq / src.size().area() + 1e-6 ));
    
    return true;
}

void detectMotion(Mat frame1,Mat frame2,Mat *fgmask)
{

	Mat gray1,gray2,diff,result,temp;
	Mat erode_mat,dilate_mat;
	
	cvtColor(frame1,gray1,COLOR_BGR2GRAY);
	cvtColor(frame2,gray2,COLOR_BGR2GRAY);
	
	//difference between frames 
	absdiff(gray1,gray2,diff);

	//post-process the frames
    threshold(diff,result,ThresholdValue,255,THRESH_BINARY);
    blur(result,result,Size(10,10));
    threshold(diff,result,ThresholdValue,255,THRESH_BINARY);
    Mat element = getStructuringElement( MORPH_RECT, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
    morphologyEx( result, result, MORPH_CLOSE, element );
        
    *fgmask = result;
}

void blobAnalysis(Mat motionmask,Mat *frame)
{

	bool objectDetected;
	double a,largestArea=0;
	int i,largestContour=0;
	
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	
	Rect objectBoundingRectangle = Rect(0,0,0,0);
	//Scalar color(0,255,255);
	
	//find contours of filtered image using openCV findContours function
	findContours(motionmask,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );		// retrieves all contours
	
	//if contours vector is not empty, we have found some objects
	if(contours.size()>0)
		objectDetected=true;
	else 
		objectDetected = false;
	
	//find the largest contour and draw a rectangle
	if(objectDetected)
	{
		for(i=0;i<contours.size();i++)
		{
			a = contourArea(contours[i],false);
			if(a > largestArea)
			{	
				largestArea = a;
				largestContour = i;
			}
		}
		
		//the rectangle or draw the conour itself
		//drawContours( *frame, contours, largestContour, Scalar(255), CV_FILLED, 8, hierarchy );
		objectBoundingRectangle = boundingRect( (Mat) contours[largestContour]);
		rectangle( *frame, objectBoundingRectangle.tl(), objectBoundingRectangle.br(), Scalar(0,0,255), 1, 8, 0 );
	}	
}



int main()
{

	Mat frame1,frame2;
	Mat motionmask;				
	int i;

	//Opeing a capturing device or video .to open video input the filename to open() function
	VideoCapture Source;
	Source.open(0);
	if(!Source.isOpened())
	{
		cout<<"ERROR ACQUIRING VIDEO FEED\n";
		return -1;
	}
	
	//Source.set(CV_CAP_PROP_FRAME_WIDTH,1280);
	//Source.set(CV_CAP_PROP_FRAME_HEIGHT,720);
	CameraViewWidth = Source.get(CV_CAP_PROP_FRAME_WIDTH );
	CameraViewHeight = Source.get(CV_CAP_PROP_FRAME_HEIGHT );
	
	// Create a window
  	namedWindow( "frame1", CV_WINDOW_AUTOSIZE );
  	namedWindow( "frame2", CV_WINDOW_AUTOSIZE );
		
  	for(;;)
  	{
			//read the frames
			Source.read(frame1);
			Source.read(frame2);
			
			detectBlur(frame1);
			
			/*while(detectBlur(frame1) || detectBlur(frame2))
			{
				Source.read(frame1);
				Source.read(frame2);
			}*/
			
			//detect motion
			detectMotion(frame1,frame2,&motionmask);
			blobAnalysis(motionmask,&frame2);		
			
			imshow("frame1", motionmask);
			imshow("frame2", frame2);
			
			if(waitKey(1) >= 0) 
				return 0; 
	}
	
	Source.release();
	return 0;
}
