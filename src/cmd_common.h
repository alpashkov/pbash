#ifndef _CMD_COMMON_H_
#define _CMD_COMMON_H_

#include <stdint.h>
/*
Safety macros to prevent execution of cmd module functions (void).
*/
#define CMD_COUNT_CHECK_VOID()  if (CMD_COUNT == 0) {return;}
/*
Safety macros to prevent execution of cmd module functions (null).
*/
#define CMD_COUNT_CHECK_RET()   if (CMD_COUNT == 0) {return 0;}
/*
Defines maximum amount of arguments that can be passed to command.
--- TODO:
    add configurable option to change this
*/
#ifndef CMD_ARGC_PEAK
#define CMD_ARGC_PEAK 64
#endif
/*
Commands' function prototype, expected from any added command.
*/
typedef int (*cmd_fp_t)(int, char**);
/*
Commands' struct, holding command name, function pointer
and argc/argv for possible manipulation after command execution.
*/
typedef struct
{
    char* name;
    cmd_fp_t func;
    int argc;
    char** argv;
} Command_t;
/*
Init function for cmd module to call inside main func.
Registers commands and lists them.
--- see cmd_glue_init()
*/
int         cmd_init();
/*
Deinit function for cmd module to call inside main func.
Unregisters commands and frees memory.
*/
void        cmd_deinit();
/*
Registers command with name and function pointer.
*/
Command_t*  cmd_register(const char* name, cmd_fp_t func);
/*
Unregisters command and frees memory.
*/
void        cmd_unregister(Command_t* cmd);
/*
Main input processing function. Gets called when ENTER is pressed,
tries to distinguish which command is called and
calls appropriate function.
*/
void        cmd_resolve_input(char* input);
/*
Searches through commands' names and prints list of available
commands based on provided input;
*/
void        cmd_try_hint(char* input);
/*
Executes command with provided name and arguments.
Returns the result of execution.
*/
int         cmd_execute(Command_t* cmd);


#endif//_CMD_COMMON_H_