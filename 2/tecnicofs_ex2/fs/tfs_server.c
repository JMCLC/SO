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

char pipes[S][40];
int pipe_write[S];

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
    struct input previousInput;
    while (1) {
        struct input received;
        previousInput = received;
        ssize_t read_size = read(rx, &received, sizeof(struct input));
        if (previousInput.code != received.code) {
            if (received.code == '1') {
                printf("code: %c, pipe: %s, readSize: %ld \n", received.code, received.pipe_name, read_size);
                pipe_write[numberOfConnections] = open(pipes[numberOfConnections], O_WRONLY);
                ssize_t write_size;
                int answer = numberOfConnections;
                if (tfs_init() == -1)
                    answer = -1;
                write_size = write(pipe_write[numberOfConnections], &answer, sizeof(answer));
                if (write_size == -1)
                    return -1;
                numberOfConnections++;
            }
        }
    }
}

// int main(int argc, char **argv) {
//     if (argc < 2) {
//         printf("Please specify the pathname of the server's pipe.\n");
//         return 1;
//     }
//     char *pipename = argv[1];
//     printf("Starting TecnicoFS server with pipe called %s\n", pipename);
//     /* TO DO */
//     if (unlink(pipename) != 0 && errno != ENOENT) {
//         printf("Unlinking pipe \n");
//         return -1;
//     }
//     if (mkfifo(pipename, 0640) != 0) {
//         printf("Error creating pipe \n");
//         return -1;
//     }
//     int rx = open(pipename, O_RDONLY);
//     if (rx == -1) {
//         printf("Error opening File \n");
//         return -1;
//     }
//     int numberOfConnections = 0;
//     char lastCommand[BUFFER_SIZE];
//     while (1) {
//         char buffer[BUFFER_SIZE];
//         ssize_t read_size = read(rx, buffer, BUFFER_SIZE);
//         if (strcmp(buffer, lastCommand) != 0) {
//             if (buffer[0] == '1') {
//                 for (int i = 1; i < NAME_SIZE; i++) {
//                     pipes[numberOfConnections][i-1] = buffer[i]; 
//                 }
//                 // printf("Received this: size: %ld info: %s \n", read_size, pipes[numberOfConnections]);
//                 pipe_write[numberOfConnections] = open(pipes[numberOfConnections], O_WRONLY);
//                 // char session[1] = (char)('0' + numberOfConnections);
//                 ssize_t write_size;
//                 if (tfs_init() == -1) {
//                     write_size = write(pipe_write[numberOfConnections], "-1", 2);
//                 } else {
//                     write_size = write(pipe_write[numberOfConnections], "0", 2);
//                 }
//                 if (write_size == -1)
//                     return -1;
//                 numberOfConnections += 1;
//             } else if (buffer[0] == '2') {

//             } else if (buffer[0] == '3') {
//                 // printf("%s \n", buffer);
//                 char name[40] = "";
//                 int flags = -1;
//                 char temp_flag[1] = "";
//                 for (int i = 2; i < NAME_SIZE; i++) {
//                     if (buffer[i + 1] == '\0' || buffer[i + 1] == ' ') {
//                         temp_flag[0] = buffer[i];
//                         break;
//                     } else {
//                         name[i - 2] = buffer[i];
//                     }
//                 }
//                 if (temp_flag[0] == ' ') {
//                     temp_flag[0] = buffer[43];
//                 }
//                 flags = atoi(temp_flag);                
//                 // printf("trying to open: %s with flag: %d \n", name, flags);
//                 int data = tfs_open(name, flags);
//                 int current_session = buffer[1] - '0';  
//                 char output[3];
//                 if (data == -1) {
//                     strcpy(output, "-1");
//                 } else if (data == 0) {
//                     strcpy(output, "0");
//                 } else {
//                     char* temp = "";
//                     while (data != 0) {
//                         int current = data % 10;
//                         *temp = (char) (current + '0');
//                         temp++;
//                         data = data / 10;
//                     }
//                     strcpy(output, temp);
//                 }
//                 ssize_t write_size = write(pipe_write[current_session], output, strlen(output));
//                 if (write_size == -1)
//                     return -1;
//             } else if (buffer[0] == '4') {
//                 // int current_session = buffer[1] - '0';
//                 // char* fhandle_char = "";
//                 // for (int i = 2; i < 4; i++) {
//                 //     if (buffer[i] != '\0') {
//                 //         *fhandle_char = buffer[i];
//                 //         fhandle_char++;
//                 //     } else {
//                 //         break;
//                 //     }
//                 // }
//                 // int fhandle = atoi(fhandle_char);
//                 // ssize_t write_size;
//                 // if (tfs_close(fhandle) == -1) {
//                 //     write_size = write(pipe_write[current_session], "-1", 2);
//                 // } else {
//                 //     write_size = write(pipe_write[current_session], "0", 2);
//                 // }
//                 // if (write_size == -1)
//                 //     return -1;
//             } else if (buffer[0] == '5') {
//                 int current_session = buffer[1] - '0';
//                 int fhandle = buffer[2] - '0';
//                 char* temp = "";
//                 for (int i = 3; i < 8; i++) {
//                     if (buffer[i] >= '0' && buffer[i] <= '9') {
//                         *temp = buffer[i];
//                         temp++;
//                     } else {
//                         break;
//                     }
//                 }
//                 int size = atoi(temp);
//                 char writing_info[size];
//                 for (int i = 3 + (int) strlen(temp); i < BUFFER_SIZE; i++) {
//                     if (buffer[i] == '\0')
//                         break;
//                     writing_info[i - (int) (3 + strlen(temp))] = buffer[i];
//                 }
//                 ssize_t result = tfs_write(fhandle, writing_info, (size_t) size);
//                 ssize_t write_size;
//                 if (result == -1) {
//                     write_size = write(pipe_write[current_session], "-1", 2);
//                 } else {
//                     char* result_str = "";
//                     int temp_result = (int) result;
//                     while (temp_result != 0) {
//                         int current = temp_result % 10;
//                         *result_str = (char) (current + '0');
//                         result_str++;
//                         temp_result = temp_result / 10;
//                     }
//                     write_size = write(pipe_write[current_session], result_str, (size_t) size + 1);
//                 }
//                 if (write_size == -1)
//                     return -1;
//             }
//         }
//         if (read_size == -1)
//             return -1;
//         strcpy(lastCommand, buffer);
//         strcpy(buffer, "");
//     }
//     return 0;
// }