#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FIFO_PATH "jobPipe"
#define MAX_COMMAND_LEN 256

void handle_command(char *command) {
    pid_t pid = fork(); // Create a new process
    if (pid == -1) {
        //In case Fork fails
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // If the Child process is running we execute the command
        execl("/bin/sh", "sh", "-c", command, NULL);
        // If execl returns, it means it failed
        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        //If Parent process is running
        int status;
        waitpid(pid, &status, 0); // Wait for child process to finish
        printf("Child process finished with status: %d\n", status);
    }
}

int main(int argc, char* argv[]) {
    mkfifo(FIFO_PATH, 0666); // Create the FIFO file with read/write permissions for all
    int fd = open(FIFO_PATH, O_RDONLY); // Open the FIFO file for reading only
    char *arr[4]; // Array to store commands

    if (fd == -1) {
        perror("Error opening file"); // Print error message in case the file fails
        return 1;
    }

    // Read commands from the FIFO file
    for (int i = 0; i < 4; i++) {
        arr[i] = malloc(MAX_COMMAND_LEN + 1); // Allocate memory to store the command
        if (arr[i] == NULL) {
            perror("Error allocating memory"); // Print error message in case memory allocation fails
            return 2; // Return with error code 2
        }
        if (read(fd, arr[i], MAX_COMMAND_LEN) == -1) {
            perror("Error reading from file"); // Print error message in case reading from file fails
            return 3; // Return with error code 3
        }
        arr[i][MAX_COMMAND_LEN] = '\0'; // Null-terminate the command string
        handle_command(arr[i]); // Process the command
        usleep(100000); // Wait for a short duration
    }

    close(fd); // Close the file descriptor
    unlink(FIFO_PATH); // Remove the FIFO file from the filesystem
    return 0; // Return with success code
}