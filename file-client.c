#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <arpa/inet.h>


#define BUFFER_SIZE 4096

void handle_ls(int client_sock) {
    DIR *d = opendir("."); 
    struct dirent *dir;
    char buffer[BUFFER_SIZE] = {0};

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
                continue;
            }
            strcat(buffer, dir->d_name);
            strcat(buffer, "\n"); 
        }
        closedir(d);
    }
    send(client_sock, buffer, strlen(buffer), 0);
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

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(1);
    }

    if (listen(server_sock, 10) < 0) {
        perror("Listen failed");
        close(server_sock);
        exit(1);
    }


    printf("Listening on %d\n", port);
    fflush(stdout);

    close(server_sock);
    return 0;
}