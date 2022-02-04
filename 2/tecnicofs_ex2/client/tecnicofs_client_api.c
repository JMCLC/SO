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

struct read {
    char buffer[1024];
    ssize_t len;
};

int tfs_mount(char const *client_pipe_path, char const *server_pipe_path) {
    printf("Achieved Mount \n");
    if (unlink(client_pipe_path) != 0 && errno != ENOENT) {
        printf("Unlinking pipe \n");
        return -1;
    }
    int pipe_create = mkfifo(client_pipe_path, 0640);
    if (pipe_create != 0)
        return -1;
    pipe_write = open(server_pipe_path, O_WRONLY);
    struct input new_request;
    new_request.code = '1';
    strcpy(new_request.pipe_name, client_pipe_path);
    ssize_t write_size = write(pipe_write, &new_request, sizeof(struct input));
    if (write_size == -1)
        return -1;
    pipe_read = open(client_pipe_path, O_RDONLY);
    ssize_t read_size = read(pipe_read, &active_session, sizeof(active_session));
    if (read_size == -1)
        return -1;
    printf("Session id: %d \n", active_session);
    return 0;
}

int tfs_unmount() {
    printf("Achieved Unmount \n");
    struct input request;
    request.code = '2';
    request.session_id = active_session;
    printf("writing\n");
    ssize_t write_size = write(pipe_write, &request, sizeof(request));
    if (write_size == -1)
        return -1;
    int close_try = close(pipe_write);
    if (close_try == -1)
        return -1;
    int answer;
    ssize_t read_size = read(pipe_read, &answer, sizeof(answer));
    printf("answer: %d \n", answer);
    int close_2 = close(pipe_read);
    if (read_size == -1 || answer == -1 || close_2 == -1) 
        return -1;
    return 0;
}

int tfs_open(char const *name, int flags) {
    printf("Achieved Open \n");
    struct input request;
    request.code = '3';
    request.session_id = active_session;
    strcpy(request.name, name);
    request.flags = flags;
    printf("writing\n");
    ssize_t write_size = write(pipe_write, &request, sizeof(request));
    if (write_size == -1)
        return -1;
    printf("wrote \n");
    int answer;
    ssize_t read_size = read(pipe_read, &answer, sizeof(answer));
    printf("answer: %d \n", answer);
    if (read_size == -1) 
        return -1;
    return answer;
}

int tfs_close(int fhandle) {
    printf("Achieved Close \n");
    struct input request;
    request.code = '4';
    request.session_id = active_session;
    request.fhandle = fhandle;
    printf("writing\n");
    ssize_t write_size = write(pipe_write, &request, sizeof(request));
    if (write_size == -1)
        return -1;
    printf("wrote \n");
    int answer;
    ssize_t read_size = read(pipe_read, &answer, sizeof(answer));
    printf("answer: %d \n", answer);
    if (read_size == -1) 
        return -1;
    return answer;
}

ssize_t tfs_write(int fhandle, void const *buffer, size_t len) {
    printf("Achieved Write \n");
    struct input request;
    request.code = '5';
    request.session_id = active_session;
    request.fhandle = fhandle;
    request.len = len;
    strcpy(request.buffer, buffer);
    printf("writing\n");
    ssize_t write_size = write(pipe_write, &request, sizeof(request));
    if (write_size == -1)
        return -1;
    printf("wrote \n");
    int answer;
    ssize_t read_size = read(pipe_read, &answer, sizeof(answer));
    printf("answer: %d \n", answer);
    if (read_size == -1) 
        return -1;
    return answer;
}

ssize_t tfs_read(int fhandle, void *buffer, size_t len) {
    printf("Achieved Read \n");
    struct input request;
    request.code = '6';
    request.session_id = active_session;
    request.fhandle = fhandle;
    request.len = len;
    printf("writing\n");
    ssize_t write_size = write(pipe_write, &request, sizeof(request));
    if (write_size == -1)
        return -1;
    printf("wrote \n");
    struct read answer;
    ssize_t read_size = read(pipe_read, &answer, sizeof(answer));
    printf("answer: %s with size: %ld \n", answer.buffer, answer.len);
    if (read_size == -1) 
        return -1;
    memcpy(buffer, answer.buffer, len);
    return answer.len;
}

int tfs_shutdown_after_all_closed() {
    printf("Achieved Shutdown \n");
    struct input request;
    request.code = '7';
    request.session_id = active_session;
    ssize_t write_size = write(pipe_write, &request, sizeof(request));
    if (write_size == -1)
        return -1;
    int answer;
    ssize_t read_size = read(pipe_read, &answer, sizeof(answer));
    if (read_size == -1) 
        return -1;
    return answer;
}
