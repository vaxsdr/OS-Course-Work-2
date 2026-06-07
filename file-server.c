#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <pthread.h>

#define BUFFER_SIZE 4096


void handle_download(int client_sock, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        return;
    }
    char buffer[BUFFER_SIZE];

    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        send(client_sock, buffer, bytes_read, 0);
    }
    fclose(fp);
}



void *client_handler(void *socket_desc) {
    int client_sock = *(int*)socket_desc;
    free(socket_desc); 

    char command_line[512] = {0};
    int idx = 0;
    char ch;

    while (recv(client_sock, &ch, 1, 0) > 0) {
        if (ch == '\n') break;
        if (idx < 511) {
            command_line[idx++] = ch;
        }
    }
    command_line[idx] = '\0';

    char cmd[10] = {0};
    char filename[256] = {0};
    sscanf(command_line, "%s %s", cmd, filename);


    if (strcmp(cmd, "ls") == 0) {
        handle_ls(client_sock);
    }
    else if (strcmp(cmd, "download") == 0) { // kum handle_downl
        handle_download(client_sock, filename);
    }

    
    close(client_sock); 
    return NULL;
}

    int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);   
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }


    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    printf("Socket created successfully on port %d\n", port);
    close(server_sock);

while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) {
            continue;
        }
        
        pthread_t thread_id;
        int *new_sock = malloc(sizeof(int));
        *new_sock = client_sock;

        if (pthread_create(&thread_id, NULL, client_handler, (void*)new_sock) == 0) {
            pthread_detach(thread_id); 
        } else {
            free(new_sock);
            close(client_sock);
        }
    }

    
    close(server_sock);
    return 0;
}