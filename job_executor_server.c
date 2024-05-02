#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FIFO_PATH "jobPipe"
#define MAX_COMMAND_LEN 256

void handle_command(char *command) {
    // Execute the received command
    system(command);
}

int main() {
    int fd;
    char command[MAX_COMMAND_LEN];

    // Open named pipe for reading
    mkfifo(FIFO_PATH, 0666);
    fd = open(FIFO_PATH, O_RDONLY | O_NONBLOCK);

    while (1) {
        // Read command from pipe
        if (read(fd, command, MAX_COMMAND_LEN) > 0) {
            // Handle the command
            handle_command(command);
        }

        // Add more logic as needed

        // Sleep for a short while to avoid CPU hogging
        usleep(100000);
    }

    // Close and remove the pipe
    close(fd);
    unlink(FIFO_PATH);

    return 0;
}