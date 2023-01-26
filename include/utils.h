#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#include "sc_map.h"

struct Hashes {
    char **values;
    uint32_t count;
};

struct Song {
    char name[256];
    char artist[256];
    char md5[36];
    size_t nameLength;
    size_t artistLength;
};

struct Hashes readWholeFile(FILE *ptr, unsigned char *header);
int countDuplicates(struct Hashes *hashes);
void removeAllDuplicates(struct Hashes *hashes);
void readHeader(FILE *ptr, unsigned char *header);
uint32_t readSongCount(FILE *ptr);
void readChartHashes(FILE *ptr, char** hashes, uint32_t count);
void printHash(char *hash);
void freeHashesValues(struct Hashes *hashes);
void shuffleHashes(struct Hashes *hashes);
bool removeHash(struct Hashes *hashes, int index);
struct Song *hashToSong(struct sc_map_sv *map, char *hash);
int nrOfDigits(int n);
void getEqualsAndSpaces(int n, char *equalsBuffer, char *spacesBuffer);
bool isValidCommand(char input);
void printMenu();

#endif // UTILS_H
