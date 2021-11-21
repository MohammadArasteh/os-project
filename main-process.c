#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

char** input();
char* read_input();
void read_section(char str[],char result[],int* cursor);
int create_pipe(char *addr);

static char *main_pipe_decoder_path = "./pipes/main_pipe_decoder";
static char *main_pipe_finder_path = "./pipes/main_pipe_finder";
static char *main_pipe_placer_path = "./pipes/main_pipe_placer";
static char *decoder_pipe_finder_path = "./pipes/decoder_pipe_finder";
static char *finder_pipe_placer_path = "./pipes/finder_pipe_placer";

static int CURRENT_CHILD = 0;

int main() {

    char** input_data = input(); // [decoder ~ finder ~ placer]
    char* decoder_data = input_data[0];
    char* finder_data = input_data[1];
    char* placer_data = input_data[2];

    int main_process_pid, 
        decoder_pid, 
        finder_pid, 
        placer_pid;

    main_process_pid = getpid();

    decoder_pid = fork();
    if(decoder_pid) finder_pid = fork();
    if(decoder_pid && finder_pid) placer_pid = fork();

    if(decoder_pid && finder_pid && placer_pid) {

        if(!create_pipe(main_pipe_decoder_path)) return 1;
        if(!create_pipe(main_pipe_finder_path)) return 1;
        if(!create_pipe(main_pipe_placer_path)) return 1;

        int buffer;

        int decoder_fd = open(main_pipe_decoder_path, O_WRONLY);
        buffer = strlen(decoder_data) + 1;
        if(write(decoder_fd, decoder_data, buffer) == -1) {
            printf("writing pipe(decoder): unexpected error\n");
            return 2;
        }
        close(decoder_fd);

        int finder_fd = open(main_pipe_finder_path, O_WRONLY);
        buffer = strlen(finder_data) + 1;
        if(write(finder_fd, finder_data, buffer) == -1) {
            printf("writing pipe(finder): unexpected error\n");
            return 2;
        }
        close(finder_fd);

        int placer_fd = open(main_pipe_placer_path, O_WRONLY);
        buffer = strlen(placer_data) + 1;
        if(write(placer_fd, placer_data, buffer) == -1) {
            printf("writing pipe(placer): unexpected error\n");
            return 2;
        }
        close(placer_fd);
    }

    if(decoder_pid == CURRENT_CHILD) { // inside decoder process
        char buffer[12];
        sprintf(buffer, "%lu", strlen(decoder_data) + 1);
        char *args[] = {"./decoder", main_pipe_decoder_path, buffer, NULL};
        execv(args[0], args);
    }
    if(finder_pid == CURRENT_CHILD) { // inside finder process
        char buffer[12];
        sprintf(buffer, "%lu", strlen(finder_data) + 1);
        char *args[] = {"./finder", main_pipe_finder_path, buffer, NULL};
        execv(args[0], args);
    }
    if(placer_pid == CURRENT_CHILD) { //inside placer process
        char buffer[12];
        sprintf(buffer, "%lu", strlen(placer_data) + 1);
        char *args[] = {"./finder", main_pipe_placer_path, buffer,NULL};
    }

    return 0;
}    

struct stat st = {0};
int create_pipe(char *addr) {
    if(stat("./pipes", &st) == -1)
        mkdir("./pipes", 0777);
    if(mkfifo(addr, 0777) == -1) {
        if(errno != EEXIST) {
            printf("creating fifo: unexpected error\n");
            return 0;
        }
    }
    return 1;
}

char* read_input() {
    FILE *file = fopen("input.txt", "r");
    fseek(file, 0, SEEK_END);
    long f_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *result = malloc(f_size + 1);
    fread(result, 1, f_size, file);
    fclose(file);
    return result;
}

void read_section(char str[],char result[],int* cursor) {
    int temp = *cursor;
    while(str[*cursor] != '#' && str[*cursor] != 10 && str[*cursor] != 0) {
        result[*cursor - temp] = str[*cursor];
        *cursor += 1;
    }
    result[*cursor] = '\0';
    while(str[*cursor] == '#' || str[*cursor] == 10) *cursor += 1;
}

char** input() {
    char* input = read_input();
    int input_len = sizeof(input)/sizeof(char);
    int cursor = 0;

    char *decoder_data = malloc(input_len);
    read_section(input, decoder_data, &cursor);
    // printf("encoder_data: %s\n", decoder_data);

    char *finder_data = malloc(input_len);
    read_section(input, finder_data, &cursor);
    // printf("finder_data: %s\n", finder_data);

    char *placer_data = malloc(input_len);
    read_section(input, placer_data, &cursor);
    // printf("placer_data: %s\n", placer_data);

    char** result = malloc(3 * input_len);
    result[0] = decoder_data;
    result[1] = finder_data;
    result[2] = placer_data;
    return result;
}
