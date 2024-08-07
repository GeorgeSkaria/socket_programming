#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define WINDOW_SIZE 3
#define NUM_PACKETS 10
#define SERVER_PORT 49153

int main() {
    // Create socket
    int ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ServerSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Bind
    struct sockaddr_in Server;
    Server.sin_family = AF_INET;
    Server.sin_port = htons(SERVER_PORT);
    Server.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (bind(ServerSocket, (struct sockaddr*)&Server, sizeof(Server)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(ServerSocket, 1) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    // Accept
    socklen_t addrlen = sizeof(Server);
    int ClientAccept = accept(ServerSocket, (struct sockaddr*)&Server, &addrlen);
    if (ClientAccept == -1) {
        perror("Error accepting");
        exit(EXIT_FAILURE);
    }

    // Receive and process packets
    int CountPackets = 0;
    while (CountPackets < NUM_PACKETS) {
        int Frames[WINDOW_SIZE];
        if (recv(ClientAccept, &Frames, sizeof(Frames), 0) == -1) {
            perror("Error receiving");
            exit(EXIT_FAILURE);
        }

        int Random = rand() % NUM_PACKETS;
        int Flag = 0;
        for (int i = 0; i < WINDOW_SIZE; i++) {
            if (Frames[i] == Random) {
                Flag = 1;
                printf("Packet dropped %d, resend it again\n", Random);
                if (send(ClientAccept, &Random, sizeof(Random), 0) == -1) {
                    perror("Error sending");
                    exit(EXIT_FAILURE);
                }
            } else if (Frames[i] != -1) {
                printf("Packet received %d\n", Frames[i]);
                int Acknowledge = 999;
                if (send(ClientAccept, &Acknowledge, sizeof(Acknowledge), 0) == -1) {
                    perror("Error sending acknowledgement");
                    exit(EXIT_FAILURE);
                }
                CountPackets++;
            }
        }

        if (Flag) {
            int Packet;
            if (recv(ClientAccept, &Packet, sizeof(Packet), 0) == -1) {
                perror("Error receiving");
                exit(EXIT_FAILURE);
            }
            printf("Packet received: %d\n", Packet);
            CountPackets++;
        }
    }

    close(ServerSocket);
    return 0;
}
