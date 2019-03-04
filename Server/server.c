#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  // sockaddr_in - references elements of the socket address. "in" for internet
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc.
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening.\n");
    	if (listen(server_fd, 10) < 0) // 10 is the maximum size of queue - connections you haven't accepted
	    {
	        perror("listen");
	        exit(EXIT_FAILURE);
	    }

	    /*for every new client we are creating a new child process and in this way we will be able to support multiple clients*/
	 pid_t childpid;
	while(1)
	{
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)//accept new sockets
	    {
	        perror("accept");
	        exit(EXIT_FAILURE);
	    }
	    if((childpid = fork())==0)//child process
    	{
    		close(server_fd);
    		while(1)
    		{
    			//actual server work
		    	char buffer[1024] = {0};
				valread = read( new_socket , buffer, 1024);  // read infromation received into the buffer
			   	if(strcmp(buffer,"close")==0){
			    	printf("Closing connection with IP %d\n",new_socket);
			    	close(new_socket);
			    	break;//close the connectioin
			    }
			    else if(strcmp(buffer,"listall")==0)
			    {
			    	//to list all files present in server's folder
			    	printf("Sending list of all the files\n");
			    	char lists[50][1024];
			    	struct dirent *file;//to iterate to files listss
			    	DIR *files=opendir(".");//open current directory
			    	int i=0;
			    	while((file=readdir(files))!=NULL)
			    	{
			    		strcpy(lists[i++],file->d_name);//read till files are there and store it in 2-d array lists
			    	}
			    	lists[i][0]='\0';
			    	char list[10000];//1-d array of list of files name that would be sent
			    	i=2;
			    	int j=0;
			    	while(1)
			    	{
			    		if(lists[i][0]=='\0')
			    			break;//last file name
			    		for(int k=0;k<strlen(lists[i]);k++)
			    			list[j++]=lists[i][k];//converting 2d array into 1d
			    		list[j++]='\n';//adding new line so that files name appear in seperate lines
			    		i++;
			    	}
			    	list[j-1]='\0';
			    	free(file);
			    	free(files);
			    	send(new_socket,list,strlen(list),0);//send file names
			    }
			    else if(strcmp(buffer,"send")==0)
			    {
			    	send(new_socket,"connected",sizeof("connected"),0);//acknowledgement sent
			    	char file_name[1024] = {0};
					valread = read( new_socket , file_name, 1024);//recive file name, this is an acknowlegment from client
			    	file_name[strlen(file_name)]='\0';
			    	FILE *file=fopen(file_name,"r");//open the corresponding file
			    	if(file==NULL)
			    	{
			    		send(new_socket,"No such file",strlen("No such file"),0);//file does not exists
			    	}
			    	else
			    	{
			    		send(new_socket,"Sending",strlen("Sending"),0);
			    		printf("Sending %s to the client\n",file_name);
			    		char tra_buffer[258];
			    		int tra_len=0;
			    		while((tra_len = fread(tra_buffer,sizeof(char),256,file))>0)//read file and if there is something to be read then return value is greater than 0. we send 256 bytes at a time
			    		{
			    			send(new_socket,tra_buffer,tra_len,0);
			    			if(tra_len < 256)
		                    {//tra_len less than 256 that means that there some error or eof file is reached
		                        if (feof(file))
		                        {
		                            printf("End file\n");//file end
		                        }
		                        if (ferror(file))
		                        {
		                            printf("Error reading file\n");//error
		                        }
		                        break;
		                    }
			    		}
			    		printf("File send succesfully.\n");
			    		fclose(file);
			    	}
			    	//send(new_socket,a,strlen(a),0);
			    }
			    //exit(0);
			}
		}
	}
    return 0;
}
