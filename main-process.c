#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int create_pipe(char *addr);

static char *main_pipe_decoder_path = "./pipes/main_pipe_decoder";
static char *main_pipe_finder_path = "./pipes/main_pipe_finder";
static char *main_pipe_placer_path = "./pipes/main_pipe_placer";
static char *decoder_pipe_finder_path = "./pipes/decoder_pipe_finder";
static char *finder_pipe_placer_path = "./pipes/finder_pipe_placer";

int main() {

    int main_process_pid, 
        decoder_pid, 
        finder_pid, 
        placer_pid;

    char *decoder_data = "hello from main (decoder)";
    char *finder_data = "hello from main (finder)";
    char *placer_data = "hello from main (placer)";

    main_process_pid = getpid();
    
    decoder_pid = fork();
    if(decoder_pid) finder_pid = fork();
    if(decoder_pid && finder_pid) placer_pid = fork();

    if(decoder_pid && finder_pid && placer_pid) {
        if(!create_pipe(main_pipe_decoder_path)) return 1;
        if(!create_pipe(main_pipe_finder_path)) return 1;
        if(!create_pipe(main_pipe_placer_path)) return 1;


        int decoder_fd = open(main_pipe_decoder_path, O_WRONLY);
        if(write(decoder_fd, decoder_data, strlen(decoder_data) + 1) == -1) {
            printf("writing pipe: unexpected error\n");
            return 2;
        }
        close(decoder_fd);
    }

        if(!decoder_pid) {
            char len[5];
            sprintf(len, "%lu", strlen(decoder_data) + 1);
            char *args[] = {"./decoder", main_pipe_decoder_path, len, NULL};
            execv(args[0], args);
        }
        if(!finder_pid) {

        }
        if(!placer_pid) {

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