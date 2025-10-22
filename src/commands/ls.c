#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

int ls(int argc, char** argv) {
    const char* path = ".";

    if (argc > 2) {
        printf("Usage: %s [path]\n", argv[0]);
        return 1;
    }

    if (argc == 2)
        path = argv[1];

    DIR* dir = opendir(path);
    if (!dir) {
        printf("Failed to open directory '%s': %s\n", path, strerror(errno));
        return 1;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
           continue;

        printf("%s\n", entry->d_name);        
    }

    closedir(dir);
    return 0;
}