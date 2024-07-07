#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8000

void get(int client_fd) {
    char filename[100];
    char filecontent[1000];
    char response[100];
    int pid;

    printf("Enter filename: ");
    scanf("%s", filename);

    // Send the filename to the server
    if (send(client_fd, filename, sizeof(filename), 0) <= 0) {
        printf("GET failed!\n");
        return;
    }

    // Receive the file content from the server
    if (recv(client_fd, filecontent, sizeof(filecontent), 0) <= 0) {
        printf("GET failed!\n");
        return;
    }

    // Receive the server's process ID
    if (recv(client_fd, &pid, sizeof(pid), 0) <= 0) {
        printf("GET failed!\n");
        return;
    }

    // Receive the response code
    if (recv(client_fd, response, sizeof(response), 0) <= 0) {
        printf("GET failed!\n");
        return;
    }

    // Check if the response is "200 OK"
    if (strcmp(response, "200 OK") == 0) {
        FILE *fp = fopen(filename, "w");
        if (fp != NULL) {
            fputs(filecontent, fp);
            fclose(fp);
            printf("GET successful (PID %d)!\n", pid);
        } else {
            printf("Failed to write file.\n");
        }
    } else {
        printf("GET failed (PID %d)!\n", pid);
    }
}

int main() {
    int client_fd;
    struct sockaddr_in serv_addr;

    printf("File Client\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);

    while (1) {
        // Create socket
        client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd < 0) {
            printf("Socket creation failed!\n");
            exit(1);
        }

        // Connect to the server
        if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            printf("Connection failed!\n");
            close(client_fd);
            exit(1);
        }

        get(client_fd);
        close(client_fd);
    }

    return 0;
}
