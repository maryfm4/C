#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <endian.h>
#include <stdint.h>

#define MAX 300
#define PORT 8081
#define INVALID_SOCKET -1
#define CONNECTION_ERROR -1
#define EXIT_COMMAND "exit"
#define EMPTY_BUFFER 1

void func(int sockfd)
{
    char buff[MAX];

    while (1)
    {
        bzero(buff, sizeof(buff));
        printf("- Wprowadz wiadomosc do wyslania\n- wpisz 'exit', zeby wyjsc z programu\n:: ");
        scanf("%[^\n]", buff);

        write(sockfd, buff, sizeof(buff));

        if ((strncmp(buff, EXIT_COMMAND, strlen(EXIT_COMMAND))) == 0)
        {
            printf("Exit...\n");
            break;
        }

        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("\nOdpowiedz: %s\n\n", buff);
    }
}

int main()
{
    int sockfd;
    struct sockaddr_in address;
    socklen_t len;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET)
    {
        printf("Utworzenie połączenia nie powiodlo sie...\n");
        exit(0);
    }
    else
    {
        printf("Soket zostal utworzony..\n");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(PORT);
    len = sizeof(address);

    if (connect(sockfd, (struct sockaddr *)&address, len) == CONNECTION_ERROR)
    {
        printf("Polaczenie z serwerem zakonczone niepowodzeniem...\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Polaczono z serwerem..\n");
    }

    func(sockfd);

    close(sockfd);
    exit(EXIT_SUCCESS);
}