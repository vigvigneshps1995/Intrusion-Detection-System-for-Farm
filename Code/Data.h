#include <GL/glut.h>


#define RED  0
#define BLUE 1
#define GREEN 2
#define YELLOW 3
#define BLACK 4
#define ORANGE 5

#define USED 1
#define UNUSED 0 

#define YES 1
#define NO 0

typedef struct
{
	GLfloat Vertex[2];
}Point2D;

typedef struct 
{
	char clientId[50];
	int positionX;
	int positionY;
	int sensorColor;
	int sensorPosition;
}MotionSensor;

typedef struct
{
	char clientId[50];
	int positionX;
	int positionY;
	int sensorColor;
	int used;
	int cameraNumber;
	int tracking;
	int	sensorPan;
	int sensorTilt;
}CameraSensor;

typedef struct
{
	char clientId[50];
	int positionX;
	int positionY;
	int ledColor;
}Led;

typedef struct
{
	char clientId[50];
	int positionX;
	int positionY;
	int color;
}Intrusion;

MotionSensor MotionSensorArray[50];
CameraSensor CameraSensorArray[50];
Led LedArray[300];
Intrusion IntrusionArray[300];

const Point2D Boundary[4] = {{100,100},{900,100},{900,900},{100,900}};						//field boundary coordinates

int FieldX = 64 ;																			//lenght of the field in meters
int FieldY = 64 ;																			//width of the field in meters 

int ResolutionX = 4;																		//minimum trackable areas lenght in meters
int ResolutionY = 4;																		//minimum trackable areas width in meters

int WindowX = 1000;																			//lenght of the window to display graphics
int WindowY = 1000;																			//width of the window to display graphics 

const float ScaleX = (Boundary[1].Vertex[0] - Boundary[0].Vertex[0]) / FieldX;				//scale in x direction
const float ScaleY = (Boundary[2].Vertex[1] - Boundary[1].Vertex[1]) / FieldY;				//scale in y direction

float IntrusionX = 0;																		//intrusion position
float IntrusionY = 0;																		//intrusion position

int MotionSensorNumber = 0;																	//number of motion sensor used 
int IntrusionNumber = 0;																	//number of intrusion occureed
int CameraSensorNumber = 0;																	//numbber of cameras used
int LedNumber = 0;																			//number of leds used

float MotionSensorRange = 10;																//range of motion sensor
float CameraSensorRange = 15;																//range of camera 
float CameraSensorAngle = 60;																//range angle of camera sensor

int MotionNumber = 75;																		//randow number to restrict motion and linearity of the motion
int InitialCameraX = 0;																		//pan degree where the camera starts
int InitialCameraY = 45;																	//tilt degree when the camera starts
int ViewCameraY = 60;																		//the camera tilt degree to look at the intrusion at the CameraSensorRange

float FrameSkipTime = 1000;																	//time in seconds - dealy between consequeive frames
int ThresholdValue = 30;																	//our sensitivity value to be used for the thresolding of the motion mask
int BlurValue = 20;                         												//blur value for reducing inconsistency
int MorphSize = 10;																			//Used for Image closing and removing incosistency
int MinAreaFilter = 500; 																	//area of the minimun accepted blob
int MaxAreaFilter = 75000;																	//area of the maximum accepted blob
double RotationConstantX = 5;																//linear rotation constant
double RotationConstantY = 1;																//linear rotation constant	

int WaitTime = 2;																			//time for which the camera waits

