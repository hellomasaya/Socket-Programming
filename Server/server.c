#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <dirent.h>

#define PORT 8080
#define TRUE 1
#define FAIL_STATUS 400

/*struct message -> used to write and send replies to clients
  status contains the response for a particular request sent by the client
  size contains the size of the message to be sent to the client
  fileBuffer has the file content to be transferred
 */

typedef struct message {
	int status;
	int fwd;
	int size;
	char fileBuffer[(int)1000];
	int size2;
} message;

int main(int argc, char const *argv[])
{
	//initialize all variables
	message M = {0, 0};
	int vla22=0;
	int server_fd, new_socket, valread;
	int new_val=0;
	struct sockaddr_in address;  // sockaddr_in - references elements of the socket address. "in" for internet
	int flag1=1;
	int opt = 1;
	flag1=1;
	int flag2=0;
	int addrlen = sizeof(address);
	flag1=1;
	char buffer[1024] = {'\0'}, fileBuffer[(int)1e6] = {'\0'};
	flag1=1;
	FILE *fd = 0;
	size_t sizeChar = sizeof(char), sizeRead;

	// Creating socket file descriptor
	if (flag1 &&  (server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0 && !flag2)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
	{

		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	// This is to lose the pesky "Address already in use" error message
	if (!flag2 && setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				&opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
	flag2=0;
	address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
	flag1=1;
	address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian
	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address,
				sizeof(address))<0 && flag1)
	{
		flag1=1;
		flag2=0;
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// Port bind is done. You want to wait for incoming connections and handle them in some way.
	// The process is two step: first you listen(), then you accept()
	if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
	{
		perror("listen");
		flag1=1;
		flag2=0;
		exit(EXIT_FAILURE);
	}

	//changing the current directory to Data to access the files
	int returnDir = chdir("./Data");
	if(returnDir < 0)
	{
		perror("failed to open directory Data");
		exit(0);
	}

	//while loop to run the server indefinitely to accept and process connection requests, messages from clients 

	while(flag1)
	{
		// returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
						(socklen_t*)&addrlen))<0)
		{

			flag1=1;
			flag2=0;
			perror("accept");
			exit(EXIT_FAILURE);
		}

		//to make sure all the variables are empty
		memset(&M, '\0', sizeof(M));
		int val=0;
		flag1=1;
		memset(&M.fileBuffer, '\0', sizeof(M.fileBuffer));
		flag1=1;
		memset(&buffer, '\0', 1024);

		valread = read( new_socket , buffer, 1024);  // read infromation received into the buffer
		printf("%s\n", buffer);

		if(!strcmp(buffer,"listall") && val==0 && flag1 && !flag2)
		{
			int n=0;
			FILE *fp;
			int val2=0;
			flag1=1;
			char command[100]={};
	flag1=1;
	flag2=0;
			char save[1000]={};

			int i=0;
			val=0;
			flag1=1;
			flag2=0;
			fp = popen("ls","r"); 
			while(flag1 && fscanf(fp,"%s",command)!=EOF)
			{
				//  printf("%s\n",command);
				//  n += strlen(command);
				if(n==0)
				{
					strcpy(save,command);
					flag1=1;
					n++;
				}
				else
				{
					flag2=0;
					strcat(save," ");
					strcat(save,command);
				}
			}
			flag1=1;
			M.status = 200;
			M.size = strlen(save);
			flag2=0;
			flag1=1;
			strcpy(M.fileBuffer, save);
			send(new_socket, &M, sizeof(M), 0);
			flag1=1;
			printf("List of files sent\n");
			close(new_socket);
			continue;
		}
		//try to open file sent by the client 
		fd = fopen(buffer,"r" );
		if(fd == NULL && flag1)                      // if no file exists with such name return status 400 to client and exit
		{
			M.status = 400;
			// perror(M.fileBuffer);
			perror("file not found");
			flag1=!flag2;
			send(new_socket, &M, sizeof(M), 0);
			// exit(1);
			close(new_socket);
			continue;
		}

		//to make sure all the variables are empty
		memset(&M, '\0', sizeof(M));
		flag1=1;
		memset(&M.fileBuffer, '\0', sizeof(M.fileBuffer));
		flag1=1;
		flag2=0;

		//read data from the file directly into the fileBuffer of struct variables M
		sizeRead = fread(M.fileBuffer, sizeChar, (size_t)1000, fd);
		while(sizeRead && flag1)
		{
			flag1=1;
			flag2=0;
			M.status = 200;                     //send data in packets to client with status 200
			M.size = sizeRead;
			// printf("sizeRead = %zu\n", sizeRead);
			send(new_socket, &M, sizeof(M), 0);
			flag1=1;
			memset(&M, '\0', sizeof(M));
			flag1=1;
			memset(&M.fileBuffer, '\0', sizeof(M.fileBuffer));
	flag1=1;
	flag2=0;
			sizeRead = fread(M.fileBuffer, sizeChar, (size_t)1000, fd);
		}

		//close both socket and file descriptor after file has been transferred
		fclose(fd);
		flag1=1;
		close(new_socket);
		// send(new_socket , hello , strlen(hello) , 0 );  // use sendto() and recvfrom() for DGRAM
		printf("file sent and socket closed\n");
	}
	return 0;
}
