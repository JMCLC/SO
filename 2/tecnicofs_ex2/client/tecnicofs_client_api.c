#include "tecnicofs_client_api.h"
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int active_session;

int tfs_mount(char const *client_pipe_path, char const *server_pipe_path) {
    /* TODO: Implement this */
    printf("Achieved Mount \n");
    mkfifo(client_pipe_path, 0640);
    int sv_pipe_write = open(server_pipe_path, O_WRONLY);
    char msg[41] = "1";
    strcat(msg, client_pipe_path);
    printf("Tried to write this: %s \n", msg);
    ssize_t ret = write(sv_pipe_write, msg, strlen(msg) + 1);
    printf("wrote \n");
    close(sv_pipe_write);
    int sv_pipe_read = open(server_pipe_path, O_RDONLY);
    char buffer[1];
    read(sv_pipe_read, buffer, 1);
    active_session = atoi(buffer);
    printf("Session id: %d \n", active_session);
    return -1;
}

int tfs_unmount() {
    /* TODO: Implement this */
    return -1;
}

int tfs_open(char const *name, int flags) {
    /* TODO: Implement this */
    return -1;
}

int tfs_close(int fhandle) {
    /* TODO: Implement this */
    return -1;
}

ssize_t tfs_write(int fhandle, void const *buffer, size_t len) {
    /* TODO: Implement this */
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
