#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <assert.h>
#include <signal.h>
#include <string.h>

#include "cli.h"

static struct termios orig_termios;
static size_t prompt_len = 0;

inline
void cli_disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

int cli_enable_raw_mode()
{
    int ret = 0;

    ret = tcgetattr(STDIN_FILENO, &orig_termios);
    if (ret) return ret;

    ret = atexit(cli_disable_raw_mode);
    if (ret) return ret;

    struct termios raw = orig_termios;

    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_iflag &= ~(IXON | ICRNL);
    raw.c_oflag &= ~(OPOST);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    ret = tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    return ret;
}

static inline
long get_sysconf_arg_max()
{
    static long arg_max = 0;

    if (!arg_max)
    {
        arg_max = sysconf(_SC_ARG_MAX);
        if (arg_max == -1) {
            perror("sysconf(_SC_ARG_MAX) failed, using default ARG_MAX=4096");
            arg_max = 8192;
        }
    }

    return arg_max / 2;
}

static inline
char* get_input_buffer()
{
    static char* buf = NULL;

    if (!buf)
    {
        buf = malloc(get_sysconf_arg_max());
        if (!buf) {
            perror("malloc() on input buffer failed");
            return NULL;
        }
        memset(buf, 0, get_sysconf_arg_max());
    }

    return buf;
}

inline
int cli_init()
{
    cli_enable_raw_mode();
    fflush(stdout);
    return 0;
}

inline
void cli_deinit()
{
    free(get_input_buffer());
}

void cli_printf(const char* fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (len <= 0) return;

    ssize_t ret = -1;
    for (int i = 0; i < len; i++) {
        if (buf[i] == '\n')
            ret = write(STDOUT_FILENO, "\r\n", 2);
        else
            ret = write(STDOUT_FILENO, &buf[i], 1);

        if (ret == -1)
            perror("write() error\r\n");
    }
}

inline
void cli_print_prompt()
{
    static char prompt_buf[PATH_MAX + CLI_PROMPT_PREFIX_LEN];
    memset(prompt_buf, 0, sizeof(prompt_buf));
    sprintf(prompt_buf, CLI_PROMPT_FMT, get_current_path());
    prompt_len = strlen(prompt_buf);
    cli_printf(prompt_buf);
}

void cli_process_input()
{
    cli_print_prompt();
    char* buf = get_input_buffer();
    memset(buf, 0, get_sysconf_arg_max());
    size_t len = 0, cur_pos = 0;
    char c;

    while (1) {
        if (read(STDIN_FILENO, &c, 1) == -1) continue;
        switch (c) {
        case ENTER_N:
        case ENTER_R:
            cli_printf("\n");
            if (len > 0)
                cmd_resolve_input(buf);
            goto loop_break;
        case BACKSPACE:
        case BACKSPACE2:
            if (cur_pos > 0) {
                memmove(&buf[cur_pos - 1], &buf[cur_pos], len - cur_pos);
                len--;
                cur_pos--;
                buf[len] = '\0';

                cli_printf("\b");
                write(STDOUT_FILENO, &buf[cur_pos], len - cur_pos);
                write(STDOUT_FILENO, " ", 1);

                for (size_t i = cur_pos; i < len + 1; ++i)
                    cli_printf("\b");
            }
            break;
        case TAB:
            cmd_try_hint(buf);
            return;
        case CTRL_C:
            signal(SIGINT, NULL);
            return;
        case ESC_SEQ: {
            char seq[2];
            if (read(STDIN_FILENO, &seq, 2) != 2) break;

            if (seq[0] == '[') {
                switch (seq[1]) {
                case 'C': // right arrow
                    if (cur_pos < len) {
                        cur_pos++;
                        cli_printf("\033[C"); // move cursor right
                    }
                    break;
                case 'D': // left arrow
                    if (cur_pos > 0) {
                        cur_pos--;
                        cli_printf("\033[D"); // move cursor left
                    }
                    break;
                }
            }
            break;
        }
        default:
            if (len < get_sysconf_arg_max() - 1) {
                memmove(buf + cur_pos + 1, buf + cur_pos, len - cur_pos);
                buf[cur_pos] = c;
                len++;
                write(STDIN_FILENO, buf + cur_pos, len - cur_pos);
                for (size_t i = cur_pos + 1; i < len; i++)
                    cli_printf("\b");
                cur_pos++;
            }
            break;
        }
    }
loop_break:
    buf[len] = '\0';
}

char* get_current_path()
{
    static char current_path[FILENAME_MAX];
    if (getcwd(current_path, sizeof(current_path)) == NULL)
        perror("getcwd() error");

    return current_path;
}