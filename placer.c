#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int get_words_count_from_finder(int fd);
void get_data_from_finder(int words_count, int fd, char result[][32]);
char* shift(char str[], int startIndex ,int k);
void fill_template(char template[], int template_len, char words[][32], int words_len);

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
    char template[2*buffer];
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

    fill_template(template, buffer, words, words_count);

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
void fill_template(char template[], int template_len, char words[][32], int words_len) {
    int current_word = 0, index = 0;
    while(current_word != words_len && template[index] != 0) {
        if(template[index] == '$') {
            memmove(&template[index], &template[index + 1], strlen(template) - index);
            int len = strlen(words[current_word]);
            char* shiftedTempalte = shift(template, index, strlen(words[current_word]));
            for(int i = 0; i < len; i++) 
                memset(shiftedTempalte + index + i, words[current_word][i], 1);
            memcpy(template, shiftedTempalte, strlen(shiftedTempalte));
            template[strlen(shiftedTempalte)] = 0;
            current_word++;
        }
        index++;
    }
}
char* shift(char str[], int startIndex ,int k) {
    char *temp = malloc(strlen(str) + k);
    memcpy(temp, str, strlen(str) + k);
    for(int i = startIndex + k; i < strlen(str) + k; i++)
        temp[i] = str[i - k];
    temp[strlen(str)+k] = 0;
    return temp;
}