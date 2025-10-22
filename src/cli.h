#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
/*
Default POSIX max path length
*/
#define PATH_MAX 4096
/*
Prompt prefix length.
--- TODO:
    This should not be hardcoded.
*/
#define CLI_PROMPT_PREFIX_LEN  8
/*
Prefix fmt string to print in prompt. Gets current path as %s.
*/
#define CLI_PROMPT_FMT  "~pbash:%s>"
/*
Function to be called in main func. Inits CLI interface.
*/
int     cli_init();
/*
Deinits CLI.
*/
void    cli_deinit();
/*
Adjusted printf variant for raw-terminal compatibility.
*/
void    cli_printf(const char* fmt, ...);
/*
Prints prompt containing current directory.
*/
void    cli_print_prompt();
/*
Main input processing function. Defines behaviour of CLI.
--- TODO:
    Add processing to all questionable keys;
*/
void    cli_process_input();
/*
Enables raw mode for CLI.
*/
void    cli_disable_raw_mode();
/*
Disables raw mode for CLI.
*/
int     cli_enable_raw_mode();
/*
Returns current working directory.
*/
char* get_current_path();
/*
Enum of keys processed by cli_process_input().
--- TODO:
    Add more keys;
*/
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