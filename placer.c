#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    // gathering args
    int buffer = atoi(argv[1]);
    char *pipe_path = argv[2];
    char *finder_pipe_path = argv[3];
}