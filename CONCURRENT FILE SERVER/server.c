#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8000

void serverRecv(int client_fd) {
    char filename[100];
    char filecontent[1000];

    // Receive the filename from the client
    if (recv(client_fd, filename, sizeof(filename), 0) <= 0) {
        printf("Connection lost!\n");
        return;
    }

    FILE *fp = fopen(filename, "r");
    int pid = getpid();

    if (fp == NULL) {
        // File not found, send an empty response and error code
        filecontent[0] = '\0';
        send(client_fd, filecontent, sizeof(char), 0);
        send(client_fd, &pid, sizeof(int), 0);
        send(client_fd, "400 BAD", 8 * sizeof(char), 0);
        printf("GET failed: File not found.\n");
        return;
    }

    // Read file content into buffer
    filecontent[0] = '\0';
    while (fgets(filename, sizeof(filename), fp) != NULL) {
        strcat(filecontent, filename);
    }
    fclose(fp);

    // Send file content and success response
    send(client_fd, filecontent, sizeof(filecontent), 0);
    send(client_fd, &pid, sizeof(int), 0);
    send(client_fd, "200 OK", 7 * sizeof(char), 0);
    printf("GET successful!\n");
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    printf("Concurrent File Server\n");

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up address struct
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Socket binding failed");
        exit(1);
    }

    // Listen for connections
    if (listen(server_fd, 5) < 0) {
        perror("Listening failed");
        exit(1);
    }

    while (1) {
        // Accept new connection
        if ((client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Connection failed");
            exit(1);
        }
        printf("Connected to client.\n");

        // Create child process to handle the connection
        if (fork() == 0) {
            close(server_fd);
            serverRecv(client_fd);
            close(client_fd);
            exit(0);
        } else {
            close(client_fd);
        }
    }

    close(server_fd);
    return 0;
}

