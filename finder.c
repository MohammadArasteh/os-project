#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

struct pair {
    int start;
    int end;
};

int main(int argc, char *argv[]) {
    
    // gathering args
    char *pipe_path = argv[1];
    int buffer = atoi(argv[2]);

    // opening fifo file (aka named-pipe)
    int fd = open(pipe_path, O_RDONLY);
    if(fd == -1) {
        printf("opening pipe(finder): unexpected error\n");
        return 2;
    }

    // getting input from parent process
    char pairs_string[buffer];
    if((read(fd, &pairs_string, buffer + 1)) == -1) {
        printf("reading pipe(decoder): unexpected error\n");
        return 2;
    }
    close(fd);
    printf("pairs: %s\n", pairs_string);
}