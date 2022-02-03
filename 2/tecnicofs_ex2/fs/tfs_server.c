#include "operations.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE (41)
#define S 1

char pipes[S][40];

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Please specify the pathname of the server's pipe.\n");
        return 1;
    }

    char *pipename = argv[1];
    printf("Starting TecnicoFS server with pipe called %s\n", pipename);

    /* TO DO */
    if (unlink(pipename) != 0 && errno != ENOENT) {
        printf("Unlinking pipe \n");
        return -1;
    }

    if (mkfifo(pipename, 0640) != 0) {
        printf("Error creating pipe \n");
        return -1;
    }
    int rx = open(pipename, O_RDONLY);
    if (rx == -1) {
        printf("Error opening File \n");
        return -1;
    }
    int numberOfConnections = 0;
    char lastCommand[BUFFER_SIZE];
    while (1) {
        char buffer[BUFFER_SIZE];
        ssize_t read_size = read(rx, buffer, BUFFER_SIZE);
        if (strcmp(buffer, lastCommand) != 0) {
            if (buffer[0] == '1' && numberOfConnections == 0) {
                printf("Received this: size: %ld info: %s \n", read_size, buffer);
                for (int i = 1; i < BUFFER_SIZE; i++) {
                    pipes[numberOfConnections][i-1] = buffer[i]; 
                }
                numberOfConnections += 1;
                int write_info = open(pipename, O_WRONLY);
                ssize_t write_size = write(rx, "0", 2);
                printf("wrote to client: %ld \n", write_size);
                close(write_info);
            } else if (buffer[0] == '2') {}
        }
        strcpy(lastCommand, buffer);
    }

    return 0;
}