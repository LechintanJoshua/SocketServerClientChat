#include "../Util/socketutil.h"

struct AcceptedSocket {
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
};

void printNetworkInfoServer(int socketFD, char *message) {
    struct sockaddr_in localAddress, remoteAddress;
    socklen_t localLen = sizeof(localAddress);
    socklen_t remoteLen = sizeof(remoteAddress);

    getsockname(socketFD, (struct sockaddr*)&localAddress, &localLen);
    getpeername(socketFD, (struct sockaddr*)&remoteAddress, &remoteLen);

    char localIP[INET_ADDRSTRLEN];
    char remoteIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(localAddress.sin_addr), localIP, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(remoteAddress.sin_addr), remoteIP, INET_ADDRSTRLEN);

    int localPort = ntohs(localAddress.sin_port);
    int remotePort = ntohs(remoteAddress.sin_port);

    printf("mesaj de la client: %s\n", message);
    printf("port sursa: %d\n", remotePort);
    printf("port destinatie: %d\n", localPort);
    printf("adresa ip-sursa: %s\n", remoteIP);
    printf("adresa ip-destinatie: %s\n", localIP);
    printf("\n");
}

struct AcceptedSocket* acceptIncomingConnection(int serverSocketFD) {
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, &clientAddressSize);

    struct AcceptedSocket* acceptedSocket = (struct AcceptedSocket*) malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->acceptedSuccessfully = clientSocketFD > 0;

    if (!acceptedSocket->acceptedSuccessfully) {
        acceptedSocket->error = clientSocketFD;
    }

    return acceptedSocket;
}

void* recieveAndPrintIncommingData (void *arg) {
    struct AcceptedSocket *pSocket = (struct AcceptedSocket*) arg;
    int socketFD = pSocket->acceptedSocketFD;
    char buffer[1024];

    while(true) {
        ssize_t recievedAmount = recv(socketFD, buffer, 1024, 0);

        if (recievedAmount > 0) {
            buffer[recievedAmount] = 0;

            printNetworkInfoServer(socketFD, buffer);

            send(socketFD, buffer, strlen(buffer), 0);
        }

        if (recievedAmount == 0) { 
            printf("Client deconectat.\n");
            break;
        }
    }

    close(socketFD);
    free(pSocket);

    return NULL;
}

void recieveAndPrintIncommingDataOnSeparateThread (struct AcceptedSocket* pSocket) {
    pthread_t id;
    pthread_create(&id, NULL, recieveAndPrintIncommingData, pSocket);
}

void startAcceptingIncomingConnection (int serverSocketFD) {
    while(true) {
        struct AcceptedSocket* clientSocket = acceptIncomingConnection(serverSocketFD);
        
        recieveAndPrintIncommingDataOnSeparateThread(clientSocket);
    }    
}

int main () {
    int serverSocketFD = createTCPIpv4Socket();
    struct sockaddr_in *serverAddres = createIPv4Address("", 2000);

    int result = bind(serverSocketFD, (struct sockaddr *)serverAddres, sizeof(*serverAddres));

    if (result == 0) {
        printf("Socket was bound successfuly.\n");
    }

    int listenResult = listen(serverSocketFD, 10);

    startAcceptingIncomingConnection(serverSocketFD);
    shutdown(serverSocketFD, SHUT_RDWR);

    return 0;
}

//lsof -i :2000