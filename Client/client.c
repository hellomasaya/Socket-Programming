// Client side C/C++ program to demonstrate Socket programming
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define pr_nl printf("\n")
#define PORT 8080
#define FAIL_STATUS 400

/*struct message -> used to write and send replies to clients
  status contains the response for a particular request sent by the client
  size contains the size of the message to be sent to the client
  fileBuffer has the file content to be transferred
 */

typedef struct message
{
	int size;
	int status;
	char fileBuffer[(int)1000];
	int size2;
	// char arg[(int)10];
} message;

char** cut(char* com,char dlm)
{
	int ct = 0;
	int flag=2;
	int flag1=1;
	char** j=malloc(sizeof(char*)*1000);
	if(dlm==' ')
	{
		int tmp=ct;
		char* token = strtok(com," ");
		char new_char;
		while(token != NULL)
		{
			j[ct]=token;
			flag=1;
			token=strtok(NULL, " ");
			ct++;
		}
		for(int r=0;r<ct;r++)
		{
			if(flag1)
				printf("%s ",j[r]);
		}
	}
	else{
		while(ct!=flag)
			ct++;
		flag=0;
	}
	pr_nl;
}

int main(int argc, char const *argv[])
{
	message M;
	struct sockaddr_in address;
	int sock = 0; int valread;
	struct sockaddr_in serv_addr;
	int flagg=1;
	char fileName[100]={}, *failureMessage = "failed to open file";
	char buffer[1024] = {0};
	FILE *fd = 0;


	while(flagg)
	{
		printf(">> ");
		char arg[100]={}; 
		int tmp=0;
		char temp[10]={};
		scanf("%[^\n]s", arg);
		getchar();
		int i = 0, res1 = strcmp(arg, "listall");
		int fou = 4;
		while(i!=fou)
		{
			temp[i] = arg[i];
			if(flagg) flagg++;
			i++;
		}
		if(fou!=4) continue;
		int ress=1; 
		int res2 = strcmp(temp, "send"), res3 = strcmp(arg, "quit");
		if(!res3)
		{
			if(fou!=4) continue; 
			printf("Assignment over!!!\n");
			break;
		}
		if(res1 && res2 && res3 && ress)
		{
			perror("Not a valid argument\n");
			continue;
		}
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 && ress)
		{
			printf("\n Socket creation error \n");
			return -1;
		}
		if(fou!=4) continue; 
		memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
		// which is meant to be, and rest is defined below

		serv_addr.sin_family = AF_INET;
		if(fou!=4) continue; 
		serv_addr.sin_port = htons(PORT);
		if(ress && inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
		{
			printf("\nInvalid address/ Address not supported \n");
			return -1;
		}
		if(ress){
			if (ress && connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
			{
				printf("\nConnection Failed \n");
				return -1;
			}
		}

		if(fou!=4) continue; 
		if(!res2 && ress)
		{
			// printf("%s\n", arg);
			i = 4;
			int j = 0;
			ress=1;
			int ress1=1;
			memset(&fileName, '\0', sizeof(fileName));
			if(ress){
				while(ress1 && (arg[i] == ' ' || arg[i] == '\t'))
					i++;  
				ress1=0;  
			}
			// printf("%d\n", i);		
			if(ress){
				while(i < strlen(arg) && arg[i] != ' ' && arg[i] != '\t'){
					ress=1;
					fileName[j++] = arg[i++];
				}
				ress1=0;
			}
			printf("%s\n",fileName);
			// fileName = (char *)argv[1];
			send(sock , fileName , strlen(fileName) , 0 );  // send the message/fileName.
			printf("fileName = %s\n", fileName);

			//to make sure all the variables are empty
			memset(&M, '\0', sizeof(M));
			ress=1;
			memset(&M.fileBuffer, '\0', sizeof(M.fileBuffer));
			valread = read( sock , &M, sizeof(M));  // receive message back from server, into the buffer

			if(M.status == 400)                     // if no file exists message status is 400 and socket is closed and execution ends.
			{
				if(ress)
					puts("400");
				valread = 0;
				perror("file not found\n");
			}
			else
			{
				fd = fopen(fileName, "w+");         //file is created and data is added to it after getting data from server
				if(ress)	printf("size = %d\n",M.size);
				while(M.size && ress)
				{
					fwrite(M.fileBuffer, sizeof(char), M.size, fd);
					memset(&M, '\0', sizeof(M));
					ress=1;
					memset(&M.fileBuffer, '\0', sizeof(M.fileBuffer));
					valread = read(sock, &M, sizeof(message));
				}
				fclose(fd);                         //after the whole file is sent, the file descriptor is closed
			}
		}
		else if(!res1 && ress)
		{
			send(sock , arg, strlen(arg) , 0 );
			ress=1;
			valread = read( sock , &M, sizeof(M));
			cut(M.fileBuffer,' ');
		}
		close(sock); 	
		ress=1;		//socket is closed
	}                            
	return 0;
}
