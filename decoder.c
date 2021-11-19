#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[]) {

    char *pipe_path = argv[1];
    int text_len = atoi(argv[2]);

    int fd = open(pipe_path, O_RDONLY);
    if(fd == -1) {
        printf("opening pipe: unexpected error\n");
        return 2;
    }

    char plain_text[text_len];
    if((read(fd, &plain_text, text_len + 1)) == -1) {
        printf("reading pipe: unexpected error\n");
        return 2;
    }
    close(fd);

    return 0;
}