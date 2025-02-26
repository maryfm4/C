#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>


#define MAX 300
#define PORT 8081
#define INVALID_SOCKET -1
#define CONNECTION_ERROR -1
#define EXIT_COMMAND "exit"


void func(int sockfd)
{
    char buff[MAX];

    while (1)
    {
        bzero(buff, MAX);
        //odczytuje od klienta do bufora
        read(sockfd, buff, sizeof(buff));

        if (strncmp(EXIT_COMMAND, buff, strlen(EXIT_COMMAND)) == 0)
        {
            printf("Exit zrywam polaczenie z klientem...\n"); 
            break;
        }

        write(sockfd, buff, sizeof(buff));
    }
}

int main()
{
    int server_sockfd, client_sockfd;
    socklen_t client_len, server_len;
    struct sockaddr_in server_address, client_address;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == INVALID_SOCKET)
    {
        printf("Utworzenie soketu nie powiodlo sie...\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Soket zostal utworzony..\n");
    }

    //IPv4
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);
    server_len = sizeof(server_address);

    if ((bind(server_sockfd, (struct sockaddr *)&server_address, server_len)) == CONNECTION_ERROR)
    {
        printf("Error nie mozna nadac socketowi adresu...\n");
        exit(EXIT_FAILURE);
    }

    //ile moze oczekiwac na polaczenie
    if ((listen(server_sockfd, 1)) != 0)
    {
        printf("Error...\n");
        exit(EXIT_FAILURE);
    }

    //serwer zamyka sie z dziecmi
    signal(SIGCHLD, SIG_IGN);

    //oczekuje na polaczenie z klienta
    while (1) 
    {
        client_len = sizeof(client_address);
        //akceptuje polaczenie
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
        printf("Polaczylem sie %d\n", client_sockfd);
        if (fork() == 0)
        {
            //klient jest kierowany do obslugi polaczenia
            func(client_sockfd);

            close(server_sockfd);
            exit(0);
        }
        else
        {
            close(client_sockfd);
        }
    }

    exit(EXIT_SUCCESS);
}