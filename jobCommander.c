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

// Function to start the server
void start_server() {
    printf("Starting jobExecutorServer...\n");
    pid_t pid = fork(); // Create a child process
    if (pid == 0) { // Child process
        execl("./jobExecutorServer", "jobExecutorServer", NULL); // Execute the server program
        perror("execl"); // Print error in case execl fails
        exit(EXIT_FAILURE); // Exit child process with failure
    } else if (pid > 0) { // Parent process
        server_pid = pid; // Assign server PID
        printf("jobExecutorServer started with PID: %d\n", server_pid);
        FILE *server_file = fopen(SERVER_FILE, "w"); // Open server file for writing
        if (server_file != NULL) { // Check if file opened successfully
            fprintf(server_file, "%d", server_pid); // Write server PID to file
            fclose(server_file); // Close server file
        } else {
            perror("fopen"); // Print error in case fopen fails
            exit(EXIT_FAILURE); // Exit parent process with failure
        }
    } else {
        perror("fork"); // Print error in case fork fails
        exit(EXIT_FAILURE); // Exit parent process with failure
    }
}

// Function to send commands through FIFO
void send_command(char **arr) {
    mkfifo(FIFO_PATH, 0666); // Create FIFO with read/write permissions
    int fd = open(FIFO_PATH, O_WRONLY | O_CREAT, 0644); // Open FIFO for writing
    
    if (fd == -1) { // Check if opening FIFO failed
        perror("Error opening file"); // Print error message in case it does fail
    }

    for (int i = 0; i < 4; i++) { 
        size_t len = strlen(arr[i]);
        if (write(fd, arr[i], len) != len) { // Write command to FIFO
            perror("Error writing to file"); // Print error in case writing fails
        }
        printf("Wrote %s\n", arr[i]); // Print the written command
        usleep(100000); // Wait for a short duration
    }

    close(fd); // Close FIFO
    unlink(FIFO_PATH); // Remove FIFO from the filesystem
}

int main() {
    start_server(); // Start the server

    char *arr[4] = {"ls -l", "ls -l", "ls -l", "ls -l"}; // Define commands
    send_command(arr); // Send commands through FIFO

    return 0;
}
