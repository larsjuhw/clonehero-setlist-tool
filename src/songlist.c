#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>

#include "songlist.h"

int songlist_load(struct sc_map_sv *songlist_map, const char *path)
{
    FILE *ptr = fopen(path, "rb");
    if (ptr == NULL)
    {
        return -1;
    }

    uint32_t count;

    if (fread(&count, sizeof(count), 1, ptr) != 1)
    {
        return -2;
    }

    if (!sc_map_init_sv(songlist_map, count, 0))
    {
        return -3;
    }

    songlist = malloc(sizeof(Song) * count);
    for (int i = 0; i < count; i++)
    {
        uint32_t titleLength, artistLength, hashLength;
        // Read lengths
        fread(&titleLength, sizeof(titleLength), 1, ptr);
        fread(&artistLength, sizeof(artistLength), 1, ptr);
        fread(&hashLength, sizeof(hashLength), 1, ptr);

        // Store lengths
        songlist[i].titleLength = titleLength;
        songlist[i].artistLength = artistLength;

        // Allocate memory for strings
        songlist[i].title = malloc(titleLength + 1);
        songlist[i].artist = malloc(artistLength + 1);
        songlist[i].hash = malloc(hashLength + 1);

        // Read strings
        fread(songlist[i].title, titleLength, 1, ptr);
        fread(songlist[i].artist, artistLength, 1, ptr);
        fread(songlist[i].hash, hashLength, 1, ptr);

        // Add null terminators
        songlist[i].title[titleLength] = '\0';
        songlist[i].artist[artistLength] = '\0';
        songlist[i].hash[hashLength] = '\0';

        // Add to map
        sc_map_put_sv(songlist_map, songlist[i].hash, &songlist[i]);
    }
}

Song *lookup_song(struct sc_map_sv *songlist_map, const char *hash)
{
    if (songlist_map == NULL)
    {
        return NULL;
    }

    Song *lookup = sc_map_get_sv(songlist_map, hash);
    if (!sc_map_found(songlist_map))
    {
        return NULL;
    }
    return lookup;
}
