#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    const char *pipeName = "jobPipe";
    int pipefd;

    // Create the named pipe
    if (mkfifo(pipeName, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    printf("Named pipe '%s' created successfully.\n", pipeName);

    // Open the named pipe for reading and writing
    pipefd = open(pipeName, O_RDWR);
    if (pipefd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("Named pipe '%s' opened successfully.\n", pipeName);

    // Close the named pipe
    close(pipefd);

    // Remove the named pipe from the file system
    if (unlink(pipeName) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    printf("Named pipe '%s' removed successfully.\n", pipeName);

    return 0;
}