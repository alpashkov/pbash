#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int cd(int argc, char** argv)
{
    const char* path;

    if (argc != 2) {
        fprintf(stderr, "Usage:cd [path]\n");
        return 1;
    }

    path = argv[1];

    if (chdir(path) != 0) {
        fprintf(stderr, "cd: %s\n", strerror(errno));
        return 1;
    }

    return 0;
}