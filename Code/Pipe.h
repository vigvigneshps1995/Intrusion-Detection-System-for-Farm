#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>


struct MessagePacket
{
	char Topic[100];
	char Payload[100];
	char ClientId[50];
	char Command[50];
}M;

char CastArray[sizeof(struct MessagePacket)];   //an array that is is used to cast memory from a structure



char PipeAddress[] = "temp/";					//address directory where the pipe files will be created
int PipeFd[5];									//fd of the pipe after opening them
int N;       									//an array to store the number of pipes required 		-----n = 3;
char PipeNames[5][20];							//names of the pipe    									-----strcpy(PipeNames[0],"SUBPIPE");
char PipePermission[2][5];						//permission of for each pipe   						-----strcpy(PipePermission[2],"RW");


int createPipes()
{
	int i,x;
	int success = 0;
	char temp[30];

	printf("\tCreating Pipes.....\t");
	for(i = 0;i < N;i++)
	{
		//generate the pipe address/name
		strcpy(temp,PipeAddress);
		strcat(temp,PipeNames[i]);

		//make a pipe file
		x = mkfifo(temp,0777);

		if( x == 0)
			printf("%s   ",PipeNames[i]);

		success = success & x;
	}
	printf("\n");

	//check for errors in opening
	if(success < 0)
	{
		printf("FAILED\n");
		return -1;
	}
}

int openPipes()
{
	int i;
	int success = 1;
	char temp[30];

	for(i = 0;i < N;i++)
	{
		//generate the pipe address/name
		strcpy(temp,PipeAddress);
		strcat(temp,PipeNames[i]);

		printf("\tWaiting for %s\t",PipeNames[i]);

		//assigning opening condition and open the pipes
		if(strcmp(PipePermission[i],"R") == 0)
			PipeFd[i] = open(temp,O_RDONLY,0777);

		if(strcmp(PipePermission[i],"W") == 0)
			PipeFd[i] = open(temp,O_WRONLY,0777);
	
		if(strcmp(PipePermission[i],"RW") == 0)
		
			PipeFd[i] = open(temp,O_RDWR,0777);

		if(PipeFd[i] > 0)
			printf("\t\tOPENED\n");
			
		success = success & PipeFd[i];
	}
	printf("\n");

	//check for errors in opening
	if(success < 0)
	{
		printf("\tPIPES FAILED TO OPEN\n");
		return -1;
	}

}

//a function to set the fields of the MessagePacket 
void setMessagePacket(struct MessagePacket *X,char t[sizeof(M.Topic)],char p[sizeof(M.Payload)],char cid[sizeof(M.ClientId)],char com[sizeof(M.Command)])
{
	strcpy(X -> Topic,t);
	strcpy(X -> Payload,p);
	strcpy(X -> ClientId,cid);
	strcpy(X -> Command,com);
}

//a function to send a MessagePacket to one of the fd
int sendMessagePacket(char Name[20],struct MessagePacket *X)
{
	int temp,i;

	//find the file desciptor for the respective name passed
	for(i = 0;i < N;i++)
	{
		if(strcmp(PipeNames[i],Name) == 0)
			break;
	}

	//cast the memory and send
	memcpy(CastArray,X,sizeof(M));
	temp = write(PipeFd[i],CastArray,sizeof(CastArray));
	return temp;
}

//a function to recieve a MessagePacket from one of the fd
int recieveMessagePacket(char Name[20],struct MessagePacket *X)
{
	int temp,i;

	//find the file desciptor for the respective name passed
	for(i = 0;i < N;i++)
	{
		if(strcmp(PipeNames[i],Name) == 0)
			break;
	}

	//revieve and cast the memory to an MessagePacket
	temp = read(PipeFd[i],CastArray,sizeof(CastArray));
	memcpy(X,CastArray,sizeof(M));
	return temp;
}

//close the fds and delete the file
void closePipe()
{
	int i;
	char temp[30];
	for( i= 0;i < N;i++)
	{
		close(PipeFd[i]);
		strcpy(temp,PipeAddress);
		strcat(temp,PipeNames[i]);
		unlink(temp);
	}
}

