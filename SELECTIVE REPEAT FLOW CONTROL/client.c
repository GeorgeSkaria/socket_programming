#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define WINDOW_SIZE 3
#define NUM_PACKETS 10
#define SERVER_PORT 49153

int main() {
    // Create socket
    int ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ClientSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Connect
    struct sockaddr_in Client;
    Client.sin_family = AF_INET;
    Client.sin_port = htons(SERVER_PORT);
    Client.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(ClientSocket, (struct sockaddr*)&Client, sizeof(Client)) == -1) {
        perror("Error connecting");
        exit(EXIT_FAILURE);
    }

    // Send and receive packets
    int SendPackets = 0, Acknowledge;
    while (SendPackets < NUM_PACKETS) {
        int Frames[WINDOW_SIZE] = {-1, -1, -1};
        for (int i = 0; i < WINDOW_SIZE && SendPackets < NUM_PACKETS; i++) {
            printf("Packet %d Transmitted\n", SendPackets);
            Frames[i] = SendPackets++;
        }

        if (send(ClientSocket, &Frames, sizeof(Frames), 0) == -1) {
            perror("Error sending");
            exit(EXIT_FAILURE);
        }

        int Flag = 0, LostPacket;
        for (int i = 0; i < WINDOW_SIZE; i++) {
            if (recv(ClientSocket, &Acknowledge, sizeof(Acknowledge), 0) == -1) {
                perror("Error receiving acknowledgement");
                exit(EXIT_FAILURE);
            }

            if (Frames[i] == Acknowledge) {
                Flag = 1;
                LostPacket = Acknowledge;
                printf("Packet %d didn't reach server\n", Acknowledge);
            } else if (Frames[i] != -1) {
                printf("Acknowledge received %d for packet %d\n", Acknowledge, Frames[i]);
            }
        }

        if (Flag) {
            printf("Sending Packet %d again\n", LostPacket);
            if (send(ClientSocket, &LostPacket, sizeof(LostPacket), 0) == -1) {
                perror("Error sending");
                exit(EXIT_FAILURE);
            }
        }
    }

    close(ClientSocket);
    return 0;
}
