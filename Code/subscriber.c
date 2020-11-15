#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Pipe.h"
#include <MQTTClient.h>


/**************************************************** INPUT ************************************************/


char Address[50] ;
char ClientId[50] = "CONTROLCENTER-SUB";
int QOS = 1;


/**************************************************** GLOBAL ************************************************/

struct MessagePacket S;
volatile MQTTClient_deliveryToken deliveredtoken;

void pipeInitialize();
int messageArrived(void*, char*, int , MQTTClient_message*);
void delivered(void* , MQTTClient_deliveryToken);
void connectionLost(void*, char*);


/**************************************************** FUNCTIONS **********************************************/


void pipeInitialize()
{
	N = 1;
	strcpy(PipeNames[0],"SUBPIPE");
	strcpy(PipePermission[0],"W");

	openPipes();
}

int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i = 0;
    char *payload;
    char *token;
    const char delimiter[1] = {'/'};
	char temp[3][20];
	setMessagePacket(&S,"","","","");
	
	printf("\n\n");
    printf("\tMESSAGE ARRIVED\n");
    printf("\t\ttopic: %s\t", topicName);
    printf("message: ");
    payload = (char*) message->payload;
    printf("%s\n",payload);
   
   	token = strtok(topicName, delimiter);
   	while( token != NULL ) 
   	{
   		  if(i == 0)
   		   	strcpy(S.Topic,token);	
   		  if(i == 1)
    	  	strcpy(S.ClientId,token);
    	  if(i == 2)
    	  	strcpy(S.Command,token);
    	  	
    	  i = i + 1;
    	  token = strtok(NULL, delimiter);
   	}
   	strcpy(S.Payload,payload);

	sendMessagePacket("SUBPIPE",&S);

	if(strcmp(topicName,"Exit") == 0)
	{
		closePipe();
		exit(0);
	}

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("\t\tDelivery Confirmed\n");
	printf("\n\n");
    deliveredtoken = dt;
}

void connectionLost(void *context, char *cause)
{
    printf("\tCONNECTION LOST!\n");
    printf("\tCAUSE: %s\n", cause);
}

/******************************************************** MAIN ********************************************/


int main(int argc,char *argv[])
{
	
	int flag,i;
	strcpy(Address,argv[1]);
	MQTTClient Client;
	MQTTClient_connectOptions Options = MQTTClient_connectOptions_initializer;

	printf("\n\n\n");
	printf("\tCreating Control-Center Subscriber.\n");
	Options.keepAliveInterval = 20;
	Options.cleansession = 1;
	MQTTClient_create(&Client, Address, ClientId,MQTTCLIENT_PERSISTENCE_NONE, NULL);
	MQTTClient_setCallbacks(Client, NULL, connectionLost, messageArrived, delivered);
	printf("\tTrying to Connect To Broker....... ");
	flag = MQTTClient_connect(Client,&Options);

	if(flag != MQTTCLIENT_SUCCESS)
	{
		printf("FAILED\n");
		return -1;
	}
	printf("CONNECTED\n");
	
	printf("\tSubcribing to :\t");
	for(i=2;i<argc;i++)
	{
		printf("%s   ",argv[i]);
		MQTTClient_subscribe(Client, argv[i], QOS);	
	}
	printf("\n");

	pipeInitialize();

	printf("\n\n\n");
	printf("********************************************************************************");
	printf("\n\n");

	while(1)
	{
		sleep(1);
	}

	closePipe();
    MQTTClient_disconnect(Client, 10000);
    MQTTClient_destroy(&Client);
	return 0;
}
