#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

int createTCPIPv4Socket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in *createIpv4Address(const char *ip, int port)
{
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);

    if (strlen(ip) == 0)
    {
        address->sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        inet_pton(AF_INET, ip, &address->sin_addr.s_addr);
    }

    return address;
}

void handleError(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}
void listenAndPrint(int clientSocketFD){
    char buffer[1024];
    while (true)
    {
        ssize_t amountRecieved = recv(clientSocketFD, buffer, sizeof(buffer), 0);
        if (amountRecieved > 0)
        {
            buffer[amountRecieved] = 0;
            printf("client said: %s", buffer);
        }
        if (amountRecieved == 0)
        {
            break;
        }
    }
    close(clientSocketFD);
}
void startListeningAndPrintMessagesONnewThreaf(int socketFD){
    pthread_t id;
    pthread_create(&id, NULL, listenAndPrint, socketFD);

}
int main()
{
    printf("setting up stack\n");
    const char *ip = "142.250.188.46";
    int socketFD = createTCPIPv4Socket(); // creates a file descriptor
    if (socketFD < 0)
    {
        printf("error bad socket\n");
        exit(1);
    }
    struct sockaddr_in *address = createIpv4Address("127.0.0.1", 2000);
    printf("about to connect\n");
    int result = connect(socketFD, (const struct sockaddr *)address, sizeof *address);

    if (result == 0)
    {
        printf("connection was successful\n");
    }
    char *buffer = NULL;
    size_t linesize = 0;
    printf("type and we will send (type Exit)...");
    startListeningAndPrintMessagesONnewThreaf(socketFD);

    while (true)
    {
        ssize_t charCount = getline(&buffer, &linesize, stdin);
        if (charCount > 0)
        {
            if (strcmp(buffer, "exit\n") == 0)
            {
                break;
            }
            else
            {
                size_t amountsent = send(socketFD, buffer, charCount, 0);
            }
        }
    }

    close(socketFD);
    return 0;
}