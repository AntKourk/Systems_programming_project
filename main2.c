#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

int main(int argcc, char* argv[]){
    int arr[5];
    int fd = open("sum",O_RDONLY);
    if (fd==-1){
        return 1;
    }
    int i;
    for(i=0; i<5; i++){
        if(read(fd, &arr[i], sizeof(int))==-1){
            return 2;
        }
        printf("Received %d\n", arr[i]);
    }
    close(fd);

    printf("homework\n");

    int fd2 = open("sum",O_WRONLY);
    for(i=0; i<5; i++){
        if(write(fd2, &arr[i], sizeof(int))==-1){
            return 2;
        }
        printf("Wrote %d\n", arr[i]);
    }
    close(fd);

    return 0;
}