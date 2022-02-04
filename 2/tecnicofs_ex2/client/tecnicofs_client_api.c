#include "tecnicofs_client_api.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int active_session;
int pipe_write;
int pipe_read;

struct input {
    char code, pipe_name[40], name[40], buffer[1024];
    int session_id, flags, fhandle;
    size_t len;
};

int tfs_mount(char const *client_pipe_path, char const *server_pipe_path) {
    // A Resposta do sv talvez tenha de vir do client pipe idk
    printf("Achieved Mount \n");
    if (unlink(client_pipe_path) != 0 && errno != ENOENT) {
        printf("Unlinking pipe \n");
        return -1;
    }
    int pipe_create = mkfifo(client_pipe_path, 0640);
    if (pipe_create != 0) {
        printf("error creating file \n");
        return -1;
    }
    pipe_write = open(server_pipe_path, O_WRONLY);
    // char msg[41] = "1";
    // strcat(msg, client_pipe_path);
    // printf("Tried to write this: %s \n", msg);
    // ssize_t write_size = write(pipe_write, msg, strlen(msg) + 1);
    struct input new_session;
    new_session.code = '1';
    strcpy(new_session.pipe_name, client_pipe_path);
    ssize_t write_size = write(pipe_write, &new_session, sizeof(struct input));
    if (write_size == -1) {
        printf("error writing file \n");
        return -1;
    }
    printf("wrote \n");
    pipe_read = open(client_pipe_path, O_RDONLY);
    // char session_string[1] = "";
    // ssize_t read_size = read(pipe_read, session_string, 1);
    ssize_t read_size = read(pipe_read, &active_session, sizeof(active_session));
    if (read_size == -1) {
        printf("error reading file \n");
        return -1;
    }
    // active_session = (int) (session_string[0] - '0');
    // if (active_session == -1)
    //     return -1;
    printf("Session id: %d \n", active_session);
    return 0;
}

int tfs_unmount() {
    printf("Achieved Unmount \n");

    return -1;
}

int tfs_open(char const *name, int flags) {
    printf("Achieved Open \n");
    char msg[44] = "";
    msg[0] = '3';
    msg[1] = (char) ('0' + active_session);
    strcat(msg, name);
    msg[strlen(name) + 2] = (char) ('0' + flags);
    printf("writing: %s \n", msg);
    ssize_t write_size = write(pipe_write, msg, strlen(msg) + 1);
    if (write_size == -1) {
        printf("error writing file \n");
        return -1;
    }
    printf("wrote \n");
    char buffer[3];
    ssize_t read_size = read(pipe_read, buffer, 3);
    if (read_size == -1) {
        printf("error reading file \n");
        return -1;
    }
    return atoi(buffer);
}

int tfs_close(int fhandle) {
    char output[5];
    char* temp = "";
    output[0] = '4';
    output[1] = (char) ('0' + active_session);
    if (fhandle != 0) {
        while (fhandle != 0) {
            int current = fhandle % 10;
            *temp = (char) (current + '0');
            temp++;
            fhandle = fhandle / 10;
        }
    } else {
        output[2] = '0';
    }
    strcat(output, temp);
    return -1;
}

ssize_t tfs_write(int fhandle, void const *buffer, size_t len) {
    int size = (int) (len);
    char info[size + 4];
    strcpy(info, "");
    char* temp = "";
    info[0] = '5';
    info[1] = (char) ('0' + active_session);
    if (fhandle != 0) {
        while (fhandle != 0) {
            int current = fhandle % 10;
            *temp = (char) (current + '0');
            temp++;
            fhandle = fhandle / 10;
        }
    } else {
        info[2] = '0';
    }
    for (int i = 2; i < size; i++) {
        if (info[i] == '\0') {
            info[i] = (char) ('0' + len);
        }
    }
    strcat(info, buffer);
    ssize_t write_size = write(pipe_write, info, strlen(info) + 1);
    if (write_size == -1) {
        printf("error writing file \n");
        return -1;
    }
    return -1;
}

ssize_t tfs_read(int fhandle, void *buffer, size_t len) {
    /* TODO: Implement this */
    return -1;
}

int tfs_shutdown_after_all_closed() {
    /* TODO: Implement this */
    return -1;
}
