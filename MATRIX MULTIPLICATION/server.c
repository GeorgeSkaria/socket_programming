#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8000
#define MAX_SIZE 3

void multiplyMatrices(int A[MAX_SIZE][MAX_SIZE], int B[MAX_SIZE][MAX_SIZE], int C[MAX_SIZE][MAX_SIZE], int row1, int col1, int row2, int col2) {
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < col2; j++) {
            C[i][j] = 0;
            for (int k = 0; k < col1; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int row1, col1, row2, col2;
    int A[3][3], B[3][3], C[3][3];

    printf("Matrix Multiplication Server\n");

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Socket binding failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listening failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if ((client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Connection failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    } else {
        printf("Connected to client.\n");
    }

    // Receive dimensions of the matrices
    recv(client_fd, &row1, sizeof(int), 0);
    recv(client_fd, &col1, sizeof(int), 0);
    recv(client_fd, &row2, sizeof(int), 0);
    recv(client_fd, &col2, sizeof(int), 0);

    // Receive elements of the matrices
    recv(client_fd, A, row1 * col1 * sizeof(int), 0);
    recv(client_fd, B, row2 * col2 * sizeof(int), 0);

    // Multiply the matrices
    multiplyMatrices(A, B, C, row1, col1, row2, col2);

    // Send the result matrix back to the client
    send(client_fd, C, row1 * col2 * sizeof(int), 0);

    close(client_fd);
    close(server_fd);
    return 0;
}
