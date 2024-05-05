#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#define SERVER_FILE "jobExecutorServer.txt"
#define FIFO_PATH "jobPipe"
#define MAX_COMMAND_LEN 256

pid_t server_pid = -1;

void signal_handler(int sig) {
    if (sig == SIGCHLD) {
        int status;
        wait(&status);
        printf("Child process terminated.\n");
    }
}

void start_server() {
    printf("Starting jobExecutorServer...\n");
    pid_t pid = fork();
    if (pid == 0) {
        execl("./jobExecutorServer", "jobExecutorServer", NULL);    //check why it doesn't start the server
        perror("execl");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        server_pid = pid;
        printf("jobExecutorServer started with PID: %d\n", server_pid);
        FILE *server_file = fopen(SERVER_FILE, "w");
        if (server_file != NULL) {
            fprintf(server_file, "%d", server_pid);
            fclose(server_file);
        } else {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}

void check_server() {
    if (access(SERVER_FILE, F_OK) != -1) {
        FILE *server_file = fopen(SERVER_FILE, "r");
        if (server_file != NULL) {
            fscanf(server_file, "%d", &server_pid);
            fclose(server_file);
            printf("jobExecutorServer is active with PID: %d\n", server_pid);
        }
    } else {
        start_server();
    }
}

void send_command(char **arr) {
    mkfifo(FIFO_PATH, 0666);
    int fd = open(FIFO_PATH, O_WRONLY | O_CREAT, 0644);
    
    if (fd == -1) {
        perror("Error opening file");
    }

    for (int i = 0; i < 4; i++) {
        size_t len = strlen(arr[i]);
        if (write(fd, arr[i], len) != len) {
            perror("Error writing to file");
        }
        printf("Wrote %s\n", arr[i]);
        usleep(100000);
    }

    close(fd);
    unlink(FIFO_PATH);
}

int main() {
    signal(SIGCHLD, signal_handler);   //last step make the signals
    check_server();

    char *arr[4] = {"./hello", "./hello", "./hello", "./hello"};
    send_command(arr);

    return 0;
}