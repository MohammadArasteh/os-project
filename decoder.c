#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

void send_data_to_finder(char* data, char* pipe_path);
char shiftChar(char c);
int isAlphabet(char c);
char* standardize(char str[], int len);

int main(int argc, char *argv[]) {

    // gathering args
    int buffer = atoi(argv[1]);
    char *pipe_path = argv[2];
    char *finder_pipe_path = argv[3];

    // opening fifo file (aka named-pipe)
    int fd = open(pipe_path, O_RDONLY);
    if(fd == -1) {
        printf("opening pipe(decoder): unexpected error\n");
        exit(1);
    }

    // getting input from parent process
    char plain_text[buffer];
    if((read(fd, &plain_text, buffer + 1)) == -1) {
        printf("reading pipe(decoder): unexpected error\n");
        exit(1);
    }
    close(fd);

    char* std_text = standardize(plain_text, buffer);
    buffer = strlen(std_text);

    // decoding
    char decoded_text[buffer];
    for(int i = 0; i < buffer; i++)
        decoded_text[i] = shiftChar(std_text[i]);
    decoded_text[buffer-1] = 0;

    // writing decoded text to a text file
    FILE *output = fopen("decoder_result.txt", "w");
    fputs(decoded_text, output);
    fclose(output);

    send_data_to_finder(decoded_text, finder_pipe_path);

    return 0;
}

void send_data_to_finder(char* data, char* pipe_path) {
    int buffer = strlen(data);
    int fd = open(pipe_path, O_WRONLY);
    if(fd == -1) {
        printf("opening pipe(decoder~finder): unexpected error\n");
        exit(1);
    }
    if(write(fd, &buffer, sizeof(int)) == -1) {
        printf("writing pipe(decoder~finder): unexpected error\n");
        exit(1);
    }
    if(write(fd, data, buffer + 1) == -1) {
        printf("writing pipe(decoder~finder): unexpected error\n");
        exit(1);
    }
    close(fd);
}

char shiftChar(char c) {
    if(!isAlphabet(c)) return c;
    if(c == 'A') return 'X';
    if(c == 'B') return 'Y';
    if(c == 'C') return 'Z';
    if(c == 'a') return 'x';
    if(c == 'b') return 'y';
    if(c == 'c') return 'z';
    return c - 3;
}

int isAlphabet(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

char* standardize(char str[], int len) {
    int index = 0;
    char* oneline_text = malloc(len);
    for(int i = 0; i < len; i++)
        if(str[i] != 10 && str[i] != 32)
            oneline_text[index++] = str[i];
    oneline_text[index] = 0;
    return oneline_text;
}
