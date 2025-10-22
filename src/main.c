#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>

#include "cli.h"
#include "cmd_common.h"

static volatile sig_atomic_t stop_flag = 0;

static 
void signal_handler(int sig)
{
    stop_flag = 1;
    cli_printf("Signal received\n");
}

int main()
{
    int rc = 0;

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction(SIGINT)");
        rc = -1;
        return rc;
    }
    
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction(SIGTERM)");
        rc = -1;
        return rc;
    }

    if ((rc = cmd_init()) != 0)        goto cmd_cleanup;
    if ((rc = cli_init()) != 0)        goto cli_cleanup;

    while (!stop_flag) {
        cli_process_input();
    }

cli_cleanup:
    cli_deinit();
cmd_cleanup:
    cmd_deinit();

    return rc;
}