// server.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8000
#define SIZE 100

typedef struct {
    int data;
    int type; // SEQ (0), ACK (1) or NACK(-1)
    int seq;  // Sequence number
} packet;

int add(int* arr, int key, int index) {
    int flag = -1;
    for (int i = 0; i < index; i++) {
        if (arr[i] == -1) {
            flag = i;
            break;
        }
    }
    arr[index] = key;
    return flag;
}

void handle_client(int client_fd) {
    packet p;
    int arr[SIZE];
    for (int i = 0; i < SIZE; i++) arr[i] = -1;

    while (1) {
        int status = recv(client_fd, &p, sizeof(packet), 0);
        if (status <= 0) {
            if (status == 0) {
                printf("Receive completed. Array: ");
                for (int i = 0; arr[i] != -1; i++) printf("%d ", arr[i]);
                printf("\n");
            } else {
                printf("Receive failed!\n");
            }
            break;
        }

        printf("Received: %d (SEQ %d)\n", p.data, p.seq);
        int index = add(arr, p.data, p.seq);
        if (index != -1) {
            p.type = -1;
            p.seq = index;
            if (send(client_fd, &p, sizeof(packet), 0) >= 0) {
                printf("Sent: NACK %d\n", p.seq);
            } else {
                printf("Send failed!\n");
            }
        }

        p.type = 1;
        if (send(client_fd, &p, sizeof(packet), 0) >= 0) {
            printf("Sent: ACK %d\n", p.seq);
        } else {
            printf("Send failed!\n");
        }
    }
    close(client_fd);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    printf("Selective Repeat ARQ\nTCP Server\n");

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, addrlen) < 0) {
        perror("Socket binding failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listening failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for client...\n");
    while ((client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) >= 0) {
        printf("Connected to client.\n");
        handle_client(client_fd);
    }

    if (client_fd < 0) {
        perror("Connection failed");
    }

    close(server_fd);
    return 0;
}
