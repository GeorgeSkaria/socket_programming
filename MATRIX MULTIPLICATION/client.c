#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8000
#define MAX_SIZE 3

void inputMatrix(int matrix[MAX_SIZE][MAX_SIZE], int row, int col) {
    printf("Enter elements of the matrix (%d x %d):\n", row, col);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            scanf("%d", &matrix[i][j]);
        }
    }
}

void printMatrix(int matrix[MAX_SIZE][MAX_SIZE], int row, int col) {
    printf("Resulting Matrix (%d x %d):\n", row, col);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int client_fd;
    struct sockaddr_in serv_addr;

    int row1, col1, row2, col2;
    int A[MAX_SIZE][MAX_SIZE], B[MAX_SIZE][MAX_SIZE], C[MAX_SIZE][MAX_SIZE];

    printf("Matrix Multiplication Client\n");

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

    // Input matrices
    printf("Enter dimensions of the first matrix (rows cols): ");
    scanf("%d %d", &row1, &col1);
    inputMatrix(A, row1, col1);

    printf("Enter dimensions of the second matrix (rows cols): ");
    scanf("%d %d", &row2, &col2);
    inputMatrix(B, row2, col2);

    if (col1 != row2) {
        printf("Matrix multiplication not possible with given dimensions.\n");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Send dimensions of the matrices
    send(client_fd, &row1, sizeof(int), 0);
    send(client_fd, &col1, sizeof(int), 0);
    send(client_fd, &row2, sizeof(int), 0);
    send(client_fd, &col2, sizeof(int), 0);

    // Send elements of the matrices
    send(client_fd, A, row1 * col1 * sizeof(int), 0);
    send(client_fd, B, row2 * col2 * sizeof(int), 0);

    // Receive the result matrix
    recv(client_fd, C, row1 * col2 * sizeof(int), 0);

    // Print the result matrix
    printMatrix(C, row1, col2);

    close(client_fd);
    return 0;
}
