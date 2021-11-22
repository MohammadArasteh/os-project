#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    // gathering args
    int buffer = atoi(argv[1]);
    char *pipe_path = argv[2];
    char *finder_pipe_path = argv[3];

    // opening fifo file (aka named-pipe)
    int fd = open(pipe_path, O_RDONLY);
    if(fd == -1) {
        printf("opening pipe(placer): unexpected error\n");
        return 2;
    }

    // getting input from parent process
    char template[buffer];
    if((read(fd, &template, buffer + 1)) == -1) {
        printf("reading pipe(placer): unexpected error\n");
        return 2;
    }
    close(fd);

}