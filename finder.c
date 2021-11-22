#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

struct pair* extract_pears(char* str, int* pairs_count);
struct pair extract_pear(char* str, int *cursor);
int read_digit(char* str, int* cursor);
int count_pairs(char* str);
char* get_data_from_decoder(char* pipe_path);

struct pair {
    int start;
    int size;
};

static int MAX_LETTER_LENGTH = 32;

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
        printf("reading pipe(decoder): unexpected error\n");
        return 2;
    }
    close(fd);

    int pairs_count;
    struct pair* pairs = extract_pears(pairs_string, &pairs_count);

    char* data = get_data_from_decoder(decoder_pipe_path);

    char letters[pairs_count][MAX_LETTER_LENGTH];
    for(int i = 0; i < pairs_count; i++) {
        int start_index = pairs[i].start;
        int letter_size = pairs[i].size;
        memcpy(letters[i], &data[start_index - 1], letter_size);
        letters[i][letter_size] = 0;
    }

    return 0;
}

struct pair* extract_pears(char* str, int* pairs_count) {
    *pairs_count = count_pairs(str);
    int cursor = 0;
    struct pair* pairs = malloc(*pairs_count * sizeof(struct pair));

    int index = 0;
    while(str[cursor] != 0) 
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
    int temp = *cursor;
    char* temp_string = malloc(8);
    while(str[*cursor] != ' ' && str[*cursor] != '$') {
        temp_string[*cursor - temp] = str[*cursor];
        *cursor += 1;
    }
    temp_string[*cursor] = 0;
    *cursor += 1;
    int digit = atoi(temp_string);
    free(temp_string);
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