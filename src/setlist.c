#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "setlist.h"

int setlist_load(Setlist *setlist, const char *path)
{
    if (setlist == NULL)
    {
        return -1;
    }

    FILE *ptr = fopen(path, "rb");
    if (ptr == NULL)
    {
        return -1;
    }

    if (fread(&(setlist->header), 4, 1, ptr) != 1)
    {
        return -1;
    }
    if (fread(&(setlist->count), sizeof(setlist->count), 1, ptr) != 1)
    {
        return -1;
    }

    setlist->entries = malloc(sizeof(SetlistEntry) * setlist->count);
    if (setlist->entries == NULL)
    {
        return -1;
    }

    if (fread(setlist->entries, sizeof(SetlistEntry), setlist->count, ptr) != setlist->count)
    {
        return -1;
    }

    fclose(ptr);

    return 0;
}

int setlist_save(Setlist *setlist, const char *path)
{
    FILE *ptr = fopen(path, "wb");
    if (ptr == NULL)
    {
        return -1;
    }

    // Write header and count
    if (fwrite(setlist, sizeof(setlist->header) + sizeof(setlist->count), 1, ptr) != 1)
    {
        return -1;
    }

    // Write entries
    if (fwrite(setlist->entries, sizeof(SetlistEntry), setlist->count, ptr) != setlist->count)
    {
        return -1;
    }

    fclose(ptr);

    return 0;
}

int setlist_getDuplicateCount(Setlist *setlist)
{
    int count = 0;
    for (int i = 0; i < setlist->count; i++)
    {
        for (int j = i + 1; j < setlist->count; j++)
        {
            if (strncmp(setlist->entries[i].hash, setlist->entries[j].hash, setlist->entries[i].length) == 0)
            {
                count++;
                break;
            }
        }
    }
    return count;
}

int setlist_removeDuplicates(Setlist *setlist)
{
    int duplicatesCount = setlist_getDuplicateCount(setlist);
    uint32_t newCount = setlist->count - duplicatesCount;
    SetlistEntry *newEntries = malloc(sizeof(SetlistEntry) * newCount);
    if (newEntries == NULL)
    {
        return -1;
    }

    SetlistEntry *curEntry = newEntries;

    for (int i = 0; i < setlist->count; i++)
    {
        int duplicate = 0;
        for (int j = i + 1; j < setlist->count; j++)
        {
            if (strncmp(setlist->entries[i].hash, setlist->entries[j].hash, setlist->entries[i].length) == 0)
            {
                duplicate = 1;
                break;
            }
        }
        if (!duplicate)
        {
            memcpy(curEntry, &(setlist->entries[i]), sizeof(SetlistEntry));
            curEntry++;
        }
    }

    free(setlist->entries);
    setlist->count = newCount;
    setlist->entries = newEntries;
    return 0;
}

void setlist_shuffle(Setlist *setlist)
{
    srand(time(NULL));
    for (int i = 0; i < setlist->count; i++)
    {
        int j = rand() % setlist->count;
        SetlistEntry temp = setlist->entries[i];
        setlist->entries[i] = setlist->entries[j];
        setlist->entries[j] = temp;
    }
}

int setlist_remove(Setlist *setlist, int index)
{
    if (index < 0 || index >= setlist->count)
    {
        return -1;
    }
    for (int i = index; i < setlist->count - 1; i++)
    {
        setlist->entries[i] = setlist->entries[i + 1];
    }
    setlist->count--;
    return 0;
}

// int main(void)
// {
//     if (setlist_load("/home/lars/.clonehero/Setlists/test3.setlist") != 0)
//     {
//         printf("Failed to load setlist\n");
//     }
//     else
//     {
//         printf("Setlist loaded\n");
//     }
//     printf("Setlist loaded with %d entries\n", setlist->count);
//     printf("Setlist has %d duplicates\n", setlist_getDuplicateCount());
//     // printf("Removing duplicates...\n");
//     // setlist_removeDuplicates();
//     // printf("Setlist has %d duplicates\n", setlist_getDuplicateCount());
//     // printf("Setlist has %d entries\n", setlist->count);
//     printf("Shuffling setlist...\n");
//     setlist_shuffle();
//     // printf("Removing entry 0...\n");
//     // setlist_remove(0);
//     printf("Setlist has %d entries\n", setlist->count);
//     if (setlist_save("/home/lars/.clonehero/Setlists/test3.setlist") != 0)
//     {
//         printf("Failed to save setlist\n");
//     }
//     else
//     {
//         printf("Setlist saved\n");
//     }
//     return 0;
// }
