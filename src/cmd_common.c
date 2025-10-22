#include <string.h>

#include "cmd_common.h"
#include "cli.h"

#include "cmd_glue_gen.h"


Command_t* cmd_register(const char* name, cmd_fp_t func)
{
    Command_t* cmd = (Command_t*)malloc(sizeof(Command_t));
    cmd->name = strdup(name);
    cmd->func = func;
    cmd->argc = 0;
    cmd->argv = NULL;
    return cmd;
}

void cmd_unregister(Command_t* cmd)
{
    free(cmd->name);
    free(cmd);
}

int cmd_init()
{
    CMD_COUNT_CHECK_RET();
    cmd_glue_init();
    cmd_glue_list_commands();
    return 0;
}

void cmd_deinit()
{
    cmd_glue_deinit();
}

void cmd_resolve_input(char* input)
{
    CMD_COUNT_CHECK_VOID();
    const char* delim = " ";
    char* saveptr;
    char* token = strtok_r(input, delim, &saveptr);
    size_t cmd_idx;

    if ((cmd_idx = cmd_glue_get_idx_by_name(token)) == -1) {
        cli_printf("Unknown command: %s\n", token);
        return;
    }

    Command_t* cmd = cmd_glue_get_cmd_by_idx(cmd_idx);
    int argc = 0;
    char* argv[CMD_ARGC_PEAK];
    argv[argc++] = token;
    while ((token = strtok_r(NULL, delim, &saveptr)))
    {
        argv[argc++] = token;
    }
    argv[argc] = NULL;
    cmd->argc = argc;
    cmd->argv = argv;
    cli_disable_raw_mode();
    cmd_execute(cmd);
    cli_enable_raw_mode();
}

inline
int cmd_execute(Command_t* cmd)
{
    CMD_COUNT_CHECK_RET();
    return cmd->func(cmd->argc, cmd->argv);
}

void cmd_try_hint(char* input)
{
    CMD_COUNT_CHECK_VOID();
    if (!strlen(input)) {
      cli_printf("\n");  
      return;
    }

    Command_t** collection = cmd_glue_get_collection();

    size_t input_len = strlen(input);
    u_char first_hit = 1;
    for (size_t i = 0; i < CMD_COUNT; i++)
    {
        if (strncmp(collection[i]->name, input, input_len) == 0) {
            if (first_hit) {
                cli_printf("\n");
                first_hit = 0;
            }
            cli_printf("%s ", collection[i]->name);
        }
    }
    cli_printf("\n");
}