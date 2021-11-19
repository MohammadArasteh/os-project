#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int create_pipe(char *addr);

static char *main_pipe_decoder_address = "./pipes/main_pipe_decoder";
static char *main_pipe_finder_address = "./pipes/main_pipe_finder";
static char *main_pipe_placer_address = "./pipes/main_pipe_placer";
static char *decoder_pipe_finder_address = "./pipes/decoder_pipe_finder";
static char *finder_pipe_placer_address = "./pipes/finder_pipe_placer";

int main() {

    int main_process_pid, 
        decoder_pid, 
        finder_pid, 
        placer_pid;

    main_process_pid = getpid();
    
    decoder_pid = fork();
    if(decoder_pid) finder_pid = fork();
    if(decoder_pid && finder_pid) placer_pid = fork();

    if(decoder_pid && finder_pid && placer_pid) {
        if(!create_pipe(main_pipe_decoder_address)) return 1;
        if(!create_pipe(main_pipe_finder_address)) return 1;
        if(!create_pipe(main_pipe_placer_address)) return 1;
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