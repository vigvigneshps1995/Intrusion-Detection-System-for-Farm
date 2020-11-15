#include <stdio.h>
#include <stdlib.h>
#include "Pipe.h"
#include <MQTTClient.h>

/**************************************************** INPUT ************************************************/


char Address[50] ;          //char Address[50] = "tcp://localhost:1883";
char ClientId[50] = "CONTROLCENTER-PUB ";
int QOS = 1;


/***************************************************** GLOBAL **********************************************/


struct MessagePacket P;
volatile MQTTClient_deliveryToken deliveredtoken;

void pipeInitialize();
int messageArrived(void*, char*, int , MQTTClient_message*);
void delivered(void* , MQTTClient_deliveryToken);
void connectionLost(void*, char*);


/***************************************************** FUNCTIONS ******************************************/


void pipeInitialize()
{
	N = 1;
	strcpy(PipeNames[0],"PUBPIPE");
	strcpy(PipePermission[0],"R");

	openPipes();
}

int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char *payload;

	printf("\n\n");
    printf("\tMESSAGE ARRIVED\n");
    printf("\t\ttopic: %s\t", topicName);
    printf("message: ");

    payload = (char*) message->payload;
    printf("%s\n",payload);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("\n\t\tDelivery Confirmed\n");
	printf("\n");
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
	int bytesRead;
	char payload[50],topic[50];
	char temp[50];
	strcpy(Address,argv[1]);
	MQTTClient Client;
	MQTTClient_connectOptions Options = MQTTClient_connectOptions_initializer;
	MQTTClient_message Message = MQTTClient_message_initializer;
    MQTTClient_deliveryToken Token;

	printf("\n\n\n");
	printf("\tCreating Control-Center Publisher.\n");
	Options.keepAliveInterval = 20;
	Options.cleansession = 1;
	MQTTClient_create(&Client, Address, ClientId,MQTTCLIENT_PERSISTENCE_NONE, NULL);
	MQTTClient_setCallbacks(Client, NULL, connectionLost, messageArrived, delivered);
	printf("\tTrying to Connect To Broker....... \t");
	flag = MQTTClient_connect(Client,&Options);

	if(flag != MQTTCLIENT_SUCCESS)
	{
		printf("FAILED\n");
		return -1;
	}
	printf("CONNECTED\n");

	pipeInitialize();
	
	printf("\n\n\n");
	printf("********************************************************************************");
	printf("\n\n");

	while(1)
	{
		bytesRead = recieveMessagePacket("PUBPIPE",&P);
		//printf("\t%d Bytes Recieved.\n",bytesRead);
		if(bytesRead > 0)
		{

			if(strcmp(P.Command,"Exit") == 0)
				break;
			else
			{
				//assemble the topic
				strcpy(temp,P.Topic);
				strcat(temp,"/");
				strcat(temp,P.ClientId);
				strcat(temp,"/");
				strcat(temp,P.Command);
				strcpy(topic,temp);
			
				//send
				printf("\tPublishing Topic :%s\t",topic);
				Message.payload = P.Payload;
    			Message.payloadlen = strlen(P.Payload);
    			Message.qos = QOS;
    			Message.retained = 0;
    			deliveredtoken = 0;
    			
				MQTTClient_publishMessage(Client, topic, &Message, &Token);
				while(deliveredtoken != Token);
			}
		}
 	}

	closePipe();
    MQTTClient_disconnect(Client, 10000);
    MQTTClient_destroy(&Client);
	return 0;
}
