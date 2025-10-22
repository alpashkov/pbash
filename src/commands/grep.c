#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLOR_RED    "\033[31m"
#define COLOR_RESET  "\033[0m"

int grep(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: grep <file> <pattern>\n");
        return 1;
    }

    const char *filename = argv[1];
    const char *pattern = argv[2];
    size_t pat_len = strlen(pattern);

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("grep: fopen");
        return 1;
    }

    char line[4096];

    while (fgets(line, sizeof(line), fp)) {
        char *p = line;

        char *tmp = strstr(line, pattern);
        if (!tmp)
            continue;

        while (1) {
            char *match = strstr(p, pattern);
            if (!match) {
                printf("%s", p);
                break;
            }

            fwrite(p, 1, match - p, stdout);
            printf(COLOR_RED "%.*s" COLOR_RESET, (int)pat_len, match);

            p = match + pat_len;
        }
    }

    fclose(fp);
    return 0;
}
