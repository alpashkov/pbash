/*
    WiP, this file is not being compiled.
    Bad solution, needs to be redone.
*/
#ifndef _HISTORY_H_
#define _HISTORY_H_

#include <stdint.h>
#include <time.h>

#ifndef HISTORY_SIZE
#define HISTORY_SIZE 25
#endif

typedef struct
{
    char* command;
    time_t tstamp;
} History_entry_t;


typedef struct
{
    History_entry_t** entries;
    int count;
    int last_viewed;
} History_t;

enum History_scroll_t
{
    PREV,
    NEXT
};


History_t*      history_init();
inline  void    history_deinit(History_t* history);
void            history_add(History_t* history, const char* command);
const char*     history_get(History_t* history, History_scroll_t scroll);

#endif//_HISTORY_H_