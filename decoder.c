#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

char shiftChar(char c);

int main(int argc, char *argv[]) {

    // gathering args
    char *pipe_path = argv[1];
    int buffer = atoi(argv[2]);

    // opening fifo file (aka named-pipe)
    int fd = open(pipe_path, O_RDONLY);
    if(fd == -1) {
        printf("opening pipe(decoder): unexpected error\n");
        return 2;
    }

    // getting input from parent process
    char plain_text[buffer];
    if((read(fd, &plain_text, buffer + 1)) == -1) {
        printf("reading pipe(decoder): unexpected error\n");
        return 2;
    }
    close(fd);

    // decoding
    char decoded_text[buffer];
    for(int i = 0; i < buffer; i++)
        decoded_text[i] = shiftChar(plain_text[i]);
    decoded_text[buffer-1] = '\0';

    // writing decoded text to a text file
    FILE *output = fopen("decoder_result.txt", "w");
    fputs(decoded_text, output);
    fclose(output);

    return 0;
}

char shiftChar(char c) {
    if(c == 'A') return 'X';
    if(c == 'B') return 'Y';
    if(c == 'C') return 'Z';
    if(c == 'a') return 'x';
    if(c == 'b') return 'y';
    if(c == 'c') return 'z';
    return c - 3;
}