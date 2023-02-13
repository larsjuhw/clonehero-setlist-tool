#ifndef SONGLIST_H
#define SONGLIST_H

#include <inttypes.h>

#include "sc_map.h"

typedef struct
{
    uint8_t titleLength;
    uint8_t artistLength;
    char *title;
    char *artist;
    char *hash;
} Song;

static Song *songlist = NULL;

int songlist_load(struct sc_map_sv *songlist_map, const char *path);
Song *lookup_song(struct sc_map_sv *songlist_map, const char *hash);

#endif // SONGLIST_H