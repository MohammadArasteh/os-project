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
void create_pipe(char *addr);
void create_pipes();
void send_data_to_child(char* data, char* pipe_path);
char* receive_data_from_placer(char* pipe_path, int buffer);

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

    int RESULT_MAX_LENGTH = 2*strlen(placer_data);

    int main_process_pid,
        decoder_pid,
        finder_pid,
        placer_pid;

    main_process_pid = getpid();

    decoder_pid = fork();
    if(decoder_pid) finder_pid = fork();
    if(decoder_pid && finder_pid) placer_pid = fork();

    if(decoder_pid && finder_pid && placer_pid) {

        create_pipes();

        send_data_to_child(decoder_data, main_pipe_decoder_path);
        send_data_to_child(finder_data, main_pipe_finder_path);
        send_data_to_child(placer_data, main_pipe_placer_path);

        char* result = receive_data_from_placer(main_pipe_placer_path, RESULT_MAX_LENGTH);
        printf("result: \n%s\n", result);
    }

    if(decoder_pid == CURRENT_CHILD) { // inside decoder process
        char buffer[12];
        sprintf(buffer, "%lu", strlen(decoder_data) + 1);
        char *args[] = {"./decoder",
                        buffer,
                        main_pipe_decoder_path,
                        decoder_pipe_finder_path, NULL};
        execv(args[0], args);
    }
    if(finder_pid == CURRENT_CHILD) { // inside finder process
        char buffer[12];
        sprintf(buffer, "%lu", strlen(finder_data) + 1);
        char *args[] = {"./finder",
                        buffer,
                        main_pipe_finder_path,
                        decoder_pipe_finder_path,
                        finder_pipe_placer_path, NULL};
        execv(args[0], args);
    }
    if(placer_pid == CURRENT_CHILD) { //inside placer process
        char buffer[12];
        sprintf(buffer, "%lu", strlen(placer_data) + 1);
        char *args[] = {"./placer",
                        buffer,
                        main_pipe_placer_path,
                        finder_pipe_placer_path, NULL};
        execv(args[0], args);
    }

    return 0;
}

struct stat st = {0};
void create_pipe(char *addr) {
    if(stat("./pipes", &st) == -1)
        mkdir("./pipes", 0777);
    if(mkfifo(addr, 0777) == -1) {
        if(errno != EEXIST) {
            printf("creating fifo: unexpected error\n");
            exit(1);
        }
    }
}
void create_pipes() {
    create_pipe(main_pipe_decoder_path);
    create_pipe(main_pipe_finder_path);
    create_pipe(main_pipe_placer_path);
    create_pipe(decoder_pipe_finder_path);
    create_pipe(finder_pipe_placer_path);
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
    while(str[*cursor] != '#' && str[*cursor] != 0) {
        result[*cursor - temp] = str[*cursor];
        *cursor += 1;
    }
    result[*cursor - temp] = 0;
    *cursor += 1;
    while(str[*cursor] == '#' || str[*cursor] == 10) *cursor += 1;
}

char** input() {
    char* input = read_input();
    int input_len = strlen(input);
    int cursor = 0;

    char *decoder_data = malloc(input_len);
    read_section(input, decoder_data, &cursor);
    printf("- decoder_data_string: \n%s\n-----------------\n", decoder_data);

    char *finder_data = malloc(input_len);
    read_section(input, finder_data, &cursor);
    printf("- finder_data_string: \n%s\n-----------------\n", finder_data);

    char *placer_data = malloc(input_len);
    read_section(input, placer_data, &cursor);
    printf("- placer_data_string: \n%s\n-----------------\n\n\n", placer_data);

    char** result = malloc(3 * input_len);
    result[0] = decoder_data;
    result[1] = finder_data;
    result[2] = placer_data;
    return result;
}

void send_data_to_child(char* data, char* pipe_path) {
    int fd = open(pipe_path, O_WRONLY);
    int buffer = strlen(data) + 1;
    if(write(fd, data, buffer) == -1) {
        printf("writing pipe(%s): unexpected error\n", pipe_path);
        exit(1);
    }
    close(fd);
}
char* receive_data_from_placer(char* pipe_path, int buffer) {
    char data[buffer];
    int fd = open(pipe_path, O_RDONLY);
    if(read(fd, data, buffer) == -1) {
        printf("reading pipe(placer): unexpected error\n");
        exit(1);
    }
    close(fd);
    char* result = malloc(buffer);
    memcpy(result, data, buffer);
    return result;
}
