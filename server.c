#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

struct AcceptedSocket; // Forward declaration
struct AcceptedSocket
{
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
};
struct AcceptedSocket acceptedSockets[10];
int acceptedSocketCount = 0;
void sendRecievedMessagesToOtherClients(char *buffer, int socketFD)
{
    // itereate over all
    for (int i = 0; i < acceptedSocketCount; i++)
    {
        if (acceptedSockets[i].acceptedSocketFD != socketFD)
        {
            send(acceptedSockets[i].acceptedSocketFD, buffer, strlen(buffer), 0);
        }
    }
}
void recieveAndPrintIncomingData(int clientSocketFD)
{
    char buffer[1024];
    while (true)
    {
        ssize_t amountRecieved = recv(clientSocketFD, buffer, sizeof(buffer), 0);
        if (amountRecieved > 0)
        {
            buffer[amountRecieved] = 0;
            printf("client said: %s", buffer);
            sendRecievedMessagesToOtherClients(buffer, clientSocketFD);
        }
        if (amountRecieved == 0)
        {
            break;
        }
    }
    close(clientSocketFD);
}
void recieveAndPrintIncomingDataOnSeperateThread(struct AcceptedSocket *clientSocket)
{
    pthread_t id;
    pthread_create(&id, NULL, recieveAndPrintIncomingData, clientSocket->acceptedSocketFD);
}
struct AcceptedSocket *acceptIncomingConnection(int serverSocketFD)
{
    struct sockaddr_in clientaddress;
    int clientAddressSize = sizeof(clientaddress);
    int clientSocketFD = accept(serverSocketFD, &clientaddress, &clientaddress);

    struct AcceptedSocket *acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->address = clientaddress;
    acceptedSocket->acceptedSuccessfully = clientSocketFD > 0;

    if (acceptedSocket->acceptedSuccessfully == false)
    {
        acceptedSocket->error = clientSocketFD;
    }
    return acceptedSocket;
}

void startAcceptingIncomingConnections(int serverSocketFD)
{
    while (true)
    {
        struct AcceptedSocket *clientSocket = acceptIncomingConnection(serverSocketFD);
        acceptedSockets[acceptedSocketCount++] = *clientSocket;
        recieveAndPrintIncomingDataOnSeperateThread(clientSocket);
    }
}

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

int main()
{
    printf("Starting server\n");

    int serverSocketFD = createTCPIPv4Socket();
    if (serverSocketFD == -1)
    {
        handleError("Error creating socket");
    }
    else
    {
        printf("successfully created socket\n");
    }

    struct sockaddr_in *serverAddress = createIpv4Address("", 2000);
    int result = bind(serverSocketFD, (const struct sockaddr *)serverAddress, sizeof *serverAddress);

    if (result == -1)
    {
        handleError("Error binding socket");
    }
    else
    {
        printf("succesfully binding to socket\n");
    }

    int listenResult = listen(serverSocketFD, 10);
    if (listenResult == -1)
    {
        handleError("Error listening on socket");
    }
    else
    {
        printf("succesfully listening to socket\n");
    }

    startAcceptingIncomingConnections(serverSocketFD);
    shutdown(serverSocketFD, SHUT_RDWR);
    return 0;
}
