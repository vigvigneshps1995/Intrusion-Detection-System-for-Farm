#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Data.h"
#include "Pipe.h"

struct MessagePacket P,S,G,C;

void computeRange(float);
void cameraTracking();
void intrusionDetected(int ,int , char *);


void pipeInitialize()
{
	N = 5;

	strcpy(PipeNames[0],"PUBPIPE");
	strcpy(PipePermission[0],"W");

	strcpy(PipeNames[1],"SUBPIPE");
	strcpy(PipePermission[1],"R");

	strcpy(PipeNames[2],"GRAPIPE");
	strcpy(PipePermission[2],"W");
	
	strcpy(PipeNames[3],"CAMPIPEW");
	strcpy(PipePermission[3],"R");
	
	strcpy(PipeNames[4],"CAMPIPER");
	strcpy(PipePermission[4],"W");

	createPipes();
	openPipes();
}

void distributeSensors()
{
	float sensorX,sensorY;
	float temp1,temp2;
	float numberX,numberY;
	int i = 0,j;

	//Perimeter Motion Sensor
	numberX = ceil(FieldX / (sqrt(2) * MotionSensorRange));
	numberY = ceil(FieldY / (sqrt(2) * MotionSensorRange));

	sensorX = (FieldX / numberX) / 2;
	sensorY = (FieldY / numberY) / 2;
	
	j =MotionSensorNumber;
	for(i=MotionSensorNumber - j; i<numberX ;i++)
	{
		temp1 = Boundary[0].Vertex[0] + ((2*i+1) * sensorX * ScaleX );
		temp2 = Boundary[0].Vertex[1] + ( sensorY * ScaleY );

		MotionSensorArray[j+i].sensorColor = RED;
		MotionSensorArray[j+i].positionX = temp1;
		MotionSensorArray[j+i].positionY = temp2;
	
		MotionSensorNumber = MotionSensorNumber + 1;
	}

	j =MotionSensorNumber;
	for(i=MotionSensorNumber - j; i<numberX ;i++)
	{
		temp1 = Boundary[3].Vertex[0] + ((2*i+1) * sensorX * ScaleX );
		temp2 = Boundary[3].Vertex[1] - ( sensorY * ScaleY );

		MotionSensorArray[j+i].sensorColor = RED;
		MotionSensorArray[j+i].positionX = temp1;
		MotionSensorArray[j+i].positionY = temp2;

		MotionSensorNumber = MotionSensorNumber + 1;
	}

	j =MotionSensorNumber;
	for(i=MotionSensorNumber - j; i<numberY - 2 ;i++)
	{
		temp1 = Boundary[0].Vertex[0] + ( sensorX * ScaleX );
		temp2 = Boundary[0].Vertex[1] + ((2*i+3) * sensorY * ScaleY ) ;

		MotionSensorArray[j+i].sensorColor = RED;
		MotionSensorArray[j+i].positionX = temp1;
		MotionSensorArray[j+i].positionY = temp2;
	
		MotionSensorNumber = MotionSensorNumber + 1;
	}

	j =MotionSensorNumber;
	for(i=MotionSensorNumber - j; i<numberY - 2 ;i++)
	{
		temp1 = Boundary[1].Vertex[0] - ( sensorX * ScaleX );
		temp2 = Boundary[1].Vertex[1] + ((2*i+3) * sensorY * ScaleY );

		MotionSensorArray[j+i].sensorColor = RED;
		MotionSensorArray[j+i].positionX = temp1;
		MotionSensorArray[j+i].positionY = temp2;

		MotionSensorNumber = MotionSensorNumber + 1;
	}


	//INNER CAMERA SENSORS
	numberX = ceil(FieldX / (sqrt(2) * CameraSensorRange));
	numberY = ceil(FieldY / (sqrt(2) * CameraSensorRange));

	sensorX = (FieldX / numberX) / 2;
	sensorY = (FieldY / numberY) / 2;

	for(i=1; i<numberX - 1 ; i++)
		for(j=1; j<numberY - 1 ; j++)
		{
			temp1 = Boundary[0].Vertex[0] + ((2*i+1) * sensorX * ScaleX );
			temp2 = Boundary[0].Vertex[1] + ((2*j+1) * sensorY * ScaleY );

			CameraSensorArray[CameraSensorNumber].sensorColor = BLUE;
			CameraSensorArray[CameraSensorNumber].positionX = temp1;
			CameraSensorArray[CameraSensorNumber].positionY = temp2;
			CameraSensorArray[CameraSensorNumber].used = UNUSED;
			CameraSensorArray[CameraSensorNumber].cameraNumber = 1 ;
			CameraSensorArray[CameraSensorNumber].tracking = NO;
			CameraSensorArray[CameraSensorNumber].sensorPan = InitialCameraX;
			CameraSensorArray[CameraSensorNumber].sensorTilt = InitialCameraY;
			
			CameraSensorNumber = CameraSensorNumber + 1;

		}

}

void assignClientId()
{
	int i;
	char temp[5],temp1[20],temp2[25];

	//ASSIGN CLIENT IDS FOR MOTION SENSOR
	strcpy(temp1,"Motion-");
	for(i=0;i<MotionSensorNumber;i++)
	{
		sprintf(temp,"%d",i);
		strcpy(temp2,temp1);
		strcat(temp2,temp);
		strcpy(MotionSensorArray[i].clientId,temp2);
		//printf("%s\n",temp2);
	}

	//ASSIGN CLIENT IDS FOR CAMERA SENSOR
	strcpy(temp1,"Camera-");
	for(i=0;i<CameraSensorNumber;i++)
	{
		sprintf(temp,"%d",i);
		strcpy(temp2,temp1);
		strcat(temp2,temp);
		strcpy(CameraSensorArray[i].clientId,temp2);
		//printf("%s\n",temp2);
	}

	//ASSIGN CLIENT IDS FOR LED
	/*strcpy(temp1,"Led-");
	for(i=0;i<LedNumber;i++)
	{
		sprintf(temp,"%d",i);
		strcpy(temp2,temp1);
		strcat(temp2,temp);
		strcpy(LedArray[i].clientId,temp2);
		//printf("%s\n",temp2);
	}*/
}

void sendMotionSensor(MotionSensor *m,int i)
{
	char x[100];
	char temp[10];
	
	setMessagePacket(&G,"","","","");
	strcpy(x,"");
	sprintf(temp,"%d",i);
	strcat(x,temp);
	strcat(x," ");
	strcat(x,m -> clientId);
	strcat(x," ");
	sprintf(temp,"%d",m -> positionX);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",m -> positionY);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",m -> sensorColor);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",m -> sensorPosition);
	strcat(x,temp);
	setMessagePacket(&G,"",x,"","MOTIONSENSOR");
	sendMessagePacket("GRAPIPE",&G);
}

void sendCameraSensorg(CameraSensor *c,int i)
{
	char x[100];
	char temp[10];
		
	setMessagePacket(&G,"","","","");
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
	setMessagePacket(&G,"",x,"","CAMERASENSOR");
	sendMessagePacket("GRAPIPE",&G);
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
	sendMessagePacket("CAMPIPER",&C);
}

void sendLed(Led *l,int i)
{
	char x[100];
	char temp[10];
		
	setMessagePacket(&G,"","","","");
	strcpy(x,"");
	sprintf(temp,"%d",i);
	strcat(x,temp);
	strcat(x," ");
	strcat(x,l -> clientId);
	strcat(x," ");
	sprintf(temp,"%d",l -> positionX);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",l -> positionY);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",l -> ledColor);
	strcat(x,temp);
	setMessagePacket(&G,"",x,"","LED");
	sendMessagePacket("GRAPIPE",&G);
}

void sendIntusion(Intrusion *I,int i)
{
	char x[100];
	char temp[100];
	setMessagePacket(&G,"","","","");
	
	strcpy(x,"");
	sprintf(temp,"%d",i);
	strcat(x,temp);
	strcat(x," ");
	strcat(x,I -> clientId);
	strcat(x," ");
	sprintf(temp,"%d",I -> positionX);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",I -> positionY);
	strcat(x,temp);
	strcat(x," ");
	sprintf(temp,"%d",I -> color);
	strcat(x,temp);
	setMessagePacket(&G,"",x,"","INTRUSION");
	sendMessagePacket("GRAPIPE",&G);
}

void computeRange(float ScaleFactor)
{
	int i,j;
	float angle;
	float normalRange = (sqrt(2) * CameraSensorRange * ScaleFactor) / 2; 

	for(i=0;i<CameraSensorNumber;i++)
	{
	
		if(CameraSensorArray[i].tracking == NO)
		{
			if(IntrusionX >= (CameraSensorArray[i].positionX - (normalRange * ScaleX)) && IntrusionX <= (CameraSensorArray[i].positionX + (normalRange * ScaleX)) && IntrusionY >= (CameraSensorArray[i].positionY - (normalRange * ScaleY)) && IntrusionY<= (CameraSensorArray[i].positionY + (normalRange * ScaleY)))

			{
				//printf("GOT IT : %d\n",i);
				angle = atan(abs((CameraSensorArray[i].positionY - IntrusionY) / (CameraSensorArray[i].positionX - IntrusionX))) / (M_PI / 180);

				if(( IntrusionX - CameraSensorArray[i].positionX > 0)  && ( IntrusionY - CameraSensorArray[i].positionY > 0) )
					angle = 90 - angle;
				if(( IntrusionX - CameraSensorArray[i].positionX > 0)  && ( IntrusionY - CameraSensorArray[i].positionY < 0) )
					angle = 90 + angle;
				if(( IntrusionX - CameraSensorArray[i].positionX < 0)  && ( IntrusionY - CameraSensorArray[i].positionY < 0) )
					angle = 270 - angle;
				if(( IntrusionX - CameraSensorArray[i].positionX < 0)  && ( IntrusionY - CameraSensorArray[i].positionY > 0) )
					angle = 270 + angle;

				CameraSensorArray[i].used = USED;
				CameraSensorArray[i].tracking = NO;
				CameraSensorArray[i].sensorPan = angle;
				CameraSensorArray[i].sensorTilt = ViewCameraY;
			
				//printf("angle:%f\n",angle);
			}
			else
			{
				CameraSensorArray[i].used = UNUSED;
				CameraSensorArray[i].tracking = NO;
				CameraSensorArray[i].sensorPan = InitialCameraX;
				CameraSensorArray[i].sensorTilt = InitialCameraY;
			}
		}
	}
}

void cameraTracking()
{
	int i;
	char x[100];
	char tempc[10];
	int tempi;
	int bytesRead = 0;
	int distancex,distancey;

	for (i = 0;i< CameraSensorNumber;i++)
	{
		if(CameraSensorArray[i].used == USED)
		{
		
			//SEND THE CAMERA NUMBER THAT MUST BE TRACKED
			setMessagePacket(&C,"","","","");
			strcpy(x,"");
			sprintf(tempc,"%d",i);
			strcat(x,tempc);
			setMessagePacket(&C,"",x,"","TRACK");
			sendMessagePacket("CAMPIPER",&C);
			
			while( bytesRead == 0)
			{
				//read from camera
				bytesRead = recieveMessagePacket("CAMPIPEW",&C);
				
				if(strcmp(C.Command, "CAMERASENSOR") == 0)
				{
				   CameraSensor Y;
				   sscanf(C.Payload,"%d %s %d %d %d %d %d %d %d %d",&tempi,&Y.clientId,&Y.positionX,&Y.positionY,&Y.sensorColor,&Y.used,&Y.cameraNumber,&Y.tracking,&Y.sensorPan,&Y.sensorTilt);
				   memcpy(&CameraSensorArray[tempi],&Y,sizeof(CameraSensor));
				}
			}
			
			bytesRead = 0;
			while(bytesRead == 0)
			{	
				//read from camera
				bytesRead = recieveMessagePacket("CAMPIPEW",&C);
				
				if(strcmp(C.Command, "DISTANCE") == 0)
				{
					sscanf(C.Payload,"%d %d",&distancex,&distancey);
					
					if(distancex > 0 && distancey > 0)
					{
						//distancex = distancex * ;
						//distancey = distancey * ;
						//usleep();
						intrusionDetected(distancex,distancey,CameraSensorArray[tempi].clientId);
					}
				}	
			}
		}
	}
}

void intrusionDetected(int X,int Y,char C[50])
{
	int i;
	char temp[50]; 
	
	//change global values
	IntrusionX = X;
	IntrusionY = Y;

	//Assign values to intrusion
	IntrusionArray[IntrusionNumber].positionX = IntrusionX;
	IntrusionArray[IntrusionNumber].positionY = IntrusionY;
	IntrusionArray[IntrusionNumber].color = BLACK;
	strcpy(IntrusionArray[IntrusionNumber].clientId,C);
						
	//send the intusion
	sendIntusion(&IntrusionArray[IntrusionNumber],IntrusionNumber);
	sprintf(temp,"%d",IntrusionNumber + 1);				   
	setMessagePacket(&G,"","","","");
	setMessagePacket(&G,"",temp,"","INTRUSIONNUMBER");
	sendMessagePacket("GRAPIPE",&G);
										
	//Rotate the camera
	computeRange(1.25);
	
	//Publish all camera rotation
	for(i=0;i<CameraSensorNumber;i++)
	{
		setMessagePacket(&P,"","","","");
		sprintf(temp,"%d",CameraSensorArray[i].sensorPan);
		setMessagePacket(&P,"CameraSensor",temp,CameraSensorArray[i].clientId,"Pan");
		sendMessagePacket("PUBPIPE",&P);
		
		setMessagePacket(&P,"","","","");
		sprintf(temp,"%d",CameraSensorArray[i].sensorTilt);
		setMessagePacket(&P,"CameraSensor",temp,CameraSensorArray[i].clientId,"Tilt");
		sendMessagePacket("PUBPIPE",&P);
	}
					
	//Send the Camera values to graphics		
	for(i=0;i<CameraSensorNumber;i++)				
		sendCameraSensorg(&CameraSensorArray[i],i);
		
	//camera tracking
	cameraTracking();	
	
	//publish the tracking coordinates
	for(i=0;i<CameraSensorNumber;i++)
	{
		setMessagePacket(&P,"","","","");
		sprintf(temp,"%d",CameraSensorArray[i].sensorPan);
		setMessagePacket(&P,"CameraSensor",temp,CameraSensorArray[i].clientId,"Pan");
		sendMessagePacket("PUBPIPE",&P);
		
		setMessagePacket(&P,"","","","");
		sprintf(temp,"%d",CameraSensorArray[i].sensorTilt);
		setMessagePacket(&P,"CameraSensor",temp,CameraSensorArray[i].clientId,"Tilt");
		sendMessagePacket("PUBPIPE",&P);
	}
		
	//increment
	IntrusionNumber = IntrusionNumber + 1;
			
}


int main()
{


	char temp[10],tempc[20];
	int i,j;
	int tempi;
	int prevmotion = 0;

	pipeInitialize();

	//Call the function to disturbute the motion sensor
	distributeSensors();
	
	//Assign Client-Id to ALL the Nodes of the system
	assignClientId();

	//send this data to graphics file
	for(i=0;i<MotionSensorNumber;i++)				//SEND THE MOTION SENSOR STRUCT
		sendMotionSensor(&MotionSensorArray[i],i);

	for(i=0;i<CameraSensorNumber;i++)				//SEND THE Camera SENSOR STRUCT
		sendCameraSensorg(&CameraSensorArray[i],i);

	for(i=0;i<LedNumber;i++)						//Send the led struct
		sendLed(&LedArray[i],i);
		
	sprintf(temp,"%d",MotionSensorNumber);			//number of motion sensors used 
	setMessagePacket(&G,"","","","");
	setMessagePacket(&G,"",temp,"","MOTIONNUMBER");
	sendMessagePacket("GRAPIPE",&G);


	sprintf(temp,"%d",CameraSensorNumber);			//number of camera used 
	setMessagePacket(&G,"","","","");
	setMessagePacket(&G,"",temp,"","CAMERANUMBER");
	sendMessagePacket("GRAPIPE",&G);


	sprintf(temp,"%d",LedNumber);				   //number of led used
	setMessagePacket(&G,"","","","");
	setMessagePacket(&G,"",temp,"","LEDNUMBER");
	sendMessagePacket("GRAPIPE",&G);
	
	
	//send this information to Camera Processing file
	for(i=0;i<CameraSensorNumber;i++)						//SEND THE Camera SENSOR STRUCT
		sendCameraSensorc(&CameraSensorArray[i],i);	
		
	sprintf(temp,"%d",CameraSensorNumber);			//number of camera used 
	setMessagePacket(&C,"","","","");
	setMessagePacket(&C,"",temp,"","CAMERANUMBER");
	sendMessagePacket("CAMPIPER",&C);	
	
	
	//Publish all the initial camera positions
	for(i=0;i<CameraSensorNumber;i++)
	{
		setMessagePacket(&P,"","","","");
		sprintf(temp,"%d",CameraSensorArray[i].sensorPan);
		setMessagePacket(&P,"CameraSensor",temp,CameraSensorArray[i].clientId,"Pan");
		sendMessagePacket("PUBPIPE",&P);
		
		setMessagePacket(&P,"","","","");
		sprintf(temp,"%d",CameraSensorArray[i].sensorTilt);
		setMessagePacket(&P,"CameraSensor",temp,CameraSensorArray[i].clientId,"Tilt");
		sendMessagePacket("PUBPIPE",&P);
	}


	while(1)
	{	
		int bytesRead;

		//read from subscriber
		bytesRead = recieveMessagePacket("SUBPIPE",&S);

		/*************************************** EDIT HERE TO ADD MORE COMMANDS************************/

		//a new motion is detected in the fiels and a message is sent
		if(strcmp(S.Topic,"MotionSensor")==0)
		{
			//the message type is a motion detected
			if(strcmp(S.Command,"Motion")==0)		
			{
				if(strcmp(S.Payload,"YES") == 0)
				{
				
					//find the sensor that detected motion
					for(i=0;i<MotionSensorNumber;i++)
						if(strcmp(MotionSensorArray[i].clientId,S.ClientId) == 0)
							break;	
					
												
					//dummy intrusion coordinates and intrusion properties
					if(IntrusionNumber == 0)
					{
						if((MotionSensorArray[i].positionY - Boundary[0].Vertex[1]) < (MotionSensorRange * ScaleY))
						{
							IntrusionX = MotionSensorArray[i].positionX - 1;
							IntrusionY = Boundary[0].Vertex[1] - 1;
						}
						else if((Boundary[3].Vertex[1] - MotionSensorArray[i].positionY ) < (MotionSensorRange * ScaleY))
						{
							IntrusionX = MotionSensorArray[i].positionX - 1;
							IntrusionY = Boundary[3].Vertex[1] - 1;
						}
						else if((MotionSensorArray[i].positionX - Boundary[0].Vertex[0]) < (MotionSensorRange * ScaleX))
						{
							IntrusionX = Boundary[0].Vertex[0] - 1;
							IntrusionY = MotionSensorArray[i].positionY - 1;
						}
						else if((Boundary[1].Vertex[0] - MotionSensorArray[i].positionX) < (MotionSensorRange * ScaleX))
						{
							IntrusionX = Boundary[1].Vertex[0] - 1;
							IntrusionY = MotionSensorArray[i].positionY - 1;
						}
						
						MotionSensorArray[i].sensorColor = GREEN;
					}
					
					/*
					else
					{	
						//if the same sensor had detected motion before then update the dummy intrusion	
						//same sensor
					    if(strcmp(MotionSensorArray[i].clientId,MotionSensorArray[prevmotion].clientId) == 0)
						{
							if((MotionSensorArray[i].positionY - Boundary[0].Vertex[1]) < (MotionSensorRange * ScaleY))
								IntrusionY = IntrusionY + (((MotionSensorRange * sqrt(2)) / 4) * ScaleY) - 1;
						
							else if((Boundary[3].Vertex[1] - MotionSensorArray[i].positionY ) < (MotionSensorRange * ScaleY))
								IntrusionY = IntrusionY - (((MotionSensorRange * sqrt(2)) / 4) * ScaleY) - 1;
						
							else if((MotionSensorArray[i].positionX - Boundary[0].Vertex[0]) < (MotionSensorRange * ScaleX))
								IntrusionX = IntrusionX + (((MotionSensorRange * sqrt(2)) / 4) * ScaleX) - 1;

							else if((Boundary[1].Vertex[0] - MotionSensorArray[i].positionX) < (MotionSensorRange * ScaleX))
								IntrusionX = IntrusionX - (((MotionSensorRange * sqrt(2)) / 4) * ScaleX) - 1;
								
							MotionSensorArray[i].sensorColor = GREEN;	
						}
						
						//find out if the same sensor had detected motion before if not then make a dummy intrustion and call the camera range funtion
						//different sensor
						else
						{
							if(abs(MotionSensorArray[prevmotion].positionY - MotionSensorArray[i].positionY) > 0)
								IntrusionY = MotionSensorArray[i].positionY - 1;
								
							else if(abs(MotionSensorArray[prevmotion].positionX - MotionSensorArray[i].positionX) > 0)
								IntrusionX = MotionSensorArray[i].positionX - 1;
								
							MotionSensorArray[i].sensorColor = GREEN;
							MotionSensorArray[prevmotion].sensorColor = RED;	
						}
					}
					
					//change the color of the sensor and send it		
					sendMotionSensor(&MotionSensorArray[i],i);
					sendMotionSensor(&MotionSensorArray[prevmotion],prevmotion);
					
					//call intrusion handling function
					intrusionDetected(IntrusionX,IntrusionY,MotionSensorArray[i].clientId);
					
					*/
											
					//Increment
					prevmotion = i;
					
					
				}
			}
					
		}
		
		//make a new logic to reduce the conflicts that occur when multiple camera track the same object
		if(strcmp(S.Command,"IntrusionPosition")==0)
		{
			int x,y;
			char c[50]; 
			strcpy(c,S.ClientId);
			sscanf (S.Payload,"%d %d",&x,&y);
			
			intrusionDetected(x,y,c);
		}
		
		if(strcmp(S.Topic,"Intrusion")==0)
		{
			int x,y;
			char c[50] = "Nil/Random";
			sscanf (S.Payload,"%d %d",&x,&y);
			
			intrusionDetected(x,y,c);
		}
		
		if(strcmp(S.Topic,"Reset")==0)
		{
			MotionSensorNumber = 0;
			CameraSensorNumber = 0;
			LedNumber = 0;
			IntrusionNumber = 0;

			//send this data to graphics file
			sprintf(temp,"%d",MotionSensorNumber);			//number of motion sensors used 
			setMessagePacket(&G,"","","","");
			setMessagePacket(&G,"",temp,"","MOTIONNUMBER");
			sendMessagePacket("GRAPIPE",&G);

			sprintf(temp,"%d",CameraSensorNumber);			//number of camera used 
			setMessagePacket(&G,"","","","");
			setMessagePacket(&G,"",temp,"","CAMERANUMBER");
			sendMessagePacket("GRAPIPE",&G);

			sprintf(temp,"%d",LedNumber);				   //number of led used
			setMessagePacket(&G,"","","","");
			setMessagePacket(&G,"",temp,"","LEDNUMBER");
			sendMessagePacket("GRAPIPE",&G);

			sprintf(temp,"%d",IntrusionNumber);				   //initializing intrusion number
			setMessagePacket(&G,"","","","");
			setMessagePacket(&G,"",temp,"","INTRUSIONNUMBER");
			sendMessagePacket("GRAPIPE",&G);
			
			//Redisturbute sensors again
			distributeSensors();
			assignClientId();

			for(i=0;i<MotionSensorNumber;i++)				//SEND THE MOTION SENSOR STRUCT
				sendMotionSensor(&MotionSensorArray[i],i);

			for(i=0;i<CameraSensorNumber;i++)				//SEND THE MOTION SENSOR STRUCT
				sendCameraSensorg(&CameraSensorArray[i],i);

			for(i=0;i<LedNumber;i++)
				sendLed(&LedArray[i],i);

			sprintf(temp,"%d",MotionSensorNumber);			//number of motion sensors used 
			setMessagePacket(&G,"","","","");
			setMessagePacket(&G,"",temp,"","MOTIONNUMBER");
			sendMessagePacket("GRAPIPE",&G);

			sprintf(temp,"%d",CameraSensorNumber);			//number of camera used 
			setMessagePacket(&G,"","","","");
			setMessagePacket(&G,"",temp,"","CAMERANUMBER");
			sendMessagePacket("GRAPIPE",&G);

			sprintf(temp,"%d",LedNumber);				   //number of led used
			setMessagePacket(&G,"","","","");
			setMessagePacket(&G,"",temp,"","LEDNUMBER");
			sendMessagePacket("GRAPIPE",&G);
		}
		
		
		if(strcmp(S.Topic,"Exit")==0)
		{
			setMessagePacket(&G,"","","","");
			strcpy(G.Command,"Exit");
			sendMessagePacket("GRAPIPE",&G);
			
			setMessagePacket(&P,"","","","");
			strcpy(P.Command,"Exit");
			sendMessagePacket("PUBPIPE",&P);
			
			setMessagePacket(&C,"","","","");
			strcpy(C.Command,"Exit");
			sendMessagePacket("CAMPIPER",&C);
			
			break;
		}

		/*
		//publish it publisher
		setMessagePacket(&P,"/please/work","HELLO",S.ClientId,S.Command);
		sendMessagePacket("PUBPIPE",&P);
		
		//publish it both graphics
		setMessagePacket(&G,"/please/work","HELLO",S.ClientId,S.Command);
		sendMessagePacket("GRAPIPE",&G);
		*/














		/*************************************** DO NOT EDIT HERE ***********************************/		


	}

	closePipe();
	return 0;
}
