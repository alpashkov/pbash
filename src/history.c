#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "history.h"

History_t* history_init()
{
    History_t* new_history = (History_t*)malloc(sizeof(History_t));
    if (!new_history) {
        perror("history_init: malloc failed");
        return NULL;
    }

    new_history->entries = (History_entry_t**)malloc(sizeof(char*) * HISTORY_SIZE);
    if (!new_history->entries) {
        perror("history_init: malloc failed");
        free(new_history);
        return NULL;
    }

    new_history->count = 0;
    new_history->last_viewed = 0;

    return new_history;
}

inline
void history_deinit(History_t* history)
{
    if (!history) return;

    for (size_t i = 0; i < HISTORY_SIZE; i++){
        if (history->entries[i])
            free(history->entries[i]);
    }
    free(history->entries);
    free(history);
}

static
History_entry_t* history_get_oldest(History_t* history)
{
    History_entry_t* oldest = history->entries[0];

    for (size_t i = 1; i < HISTORY_SIZE; i++)
    {
        if (history->entries[i]->tstamp < oldest->tstamp)
            oldest = history->entries[i];
    }

    return oldest;
}

void history_add(History_t* history, const char* command)
{
    if (!history || !command) return;

    History_entry_t* new_data;
    if (history->count == HISTORY_SIZE) {
        new_data = history_get_oldest(history);
        free(new_data->command);
    }
    else
        new_data = history->entries[history->count-1];

    new_data->command = strdup(command);
    new_data->tstamp = time(NULL);
}