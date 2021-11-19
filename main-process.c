#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {

    int main_process_pid, 
        decoder_pid, 
        finder_pid, 
        placer_pid;

    main_process_pid = getpid();
    
    decoder_pid = fork();
    if(decoder_pid) finder_pid = fork();
    if(decoder_pid && finder_pid) placer_pid = fork();

    return 0;
}