 CC = gcc

CFLAGS = -Wall -Wextra -pthread


all: file-server file-client


file-server: file-server.c

$(CC) $(CFLAGS) file-server.c -o file-server


file-client: file-client.c

$(CC) $(CFLAGS) file-client.c -o file-client


clean:

rm -f file-server file-client 