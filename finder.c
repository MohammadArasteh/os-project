#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

static int MAX_LETTER_LENGTH = 32;

struct pair* extract_pears(char* str, int* pairs_count);
struct pair extract_pear(char* str, int *cursor);
int read_digit(char* str, int* cursor);
int count_pairs(char* str);
char* get_data_from_decoder(char* pipe_path);
void send_data_to_placer(char words[][MAX_LETTER_LENGTH], char* pipe_path, int words_count);

struct pair {
    int start;
    int size;
};


int main(int argc, char *argv[]) {
    // gathering args
    int buffer = atoi(argv[1]);
    char *pipe_path = argv[2];
    char *decoder_pipe_path = argv[3];
    char *placer_pipe_path = argv[4];

    // opening fifo file (aka named-pipe)
    int fd = open(pipe_path, O_RDONLY);
    if(fd == -1) {
        printf("opening pipe(finder): unexpected error\n");
        return 2;
    }

    // getting input from parent process
    char pairs_string[buffer];
    if((read(fd, &pairs_string, buffer + 1)) == -1) {
        printf("reading pipe(finder): unexpected error\n");
        return 2;
    }
    close(fd);
    int pairs_count;
    struct pair* pairs = extract_pears(pairs_string, &pairs_count);
    char* data = get_data_from_decoder(decoder_pipe_path);

    char words[pairs_count][MAX_LETTER_LENGTH];
    for(int i = 0; i < pairs_count; i++) {
        int start_index = pairs[i].start;
        int letter_size = pairs[i].size;
        memcpy(words[i], &data[start_index - 1], letter_size);
        words[i][letter_size] = 0;
    }

    FILE *output = fopen("finder_result.txt", "w");
    for(int i = 0; i < pairs_count; i++) {
        char* template = malloc(MAX_LETTER_LENGTH);
        memcpy(template, words[i], strlen(words[i]));
        template[strlen(template)] = '\n';
        fputs(template, output);
    }
    fclose(output);

    send_data_to_placer(words, placer_pipe_path, pairs_count);

    return 0;
}

struct pair* extract_pears(char* str, int* pairs_count) {
    *pairs_count = count_pairs(str);
    int cursor = 0;
    struct pair* pairs = malloc(*pairs_count * sizeof(struct pair));
    int index = 0;
    while(cursor < strlen(str) - 1)
        pairs[index++] = extract_pear(str, &cursor);

    return pairs;
}
struct pair extract_pear(char* str, int* cursor) {
    int start = read_digit(str, cursor);
    int size = read_digit(str, cursor);
    struct pair p = {start = start, size = size};
    return p;
}
int read_digit(char* str, int* cursor) {
    int temp = *cursor, digit = 0;
    while(str[*cursor] != ' ' && str[*cursor] != '$') {
        int d = str[*cursor] - 48;
        digit = digit*10 + d;
        *cursor += 1;
    }
    *cursor += 1;
    return digit;
}

int count_pairs(char* str) {
    int index = 0;
    int counter = 0;
    while(str[index] != 0) {
        if(str[index] == '$') counter++;
        index++;
    }
    return counter;
}

char* get_data_from_decoder(char* pipe_path) {
    int fd = open(pipe_path, O_RDONLY);
    if(fd == -1) {
        printf("opening pipe(finder~decoder): unexpected error\n");
        exit(1);
    }
    int buffer;
    if((read(fd, &buffer, sizeof(int))) == -1) {
        printf("reading pipe(finder~decoder): unexpected error\n");
        exit(1);
    }

    char data[buffer];
    if((read(fd, &data, buffer + 1)) == -1) {
        printf("reading pipe(finder~decoder): unexpected error\n");
        exit(1);
    }
    close(fd);
    char* result = malloc(buffer + 1);
    memcpy(result, data, buffer + 1);
    return result;
}

void send_data_to_placer(char words[][MAX_LETTER_LENGTH], char* pipe_path, int words_count) {
    int buffer = words_count * MAX_LETTER_LENGTH;
    int fd = open(pipe_path, O_WRONLY);
    if(fd == -1) {
        printf("opening pipe(finder~placer): unexpected error\n");
        exit(1);
    }
    if(write(fd, &words_count, sizeof(int)) == -1) {
        printf("writing pipe(finder~placer): unexpected error\n");
        exit(1);
    }
    if(write(fd, words, buffer + 1) == -1) {
        printf("writing pipe(finder~placer): unexpected error\n");
        exit(1);
    }
    close(fd);
}
