#include "operations.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define NAME_SIZE 40
#define S 1

struct input {
    char code, pipe_name[40], name[40], buffer[1024];
    int session_id, flags, fhandle;
    size_t len;
};

struct read {
    char buffer[1024];
    ssize_t len;
};

char pipes[S][40];
int pipe_write[S];

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Please specify the pathname of the server's pipe.\n");
        return 1;
    }
    char *pipename = argv[1];
    printf("Starting TecnicoFS server with pipe called %s\n", pipename);

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
    struct input previousInput;
    while (1) {
        struct input received;
        previousInput = received;
        ssize_t read_size = read(rx, &received, sizeof(struct input));
        if (previousInput.code != received.code) {
            if (received.code == '1') {
                strcpy(pipes[numberOfConnections], received.pipe_name);
                pipe_write[numberOfConnections] = open(pipes[numberOfConnections], O_WRONLY);
                int answer = numberOfConnections;
                if (tfs_init() == -1)
                    answer = -1;
                ssize_t write_size = write(pipe_write[numberOfConnections], &answer, sizeof(answer));
                if (write_size == -1)
                    return -1;
                numberOfConnections++;
            } else if (received.code == '2') {
                int answer = close(pipe_write[received.session_id]);
                ssize_t write_size = write(pipe_write[received.session_id], &answer, sizeof(answer));
                if (write_size == -1)
                    return -1;
            } else if (received.code == '3') {
                int answer = tfs_open(received.name, received.flags);
                ssize_t write_size = write(pipe_write[received.session_id], &answer, sizeof(answer));
                if (write_size == -1)
                    return -1;
            } else if (received.code == '4') {
                int answer = tfs_close(received.fhandle);
                ssize_t write_size = write(pipe_write[received.session_id], &answer, sizeof(answer));
                if (write_size == -1)
                    return -1;
            } else if (received.code == '5') {
                char buffer[received.len];
                for (size_t i = 0; i < received.len; i++)
                    buffer[i] = received.buffer[i];
                int answer = (int) tfs_write(received.fhandle, buffer, received.len);
                ssize_t write_size = write(pipe_write[received.session_id], &answer, sizeof(answer));
                if (write_size == -1)
                    return -1;
            } else if (received.code == '6') {
                struct read answer;
                answer.len = tfs_read(received.fhandle, answer.buffer, received.len);   
                ssize_t write_size = write(pipe_write[received.session_id], &answer, sizeof(answer));
                if (write_size == -1)
                    return -1;
            } else if (received.code == '7') {
                int answer = tfs_destroy_after_all_closed();
                ssize_t write_size = write(pipe_write[received.session_id], &answer, sizeof(answer));
                if (write_size == -1)
                    return -1;
            }
        }
        if (read_size == -1)
            return -1;
    }
    return 0;
}