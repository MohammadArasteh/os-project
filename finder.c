#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

struct pair* extract_pears(char* str);
struct pair extract_pear(char* str, int *cursor);
int read_digit(char* str, int* cursor);
int count_pairs(char* str);

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
    struct pair* pairs = extract_pears(pairs_string);
}

struct pair* extract_pears(char* str) {
    int count = count_pairs(str);
    int cursor = 0;
    struct pair* pairs = malloc(count * sizeof(struct pair));
    
    int index = 0;
    while(str[cursor] != 0) 
        pairs[index++] = extract_pear(str, &cursor);
    
    return pairs;
}
struct pair extract_pear(char* str, int* cursor) {
    int start = read_digit(str, cursor);
    int end = read_digit(str, cursor);
    struct pair p = {start = start, end = end};
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