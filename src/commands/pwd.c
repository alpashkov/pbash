#include <stdio.h>

#define POSIX_MAX_PATH 4096

int pwd(int argc, char** argv)
{
    static char current_path[POSIX_MAX_PATH];
    if (getcwd(current_path, sizeof(current_path)) == NULL) {
        perror("getcwd() error");
        return -1;
    }

    printf("%s\n", current_path);
    return 0;
}