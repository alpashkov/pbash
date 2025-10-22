#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>

#include "cli.h"
#include "cmd_common.h"

static volatile sig_atomic_t stop_flag = 0;

static
void* signal_handler_thread(void* arg)
{
    sigset_t* set = (sigset_t*)arg;
    int sig;

    while (1) {
        int res = sigwait(set, &sig);
        if (res == 0) {
            switch (sig) {
                case SIGTERM:
                case SIGINT:
                    stop_flag = 1;
                    return NULL;
                default:
                    break;
            }
        } else {
            perror("sigwait");
        }
    }
    return NULL;
}

static
int sig_init(pthread_t* tid)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGINT);

    if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
        perror("pthread_sigmask");
        return -1;
    }

    if (pthread_create(tid, NULL, signal_handler_thread, &set) != 0) {
        perror("pthread_create");
        return -1;
    }
}


int main()
{
    pthread_t tid;
    int rc;
    
    // if (rc = sig_init(&tid) != 0)    goto sig_cleanup;
    cmd_init();
    if (rc = cli_init() != 0)        goto cli_cleanup;

    while (!stop_flag) {
        cli_process_input();

    }

cli_cleanup:
    cli_deinit();
sig_cleanup:
    pthread_join(tid, NULL);

    return rc;
}