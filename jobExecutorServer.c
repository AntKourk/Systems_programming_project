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

//READ THIS CODE TO UNDERSTAND WHAT IT DOES CAUSE YOU HAVE TO SAVE ON TXT EVERY NEW PROCESS
void handle_command(char *command) {
    pid_t pid = fork(); // Create a new process
    if (pid == -1) {
        // Fork failed
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        execl("/bin/sh", "sh", "-c", command, NULL);
        // If execl returns, it means it failed
        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0); // Wait for child process to finish
        printf("Child process finished with status: %d\n", status);
    }
}

//SERVER MUST ALWAYS STAY OPEN UNTIL I CLOSE IT AND RESEIVE COMMANDS
int main(int argc, char* argv[]){
    mkfifo(FIFO_PATH, 0666);
    int fd = open(FIFO_PATH, O_RDONLY);
    char *arr[4];
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    for (int i = 0; i < 4; i++) {
        arr[i] = malloc(MAX_COMMAND_LEN + 1);
        if (arr[i] == NULL) {
            perror("Error allocating memory");
            return 2;
        }
        if (read(fd, arr[i], MAX_COMMAND_LEN) == -1) {
            perror("Error reading from file");
            return 3;
        }
        arr[i][MAX_COMMAND_LEN] = '\0';
        handle_command(arr[i]);
        usleep(100000);
    }

    close(fd);
    unlink(FIFO_PATH);
    return 0;
}