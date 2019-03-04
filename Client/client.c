// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 8080
char ** break_input(char * c)
{
    char ** ret = malloc(10*sizeof(char*));
    char *token = strtok(c," \t\n");//tokenizing based on newline or tab characters
    int i=0;
    while(token!=NULL)
    {
        ret[i]=token;
        i++;
        token = strtok(NULL," \t\n");
    }
    ret[i]=NULL;
    return ret;//returning the tokenized commands.

}
int main(int argc, char const *argv[])
{
        struct sockaddr_in address;
        int sock = 0, valread;
        struct sockaddr_in serv_addr;
        
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Socket creation error \n");
            return -1;
        }

        memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                    // which is meant to be, and rest is defined below

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        // Converts an IP address in numbers-and-dots notation into either a
        // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
        if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
        {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
        {
            printf("\nConnection Failed \n");
            return -1;
        }
        else
        {
            while(1)
            {
                printf(">> ");
                
                char input[1000];
                char *temp;
                size_t len;
                //get line function will read whitespaces also
                getline(&temp,&len,stdin);
                //we then need to convert it into actual characters
                strcpy(input,temp);
                int length = strlen(input);
                input[length]='\0';
                //break_input function will break the input based on spaces i.e will tokenize on the basis of 
                //whitespaces so that arguments could be sperated.
                char **args = break_input(input);
                int size=0;
                while(args[size]!=NULL)
                    size++;
                if(strcmp(args[0],"close")==0)
                {
                    break;
                    //will break the connection
                }
                else if(strcmp(args[0],"listall")==0)
                {
                    char buffer[10240] = {0};
                    send(sock , args[0] , sizeof(args[0]) , 0 );
                    valread = read( sock , buffer, 10240);  // receive message back from server, into the buffer
                    printf("%s\n",buffer);
                }
                else if(strcmp(args[0],"send")==0)
                {
                    char buffer[10240] = {0};
                    if(size ==1)
                    {
                        // only send command entered
                        printf("No File Entered\n");
                        continue;
                    }
                    else if(size>2)
                    {
                        // multiple files entered.
                        printf("Multiple Files Entered(Only 1 Allowed)\n");
                        continue;
                    }
                    send(sock , args[0] , sizeof(args[0]) , 0 );//inform server about the send command
                    valread = read( sock , buffer, 10240);  // receive acknowledgment
                    send(sock , args[1], sizeof(args[1]),0);//send file name
                    valread = read( sock , buffer, 10240);//recived acknowlegment
                    if(strcmp(buffer,"No such file")==0)
                    {
                        //file does not exist
                        printf("No such file\n");
                        continue;
                    }
                    //file exits
                    FILE * file;
                    file=fopen(args[1],"w");//open a file for writing, if a file already exists then it will overwrite it
                    //printf("SFD");
                    while(1)
                    {
                        //will recieve 256 bytes at a time
                        char rec_buffer[258]={0};
                        valread = read(sock,rec_buffer,256);
                        if(valread<=0)//failure in reciving or recieved nothing
                            break;
                        fwrite(rec_buffer,sizeof(char),valread,file);
                        if(valread!=256)
                            break;//if less than 256 then eof is reached
                    }
                    if(valread<0)
                        printf("Downloading Error\n");
                    else
                    {
                        printf("File downloaded successfully\n");
                        fclose(file);
                    }
                    fclose(file);
                }
                else
                {
                    printf("Invalid Command\n");
                }
            }
        }
    return 0;
}
