#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <libgen.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <host> <port> <command> [args]\n", argv[0]);
        return 1;
} 

    char *host = argv[1];
    int port = atoi(argv[2]);   
    char *cmd = argv[3];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address\n");
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return 1;
    }

if (strcmp(cmd, "ls") == 0) {
send(sock, "ls", 2, 0);

char buffer[BUFFER_SIZE];
ssize_t bytes_received;

while ((bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
buffer[bytes_received] = '\0';
printf("%s", buffer);}
}

else if (strcmp(cmd, "upload") == 0) {
if (argc < 5) {
fprintf(stderr, "Missing local-path for upload\n");
close(sock);
return 1;
}

char *local_path = argv[4];
char *uploaded_name = (argc >= 6) ? argv[5] : basename(local_path);

FILE *fp = fopen(local_path, "rb");
if (!fp) {
perror("Local file open failed");
close(sock);
return 1;
}

char req[512];
snprintf(req, sizeof(req), "upload %s", uploaded_name);
send(sock, req, strlen(req), 0);

usleep(10000);


char buffer[BUFFER_SIZE];
size_t bytes_read;
while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
send(sock, buffer, bytes_read, 0);
}
fclose(fp);
}

else if (strcmp(cmd, "download") == 0) {
    if (argc < 5) {
    fprintf(stderr, "Missing file-name for download\n");
    close(sock);
    return 1;
    }

    char *file_name = argv[4];
    char *local_path = (argc >= 6) ? argv[5] : file_name;

    char req[512];

    snprintf(req, sizeof(req), "download %s", file_name);
    send(sock, req, strlen(req), 0);

    usleep(10000);

    FILE *fp = fopen(local_path, "wb");
    if (!fp) {
    perror("Local file creation failed");
    close(sock);
    return 1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    while ((bytes_received = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, fp);
    }

    fclose(fp);
}
    else {
        fprintf(stderr, "Unknown command: %s\n", cmd);
        close(sock);
        return 1;
    }

    close(sock);
    return 0;
} 