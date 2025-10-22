#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

// Default POSIX max path length
#define PATH_MAX 4096

#define CLI_PROMPT_PREFIX_LEN  8
#define CLI_PROMPT_FMT  "~pbash:%s>"

int     cli_init();
void    cli_deinit();
void    cli_printf(const char* fmt, ...);
void    cli_print_prompt();
void    cli_process_input();

void    cli_disable_raw_mode();
int     cli_enable_raw_mode();

char* get_current_path();

enum Key_t
{
    ENTER_N = '\n',
    ENTER_R = '\r',
    BACKSPACE = '\b',
    BACKSPACE2 = 127,
    TAB = '\t',
    CTRL_C = 3,
    ESC_SEQ = 0x1b
};