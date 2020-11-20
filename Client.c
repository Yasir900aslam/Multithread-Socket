/*  Make the necessary includes and set up the variables.  */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define PORT 8081

int main(int argc, char *argv[])
{
    int sockfd;
    int len, port;
    struct sockaddr_in address;
    int result;
    char ch;
    int iterations = 1;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(PORT);
    len = sizeof(address);

    result = connect(sockfd, (struct sockaddr *)&address, len);

    if (result == -1)
    {
        perror("oops: client could not connect to server");
        exit(1);
    }

    //receiving menu from server
    char buffer[1024] = {0};
    read(sockfd, buffer, 1024); //reading from the server
    int option,convertedOption ;
    char fs_name[20];
    int LENGTH = 512;
    char sdbuf[LENGTH];
    int fs_block_sz;
    while(1) {
    printf("menu sent by server %s\n", buffer);
    //receiving input from user
    printf("choose service\n");
    scanf("%d", &option);
    //sending input to server
    convertedOption = htonl(option);
    write(sockfd, &convertedOption, sizeof(convertedOption));
    fflush(stdin);
    if (option == 2)
    {
        //****************************************************Uploading file to server*******************************************
        printf("enter file Path :");
        scanf("%s", fs_name);
        // printf("%s",fs_name);
        //**********************send file path to server**************************** 
        printf("Client Sending %s to the Server.....\n ", fs_name);
        FILE *fs = fopen(fs_name, "r");
        if (fs == NULL)
        {
            printf("ERROR: File %s not found.\n", fs_name);
            exit(1);
        }
        bzero(sdbuf, LENGTH);
        while ((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0)
        {
            if (send(sockfd, sdbuf, fs_block_sz, 0) < 0)
            {
                break;
            }
            bzero(sdbuf, LENGTH);
        }

        printf("File %s from Client was Sent!\n", fs_name);
        //server to client ..... sending time
        float x;
        recv(sockfd, &x, sizeof(float), 0);
        printf("File uploaded time taken=%f \n", x);
        //***********************************************************************************
    }
    else if (option == 1)
    {
        float RTT[1000]; 
        int noOfString = 0;

        //*****************************************Echo Request Reply***************************

        while (1)
        {
            char *quit = "q";
            printf("enter string or press quit to quit\n");
            char message[50];
            fflush(stdout);
            bzero(message, 50);
            scanf(" %[^\n]%*c", message);
            //client to server
            write(sockfd, &message, strlen(message));
            
            if (strcmp(message, quit) == 0)
            {

                for (int i = 1; i <= noOfString; i++)
                {
                    printf("Time Taken in Round %d =%f\n", i, RTT[i]);
                }
                break;
            }
            noOfString++;
            //server to cleint
            bzero(message, 50);
            read(sockfd, message, 50);
            printf("string passed by server%s\n", message);

            //client to server
            write(sockfd, &message, strlen(message));
            printf("string passed to server%s\n", message);

            //server to client ..... sending time
            float x;
            recv(sockfd, &x, sizeof(float), 0);
            RTT[noOfString] = x;
        }
    }
    else
    {
        printf("Service not available");
    }
    }
   
}
