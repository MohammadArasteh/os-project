#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int get_words_count_from_finder(int fd);
void get_data_from_finder(int words_count, int fd, char result[][32]);

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

    fd = open(finder_pipe_path, O_RDONLY);
    if(fd == -1) {
        printf("opening pipe(finder~placer): unexpected error\n");
        exit(1);
    }
    int words_count = get_words_count_from_finder(fd);
    char words[words_count][32];
    get_data_from_finder(words_count, fd, words);
    close(fd);

    return 0;
}

int get_words_count_from_finder(int fd) {
    int words_count;
    if(read(fd, &words_count, sizeof(int)) == -1) {
        printf("writing pipe(finder~placer): unexpected error\n");
        exit(1);
    }
    return words_count;
}
void get_data_from_finder(int words_count, int fd, char result[][32]) {
    if((read(fd, result, words_count * 32)) == -1) {
        printf("reading pipe(placer~decoder): unexpected error\n");
        exit(1);
    }
}