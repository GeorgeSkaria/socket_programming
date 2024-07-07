// client.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8000

typedef struct {
    int data;
    int type; // SEQ (0), ACK (1) or NACK(-1)
    int seq;  // Sequence number
} packet;

typedef struct {
    int size;
    int start;
    int end;
} window;

void send_window(int client_fd, int* arr, int n, packet* p, window* w) {
    for (p->seq = w->start; p->seq <= w->end && p->seq < n; p->seq++) {
        p->type = 0;
        p->data = arr[p->seq];
        if (send(client_fd, p, sizeof(packet), 0) >= 0) {
            printf("Sent: %d (SEQ %d)\n", p->data, p->seq);
        } else {
            printf("Send failed!\n");
        }
    }
}

void send_frame(int client_fd, int* arr, int seq, packet* p) {
    p->type = 0;
    p->data = arr[seq];
    p->seq = seq;
    if (send(client_fd, p, sizeof(packet), 0) >= 0) {
        printf("Sent: %d (SEQ %d)\n", p->data, p->seq);
    } else {
        printf("Send failed!\n");
    }
}

void recv_ack(int client_fd, int* arr, int n, packet* p, window* w) {
    while (1) {
        if (recv(client_fd, p, sizeof(packet), 0) < 0) {
            printf("Time out! Window retransmitting.\n");
            send_window(client_fd, arr, n, p, w);
            continue;
        }

        if (p->type == 1) {
            printf("Received: ACK %d\n", p->seq);
            arr[p->seq] = -1;
            w->start++;
            w->end++;
            if (w->end < n) send_frame(client_fd, arr, w->end, p);
            else break;
        } else if (p->type == -1) {
            printf("Received: NACK %d. Retransmitting SEQ %d.\n", p->seq, p->seq);
            send_frame(client_fd, arr, p->seq, p);
        }
    }
}

int main() {
    int client_fd;
    struct sockaddr_in serv_addr;

    printf("TCP Client\n");

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    int n, window_size;
    printf("Enter window size: ");
    scanf("%d", &window_size);
    printf("Enter array size: ");
    scanf("%d", &n);

    int arr[n];
    printf("Enter array elements: ");
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    packet p;
    window w = {window_size, 0, window_size - 1};

    send_window(client_fd, arr, n, &p, &w);
    recv_ack(client_fd, arr, n, &p, &w);

    close(client_fd);
    return 0;
}
