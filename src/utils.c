#include "utils.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sc_map.h"

struct Hashes readWholeFile(FILE *ptr, unsigned char *header)
{
    fseek(ptr, 0, SEEK_SET);
    readHeader(ptr, header);
    uint32_t count = readSongCount(ptr);

    char **hashes = malloc(count * sizeof(char *));
    for (int i = 0; i < count; i++)
    {
        hashes[i] = malloc(33 * sizeof(char));
    }
    readChartHashes(ptr, hashes, count);
    struct Hashes r = {hashes, count};
    return r;
}

int countDuplicates(struct Hashes *hashes)
{
    char buffer[hashes->count][33];
    int insertIndex = 0;
    int duplicates = 0;
    for (int i = 0; i < hashes->count; i++)
    {
        bool collision = false;
        for (int j = 0; j < insertIndex; j++)
        {
            if (strcmp(hashes->values[i], buffer[j]) == 0)
            {
                collision = true;
                ++duplicates;
                break;
            }
        }
        if (!collision)
        {
            strcpy_s(buffer[insertIndex], 33, hashes->values[i]);
            insertIndex++;
        }
    }
    return duplicates;
}

void removeAllDuplicates(struct Hashes *hashes)
{
    char **newHashes = malloc(hashes->count * sizeof(char *));

    // Malloc for every string
    for (int i = 0; i < hashes->count; i++)
    {
        newHashes[i] = malloc(33 * sizeof(char));
    }

    int newSize = 0;
    for (int i = 0; i < hashes->count; i++)
    {
        bool collision = false;
        // Check if copy already contains the hash
        for (int j = 0; j < newSize; j++)
        {
            if (strcmp(hashes->values[i], newHashes[j]) == 0)
            {
                collision = true;
            }
        }
        if (!collision)
        {
            strcpy_s(newHashes[newSize++], 33, hashes->values[i]);
        }
    }

    freeHashesValues(hashes);
    hashes->values = newHashes;
    hashes->count = newSize;
}

void readHeader(FILE *ptr, unsigned char *header)
{
    fread(header, sizeof(unsigned char), 4, ptr);
}

uint32_t readSongCount(FILE *ptr)
{
    uint32_t count;
    fread(&count, sizeof(uint32_t), 1, ptr);
    return count;
}

void readChartHashes(FILE *ptr, char **hashes, uint32_t count)
{
    for (int i = 0; i < count; i++)
    {
        fseek(ptr, 1, SEEK_CUR); // Skip 0x20 prefix
        fgets(hashes[i], 33, ptr);
        fseek(ptr, 1, SEEK_CUR); // Skip 'd' suffix
        fseek(ptr, 1, SEEK_CUR); // Skip 0x00 divider
    }
}

void printHash(char *hash)
{
    char *cursor = hash;
    while (*cursor)
    {
        printf("[%02x]", *cursor);
        ++cursor;
    }
    printf("\n");
}

void freeHashesValues(struct Hashes *hashes)
{
    for (int i = 0; i < hashes->count; i++)
    {
        free(hashes->values[i]);
    }
    free(hashes->values);
}

void shuffleHashes(struct Hashes *hashes)
{
    for (int i = 0; i < hashes->count; i++)
    {
        int j = rand() % hashes->count;
        char *temp = hashes->values[i];
        hashes->values[i] = hashes->values[j];
        hashes->values[j] = temp;
    }
}

bool removeHash(struct Hashes *hashes, int index)
{
    if (index < 0 || index >= hashes->count)
    {
        return false;
    }
    free(hashes->values[index]);
    for (int i = index; i < hashes->count - 1; i++)
    {
        hashes->values[i] = hashes->values[i + 1];
    }
    hashes->count--;
    return true;
}

struct Song *hashToSong(struct sc_map_sv *map, char *hash)
{
    struct Song *lookup = sc_map_get_sv(map, hash);
    if (!sc_map_found(map))
    {
        return NULL;
    }

    return lookup;
}

int nrOfDigits(int n)
{
    int count = 0;
    while (n != 0)
    {
        n /= 10;
        ++count;
    }
    return count;
}

void getEqualsAndSpaces(int n, char *equalsBuffer, char *spacesBuffer)
{
    for (int i = 0; i < n; i++)
    {
        if (equalsBuffer != NULL)
        {
            equalsBuffer[i] = '=';
        }
        if (spacesBuffer != NULL)
        {
            spacesBuffer[i] = ' ';
        }
    }
    if (equalsBuffer != NULL)
    {
        equalsBuffer[n] = '\0';
    }
    if (spacesBuffer != NULL)
    {
        spacesBuffer[n] = '\0';
    }
}

bool isValidCommand(char input)
{
    return ('a' <= input && input <= 'f') || input == 'r' || input == 's' || input == 'q';
}

void printMenu()
{
    static char *equalsBuffer = NULL;
    int width = 30;
    if (equalsBuffer == NULL)
    {
        equalsBuffer = malloc((width+1) * sizeof(char));
    }

    getEqualsAndSpaces(width, equalsBuffer, NULL);
    printf("  %s\n", equalsBuffer);
    printf("  |  a: output analysis        |\n");
    printf("  |  b: remove a song          |\n");
    printf("  |  c: swap two songs         |\n");
    printf("  |  d: remove all duplicates  |\n");
    printf("  |  e: list all songs         |\n");
    printf("  |  f: shuffle playlist       |\n");
    printf("  |  s: save to disk           |\n");
    printf("  |  q: quit                   |\n");
    printf("  %s\n", equalsBuffer);
    printf("Action: ");
}
