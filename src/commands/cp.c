#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>

int cp(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: cp <source> <destination>\n");
        return 1;
    }

    const char *src = argv[1];
    const char *dst = argv[2];

    struct stat st_src;
    if (stat(src, &st_src) != 0) {
        fprintf(stderr, "cp: cannot stat '%s': %s\n", src, strerror(errno));
        return 1;
    }

    if (S_ISDIR(st_src.st_mode)) {
        fprintf(stderr, "cp: '%s' is a directory — not copied\n", src);
        return 1;
    }

    char dst_path[PATH_MAX];

    struct stat st_dst;
    if (stat(dst, &st_dst) == 0 && S_ISDIR(st_dst.st_mode)) {
        char src_copy[PATH_MAX];
        if (strlen(src) >= sizeof(src_copy)) {
            fprintf(stderr, "cp: source path too long\n");
            return 1;
        }
        strncpy(src_copy, src, sizeof(src_copy)-1);
        src_copy[sizeof(src_copy)-1] = '\0';

        const char *fname = basename(src_copy);
        int ret = snprintf(dst_path, sizeof(dst_path), "%s/%s", dst, fname);
        if (ret < 0 || ret >= (int)sizeof(dst_path)) {
            fprintf(stderr, "cp: destination path too long\n");
            return 1;
        }
    } else {
        if (strlen(dst) >= sizeof(dst_path)) {
            fprintf(stderr, "cp: destination path too long\n");
            return 1;
        }
        strncpy(dst_path, dst, sizeof(dst_path)-1);
        dst_path[sizeof(dst_path)-1] = '\0';
    }
    
    printf("Copying '%s' -> '%s'\n", src, dst_path);

    int in_fd = open(src, O_RDONLY);
    if (in_fd < 0) {
        fprintf(stderr, "cp: cannot open '%s': %s\n", src, strerror(errno));
        return 1;
    }

    int out_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, st_src.st_mode & 0777);
    if (out_fd < 0) {
        fprintf(stderr, "cp: cannot create '%s': %s\n", dst_path, strerror(errno));
        close(in_fd);
        return 1;
    }

    char buf[8192];
    ssize_t n;
    int ret_code = 0;
    while ((n = read(in_fd, buf, sizeof(buf))) > 0) {
        ssize_t written = 0;
        while (written < n) {
            ssize_t w = write(out_fd, buf + written, n - written);
            if (w < 0) {
                fprintf(stderr, "cp: write error to '%s': %s\n", dst_path, strerror(errno));
                ret_code = 1;
                goto cleanup;
            }
            written += w;
        }
    }

    if (n < 0) {
        fprintf(stderr, "cp: read error from '%s': %s\n", src, strerror(errno));
        ret_code = 1;
    }

cleanup:
    close(in_fd);
    close(out_fd);
    return ret_code;
}
