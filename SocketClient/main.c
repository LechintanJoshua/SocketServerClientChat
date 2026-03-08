#include "../Util/socketutil.h"

void printNetworkInfoClient(int socketFD, char *message) {
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

    // Afișarea exactă, cuvânt cu cuvânt, așa cum a cerut profesorul
    printf("mesaj de la server: \"%s\"\n", message);
    printf("adresa port sursa: %d\n", localPort);
    printf("adresa port destinatie: %d\n", remotePort);
    printf("adresa ip destinatie: %s\n", remoteIP);
    printf("Adresa ip sursa: %s\n", localIP);
    printf("\n");
}

void* listenAndPrint(void *arg) {
    int socketFD = *(int*)arg;
    free(arg);
    char buffer[1024];

    while(true) {
        ssize_t recievedAmount = recv(socketFD, buffer, 1024, 0);

        if (recievedAmount > 0) {
            buffer[recievedAmount] = 0;

            printNetworkInfoClient(socketFD, buffer);
        }

        if (recievedAmount == 0) { 
            printf("Serverul a fost inchis.\n");
            break;
        }
    }

    close(socketFD);

    return NULL;
}

void startListeningAndPrintMessagesOnNewThread (int socketFD) {
    pthread_t id;

    int *pSocket = malloc(sizeof(int));
    *pSocket = socketFD;

    pthread_create(&id, NULL, listenAndPrint, pSocket);
    
}

int main (int argc, char* argv[]) {
    if (argc < 3) {
        printf("Prea putine argumente, introdu ip-ul si portul", argv[0]);
        return 1;
    }

    int socketFD = createTCPIpv4Socket();

    // In cazul acesta argv[1] este ip-ul si argv[2] este portul
    struct sockaddr_in *address = createIPv4Address(argv[1], atoi(argv[2]));

    int result = connect(socketFD, (struct sockaddr*) address, sizeof(*address));

    if (result == 0) {
        printf("Connection was successful\n");
    } else {
        printf("Connection was unsuccessful.\n");
        return 1;
    }

    // char *name = NULL;
    // size_t nameSize = 0;
    // printf("Te rog introdu numele\n");
    // ssize_t nameCount = getline(&name, &nameSize, stdin);
    // name[nameCount - 1] = 0;

    char *line = NULL;
    size_t lineSize= 0;

    startListeningAndPrintMessagesOnNewThread(socketFD);

    // char buffer[1024];

    while(true) {
        ssize_t charCount = getline(&line, &lineSize, stdin);
        // line[charCount - 1] = 0;

        // sprintf(buffer, "%s: %s", name, line);

        // if (charCount > 0 && strcmp(line, "exit") == 0) {
        //     break;
        // }


        // ssize_t amountWasSent = send(socketFD, buffer, strlen(buffer), 0);
    
        if (charCount > 0 && line[charCount - 1] == '\n') {
            line[charCount - 1] = 0;
            charCount--;
        }

        if (charCount > 0 && strcmp(line, "exit") == 0) {
            break;
        }

        if (charCount > 0) {
            send(socketFD, line, charCount, 0);
        }
    }

    close(socketFD);

    return 0;
}

// ./main 127.0.0.1 2000