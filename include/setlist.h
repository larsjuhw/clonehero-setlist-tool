#ifndef SETLIST_H
#define SETLIST_H

#include <inttypes.h>

typedef struct
{
    uint8_t length;
    char hash[32];
    char suffix[2];
} SetlistEntry;

typedef struct
{
    char header[4];
    uint32_t count;
    SetlistEntry *entries;
} Setlist;

int setlist_load(Setlist *setlist, const char *path);
int setlist_save(Setlist *setlist, const char *path);
int setlist_getDuplicateCount(Setlist *setlist);
int setlist_removeDuplicates(Setlist *setlist);
void setlist_shuffle(Setlist *setlist);
int setlist_remove(Setlist *setlist, int index);
void setlist_swap(Setlist *setlist, int index1, int index2);


#endif // SETLIST_H