#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define PORT 8081
void *thread_connection(void *); // Thread function
int c = 0;                       // For numbering and keeping track of our clients
int iterations = 1;

int main(int argc, char *argv[])
{
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    pthread_t tid;
    int port;

    // passing port number as an argument
    /* Avoid giving special port numbers */
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1)
    {
        perror("Could not create server socket");
        exit(2);
    }

    //sigaction(SIGCHLD, &sa, NULL);
    server_address.sin_family = AF_INET; //IPv4 stream socket family
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    printf("listening on %d \n", PORT);

    server_len = sizeof(server_address);
    /* Binding server to the IP address */
    if (bind(server_sockfd, (struct sockaddr *)&server_address, server_len) == -1)
    {
        perror("Unable to bind");
        exit(2);
    }

    /*  Create a connection queue, ignore child exit details and wait for clients.  */

    listen(server_sockfd, 5); // Waits or listens for connection
    printf("server waiting\n");
    client_len = sizeof(client_address);

    /* While loop runs for every client connection ACCEPTED by the server */
    while ((client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len)))
    {
        c++; // For numbering connected clients
        printf("Connection Established with client %d\n", c);

        /* Creating thread for every client connection ACCEPTED */
        if (pthread_create(&tid, NULL, thread_connection, (void *)&client_sockfd) < 0)
        {                                 /* Passing the thread is the CLIENT SOCKET */
            perror("Thread not created"); /* with which server and client can communicate in thread function */
            //return 1;
            break;
        }
        printf("Thread assigned to client %d \n", c);
    }

    if (client_sockfd < 0)
    {
        perror("accept falied");
        return 1;
        //break;
    }
    return 0;
}

void *thread_connection(void *client_sockfd)
{
    char ch;
    /* Converting client_sockfd back to integer type and assigning it to client_sock */
    int client_sock = *(int *)client_sockfd;
    /* Now using client_scok for server-to-client communication */

    //server sending menu
    char *menu = "\n1.echo \n2.upload";
    send(client_sock, menu, strlen(menu), 0);

    //server receiving option from client
    int option;

    int recv_option = 0;

    int return_status = read(client_sock, &recv_option, sizeof(recv_option));
    if (return_status > 0)
    {
        option = ntohl(recv_option);
    }
    else
    {
        printf("Error Occured");
        return;
    }
    //fflush(stdin);
    if (option == 2)
    {
        clock_t time;
        char file[2000] = {0};
        //************************************reading entire file**********************
        int LENGTH = 1024;
        char revbuf[LENGTH]; // Receiver buffer
        //creating file name
        time = clock();
        int ft = (int)time;
        char fr_name[30];
        sprintf(fr_name, "%dServer%d.txt", c, ft);
        FILE *fr = fopen(fr_name, "a");
        if (fr == NULL)
            printf("File %s Cannot be opened file on server.\n", fr_name);
        else
        {
            bzero(revbuf, LENGTH);
            int fr_block_sz = 0;
            while ((fr_block_sz = recv(client_sock, revbuf, LENGTH, 0)) > 0)
            {
                int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);

                bzero(revbuf, LENGTH);
                if (fr_block_sz == 0 || fr_block_sz != 512)
                {
                    break;
                }
            }

            time = clock() - time;
            float times = (float)time / CLOCKS_PER_SEC;
            float x = times;
            send(client_sock, &x, sizeof(float), 0);
            fclose(fr);
        }

        //**********************************************************************************
    }
    else if (option == 1)
    {
        //*************************************ECHO REQUEST*******************************
        float RTT[1000];
        int noOfString = 0;
        int strLength = 100;
        char string[strLength];
        char *quit = "q";

        while (1)
        {
            bzero(string, strLength);

            //reading from client
            read(client_sock, string, strLength);
            if (strcmp(string, quit) == 0)
            { //for(int i=1;i<=noOfString;i++)
                //{printf("Time Taken in Round %d =%f\n",i,RTT[i]);}
                break;
            }
            // printf("string sent by client %s\n",string);

            //sending string to client
            //timer shoud start
            noOfString++;
            clock_t time1;
            time1 = clock();
            write(client_sock, &string, strlen(string));
            printf("string sent to client %s\n", string);

            //reading from client
            read(client_sock, string, strLength);
            RTT[noOfString] = (float)(clock() - time1) / CLOCKS_PER_SEC;
            printf("string sent by client %s\n", string);
            printf("round complete\n");

            //sending time to client
            float x = RTT[noOfString];
            send(client_sock, &x, sizeof(float), 0);
        }
    }

    else
    {

        printf("not available yet \n");
    }

    printf("\n Connection is now closed with client %d\n", c);
    c--; // keeping track of our client
}
