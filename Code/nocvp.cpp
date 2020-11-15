#include <math.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/video/background_segm.hpp"

#include "Pipe.h"
#include "Data.h"

using namespace std;
using namespace cv;

struct MessagePacket C;
VideoCapture Source;

double CameraViewWidth,CameraViewHeight;    												//the lenght and width of the frame
float CenteroidX1=0,CenteroidY1=0;															//intrusion centeroid in first processing	
float CenteroidX2=0,CenteroidY2=0;															//intrusion centeroid in second processing

void pipeInitialize()
{
	N = 2;
	strcpy(PipeNames[0],"CAMPIPEW");
	strcpy(PipePermission[0],"W");
	
	strcpy(PipeNames[1],"CAMPIPER");
	strcpy(PipePermission[1],"R");

	openPipes();
}

void sendCameraSensorc(CameraSensor *c,int i)
{
	char x[100];
	char temp[10];
		
	setMessagePacket(&C,"","","","");
	strcpy(x,"");
	sprintf(temp,"%d",i);
	strcat(x,temp);
	strcat(x," ");
	strcat(x,c -> clientId);
	strcat(x," ");
	sprintf(temp,"%d",c -> positionX);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",c -> positionY);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",c -> sensorColor);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",c -> used);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",c -> cameraNumber);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",c -> tracking);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",c -> sensorPan);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",c -> sensorTilt);
	strcat(x,temp);
	setMessagePacket(&C,"",x,"","CAMERASENSOR");
	sendMessagePacket("CAMPIPEW",&C);
}

void sendDistance(int a,int b)
{
	char x[100];
	char temp[10];
		
	setMessagePacket(&C,"","","","");
	strcpy(x,"");
	sprintf(temp,"%d",a);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",b);
	strcat(x,temp);
	setMessagePacket(&C,"",x,"","DISTANCE");
	sendMessagePacket("CAMPIPEW",&C);
}

int openCamera(int Camera)
{
	//setting camera properties
	//Source.set(CV_CAP_PROP_FRAME_WIDTH,1280);
	//Source.set(CV_CAP_PROP_FRAME_HEIGHT,720);	

	//Opeing a capturing device or video .to open video input the filename to open() function
	Source.open(Camera);
	if(!Source.isOpened())
	{
		cout<<"ERROR ACQUIRING VIDEO FEED" << endl;
		return -1;
	}
}

void readFrames(Mat &f1,Mat &f2,Mat &f3,Mat &f4,int c)
{

	Mat temp1,temp2,temp3,temp4;
	
	openCamera(c);
	Source.read(temp1);
	Source.release();
	usleep(FrameSkipTime);
	f1 = temp1;
	
	openCamera(c);
	Source.read(temp2);
	Source.release();
	usleep(FrameSkipTime);
	f2 = temp2;
	
	openCamera(c);
	Source.read(temp3);
	Source.release();
	usleep(FrameSkipTime);
	f3 = temp3;
	
	openCamera(c);
	Source.read(temp4);
	Source.release();
	usleep(FrameSkipTime);
	f4 = temp4;
		
}

Mat detectMotion(Mat &f1,Mat &f2,Mat &f3)
{
	Mat prev_frame,curr_frame,next_frame;
	prev_frame = f1;
	curr_frame = f2;
	next_frame = f3;

	//convert all the frames to gray 	
	Mat gray1,gray2,gray3;	
	cvtColor(prev_frame,gray1,COLOR_BGR2GRAY);
	cvtColor(curr_frame,gray2,COLOR_BGR2GRAY);
	cvtColor(next_frame,gray3,COLOR_BGR2GRAY);
	
	//difference between frames 
	Mat diff1,diff2;
	absdiff(gray1,gray3,diff1);
	absdiff(gray2,gray3,diff2);
	
	//compute the result frame
	Mat result;
	bitwise_and(diff1, diff2, result);

	//post-process the frames
    threshold(result,result,ThresholdValue,255,THRESH_BINARY);
    blur(result,result,Size(BlurValue,BlurValue));
    threshold(result,result,ThresholdValue,255,THRESH_BINARY);
    Mat element = getStructuringElement( MORPH_RECT, Size( 2*MorphSize + 1, 2*MorphSize+1 ), Point( MorphSize, MorphSize ) );
    morphologyEx( result, result, MORPH_CLOSE, element );
        
    return result;
}

Rect blobAnalysis(Mat &f)
{
	double a,largestArea=0;
	int i,largestContour=0;
	Rect box = Rect(0,0,0,0);
	
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	
	//find contours of filtered image using openCV findContours function
	findContours(f,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );		// retrieves all contours
	
	if(contours.size() < 1)
		return box;
		
	else
	{	
		//find the largest contour 	
		for(i=0;i<contours.size();i++)
		{
			a = contourArea(contours[i],false);
			if(a > largestArea)
			{	
				largestArea = a;
				largestContour = i;
			}
		}
		
		//area filter
		if(contourArea(contours[largestContour],false) < MinAreaFilter || contourArea(contours[largestContour],false) > MaxAreaFilter )
			return box;
		
		//cout << contourArea(contours[largestContour],false) << endl;
		
		//get the rectangle 
		box = boundingRect( (Mat) contours[largestContour]);
	}
				
	return box;			
}

float histcheck(Mat &f1,Mat &f2)
{
	float x;
	Mat t1,t2;
	MatND tp1,tp2;
	int channels[] = {0,1};
	int bins[] = {50,60};
	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };
	const float* ranges[] = { h_ranges, s_ranges };
		
	cvtColor( f1, t1, COLOR_BGR2HSV );
   	cvtColor( f2, t2, COLOR_BGR2HSV );
   	
    calcHist( &t1, 1, channels, Mat(), tp1, 2, bins, ranges, true, false );
   	calcHist( &t2, 1, channels, Mat(), tp2, 2, bins, ranges, true, false );
   	
   	normalize( tp1, tp1, 0, 1, NORM_MINMAX, -1, Mat() );
   	normalize( tp2, tp2, 0, 1, NORM_MINMAX, -1, Mat() );
   	
   	x = compareHist( tp1, tp2, CV_COMP_CORREL);
   			
	return x;
}

int featurecheck(Mat &f1,Mat &f2,Mat &f3)
{
	vector< KeyPoint > key1,key2;
	vector< DMatch > matches;
	Mat des1,des2;
	Mat a,b;
	int matchnumber;
	
	cvtColor(f1,a,COLOR_BGR2GRAY);
	cvtColor(f2,b,COLOR_BGR2GRAY);
		
	Ptr<FeatureDetector> detector = ORB::create();
	BFMatcher matcher(NORM_L1,true);
	
	detector -> detect(a,key1);
	detector -> detect(b,key2);
	
	detector -> compute(a,key1,des1);
	detector -> compute(b,key2,des2);
	
	if(des1.rows > 0 && des1.cols > 0 && des2.rows > 0 && des2.cols > 0)
		matcher.match(des1,des2,matches);
		
	matchnumber = matches.size();
	
	Mat img_matches;
    drawMatches( a, key1, b, key2, matches, img_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    f3 = img_matches;
	 	
	return matchnumber;
}

void computeParameter(Rect box1,Rect box2,int* dp,int* dt)
{
	double temp1,temp2;
	double r = 25;
	
	//compute the centeroids
	CenteroidX1 = box1.x + (box1.width  / 2);
	CenteroidY1 = CameraViewHeight - (box1.y + (box1.height / 2));
	CenteroidX2 = box2.x + (box2.width  / 2);
	CenteroidY2 = CameraViewHeight - (box2.y + (box2.height / 2));
			
	//compute the pan degree		
	temp1 = abs(CenteroidX1 - CenteroidX2) * 0.0147;
	temp2 = ((2*r*r) - (temp1*temp1)) / (2 * r * r);
	temp2 = acos(temp2) * (180 / M_PI) * RotationConstantX;
	*dp = ceil(temp2);
	
	//compute the tilt angle
	temp1 = abs(CenteroidY1 - CenteroidY2);
	temp1 = temp1 / 70;
	temp2 = temp1 * 5;
	*dt = ceil(temp2) * RotationConstantY;	
}

void normalizeParamters(CameraSensor *c ,int degreePan,int degreeTilt)
{
	//update pan degree 
	if((CenteroidX1 - CenteroidX2) < 0)
		c -> sensorPan = c -> sensorPan + degreePan;
	else if((CenteroidX1 - CenteroidX2) > 0)
		c -> sensorPan = c -> sensorPan - degreePan;
	else
		c -> sensorPan = c -> sensorPan;
				
	//normalize pan
	if(c -> sensorPan >= 360)
		c -> sensorPan = c -> sensorPan - 360;
	else if(c -> sensorPan <= 0)
		c -> sensorPan = 360 - abs(c -> sensorPan);
			
		
	//update tilt degree
	if((CenteroidY1 - CenteroidY2) > 0)
		c -> sensorTilt = c -> sensorTilt + degreeTilt;
	else if((CenteroidX1 - CenteroidX2) < 0)
		c -> sensorTilt = c -> sensorTilt - degreeTilt;	
	else
		c -> sensorTilt = c -> sensorTilt;
	
	//normalze tilt	
	if(c -> sensorTilt > 180)
		c -> sensorTilt = 0;
	else if(c -> sensorTilt < 0)
		c -> sensorTilt = 60;
		
}

int processTracking(CameraSensor *c,int iteration,int* posx,int *posy)
{
	//local Variables
	Mat frame1,frame2,frame3,frame4,frame5;
	Mat fgmask1,fgmask2;
	Mat Cropped1,Cropped2;
	Rect box1,box2;	
	Mat matched;
	int degreePan,degreeTilt;	
	char a[30],b[30];		


	//read frames
	cout << endl;
	cout << "READING FRAMES" << endl;
	readFrames(frame1,frame2,frame3,frame4,c -> cameraNumber);
		
	//compute the frame parameters	  	
	CameraViewWidth = frame1.cols;
	CameraViewHeight = frame1.rows;
	
	//initialize boxes for everyframe 
	box1.width = 0,box1.height = 0;
	box2.width = 0,box2.height = 0;
	
	// for the first three frames and iteration one
	fgmask1 = detectMotion(frame1,frame2,frame3);							//detect motion in the camera
	box1 = blobAnalysis(fgmask1);											//blob analysis
	Cropped1 = frame3(box1);
		  	
	// for the first three frames and iteration one
	fgmask2 = detectMotion(frame2,frame3,frame4);							//detect motion in the camera
	box2 = blobAnalysis(fgmask2);											//blob analysis
	Cropped2 = frame4(box2);
	
	if(box1.width > 0 && box1.height > 0 && box2.width > 0 && box2.height > 0)
	{
		cout << "Intrusion Detected" << endl;
		//cout << "Hist-Check: " << histcheck(Cropped1,Cropped2) << endl;
    	//cout << "Feature-Check: " << featurecheck(Cropped1,Cropped2,matched) << endl;
    	
    	computeParameter(box1,box2,&degreePan,&degreeTilt);
    	normalizeParamters(c,degreePan,degreeTilt);
    	//estimateDistance(posx,posy);
    	
    	cout << "Camera Pan Angle:" << c -> sensorPan << endl;
    	cout << "Camera Tilt Angle:" << c -> sensorTilt << endl;
    	
    	//caputure a refernce frame
    	usleep(500000);
    	openCamera(c -> cameraNumber);
		Source.read(frame5);
		Source.release();
    	   	
    	//draw the intrusion
    	rectangle( frame3, box1.tl(), box1.br(),Scalar(0,0,255) , 2, 8, 0 ); 		//Draw rectangle
		rectangle( frame4, box2.tl(), box2.br(),Scalar(0,0,255) , 2, 8, 0 ); 		//Draw rectangle
		
		//store the frames the result			
		strcpy(a,"../Photos/");
		strcat(a,"MotionFrame-1");
		sprintf(b,"%d",iteration);
		strcat(a,b);
		strcat(a,".jpg");
		imwrite(a,frame3);
			
		strcpy(a,"../Photos/");
		strcat(a,"MotionFrame-2");
		sprintf(b,"%d",iteration);
		strcat(a,b);
		strcat(a,".jpg");
		imwrite(a,frame4);
			
		strcpy(a,"../Photos/");
		strcat(a,"ReferenceFrame");
		sprintf(b,"%d",iteration);
		strcat(a,b);
		strcat(a,".jpg");
		imwrite(a,frame5);
		  				  		  				
		return YES;							
	}
	
	else
	{
		cout << "Intrusion NOT Detected" << endl;
		
		*posx = 0;
		*posy = 0;
		
		//store the frames the result			
		strcpy(a,"../Photos/");
		strcat(a,"MotionFrame-1");
		sprintf(b,"%d",iteration);
		strcat(a,b);
		strcat(a,".jpg");
		imwrite(a,frame3);
			
		strcpy(a,"../Photos/");
		strcat(a,"MotionFrame-2");
		sprintf(b,"%d",iteration);
		strcat(a,b);
		strcat(a,".jpg");
		imwrite(a,frame4);
			
		strcpy(a,"../Photos/");
		strcat(a,"ReferenceFrame");
		sprintf(b,"%d",iteration);
		strcat(a,b);
		strcat(a,".jpg");
		imwrite(a,frame5);
		
		return NO;
	}
	
}


int main()
{
	int i = 0;
	int positionX,positionY;
	pipeInitialize();
	
	while(1)
	{
		int bytesRead;
		int temp;
		bytesRead = recieveMessagePacket("CAMPIPER",&C);
		
		if(bytesRead > 0)
		{

			if(strcmp(C.Command,"Exit") == 0)
				break;
				
			if(strcmp(C.Command, "CAMERANUMBER") == 0)
			{
				sscanf (C.Payload,"%d",&temp);
				CameraSensorNumber = temp;
			}
			
			if(strcmp(C.Command, "CAMERASENSOR") == 0)
			{
				CameraSensor Y;
				int temp;
				sscanf (C.Payload,"%d %s %d %d %d %d %d %d %d %d",&temp,&Y.clientId,&Y.positionX,&Y.positionY,&Y.sensorColor,&Y.used,&Y.cameraNumber,&Y.tracking,&Y.sensorPan,&Y.sensorTilt);
				memcpy(&CameraSensorArray[temp],&Y,sizeof(CameraSensor));
			}
			
			if(strcmp(C.Command, "TRACK") == 0)
			{
			    int x;
			    
			    //recieve the camera number to run tracking on
				sscanf (C.Payload,"%d",&temp);
				
				//run the tracking that camera
				//x = processTracking(&CameraSensorArray[temp],i,&positionX,&positionY);
				CameraSensorArray[temp].tracking = YES;
				
				//send the tracking status
				sendCameraSensorc(&CameraSensorArray[temp],temp);
				positionX = CameraSensorArray[temp].positionX + 50 + i*5;
				positionY = CameraSensorArray[temp].positionY + 50 + i*5;
				//send the distance estimation
				sendDistance(positionX,positionY);
				
				i = i + 1;  
			}	
			
		}
	}
	
	Source.release();
	closePipe();
	return 0;

}
