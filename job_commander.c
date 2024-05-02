#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define SERVER_FILE "jobExecutorServer.txt"
#define FIFO_PATH "jobPipe"
#define MAX_COMMAND_LEN 256

pid_t server_pid = -1;

void signal_handler(int sig) {
    if (sig == SIGCHLD) {
        // Handle SIGCHLD signal
        int status;
        wait(&status);
        printf("Child process terminated.\n");
    }
}

void start_server() {
    printf("Starting jobExecutorServer...\n");
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execl("./jobExecutorServer", "jobExecutorServer", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
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
        // Server file exists
        FILE *server_file = fopen(SERVER_FILE, "r");
        if (server_file != NULL) {
            fscanf(server_file, "%d", &server_pid);
            fclose(server_file);
            printf("jobExecutorServer is active with PID: %d\n", server_pid);
        }
    } else {
        // Server file does not exist, start the server
        start_server();
    }
}

void send_command(const char *command) {
    int fd;
    mkfifo(FIFO_PATH, 0666);
    fd = open(FIFO_PATH, O_WRONLY);
    write(fd, command, strlen(command)+1);
    close(fd);
    unlink(FIFO_PATH);
}

int main() {
    signal(SIGCHLD, signal_handler);
    check_server();

    // Example command to send to jobExecutorServer
    const char *example_command = "./hello";
    send_command(example_command);

    return 0;
}