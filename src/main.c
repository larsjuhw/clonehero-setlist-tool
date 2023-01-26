#define _CRT_SECURE_NO_DEPRECATE
#include "main.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sc_map.h"

#include "utils.h"

static char *argv0;
static struct sc_map_sv *songmap = NULL;
static char *fp = NULL;
static char *songlistPath = NULL;
static FILE *ptr;
static struct Hashes hashes = {NULL, -1};

char getInput()
{
    char input;
    while (scanf(" %c", &input) != 1 || !isValidCommand(input))
    {
        printf("Invalid input: %c\n", input);
        printMenu();
    }
    return input;
}

void analysis()
{
    printf("\nAnalysis\n");
    printf("Songs: %" PRIu32 "\n", hashes.count);
    int duplicates = countDuplicates(&hashes);
    printf("Duplicates: %d\n\n", duplicates);
}

void removeSong()
{
    printf("\nSong removal\n");
    if (hashes.count == 0)
    {
        printf("No song left to remove\n");
        return;
    }

    if (hashes.values == NULL || hashes.count < 0)
    {
        fprintf(stderr, "Error: songs not loaded properly\n");
        return;
    }

    printf("Enter the index of the song to remove [0 .. %" PRIu32
           "] (-1 to abort): ",
           hashes.count - 1);

    int index;
    while (scanf(" %d", &index) < 1 || index < -1 ||
           (index >= hashes.count && index >= 0))
    {
        printf("Invalid index: %d\n", index);
        printf("Enter the song index to remove: ");
    }
    if (index == -1)
    {
        printf("Aborting..\n\n");
        return;
    }

    struct Song *song = NULL;
    if (songlistPath != NULL && songmap == NULL)
    {
        loadSonglist(songlistPath);
    }
    if (songmap != NULL)
    {
        song = hashToSong(songmap, hashes.values[index]);
    }
    if (song != NULL)
    {
        printf("\nRemoving song %d: %s - %s\n", index, song->artist,
               song->name);
    }
    else
    {
        printf("\nRemoving song %d: %s\n", index, hashes.values[index]);
    }

    if (removeHash(&hashes, index))
    {
        printf("Song removed!\n\n");
    }
    else
    {
        printf("Error: song not removed\n\n");
    }
}

void shufflePlaylist()
{
    printf("\nPlaylist shuffle\n");
    shuffleHashes(&hashes);
    printf("Done!\n\n");
}

void swapSongs()
{
    printf("\nSong swap\n");
    if (hashes.count < 2)
    {
        printf("Not enough songs to swap\n");
        return;
    }

    if (hashes.values == NULL || hashes.count < 0)
    {
        fprintf(stderr, "Error: songs not loaded properly\n");
        return;
    }

    if (songlistPath != NULL && songmap == NULL)
    {
        loadSonglist(songlistPath);
    }

    printf("Enter the index of the first song to swap [0 .. %" PRIu32
           "] (-1 to abort): ",
           hashes.count - 1);

    int index1;
    while (scanf(" %d", &index1) < 1 || index1 < -1 ||
           (index1 >= hashes.count && index1 >= 0))
    {
        printf("Invalid index: %d\n", index1);
        printf("Enter the index of the first song to swap [0 .. %" PRIu32
               "] (-1 to abort): ",
               hashes.count - 1);
    }

    if (index1 == -1)
    {
        printf("Aborting..\n");
        return;
    }

    printf("Enter the index of the second song to swap [0 .. %" PRIu32
           "] (-1 to abort): ",
           hashes.count - 1);

    int index2;
    while (scanf(" %d", &index2) < 1 || index2 < -1 || index1 == index2 ||
           (index2 >= hashes.count && index2 >= 0))
    {
        if (index1 == index2)
        {
            printf("Same index, try again\n");
        }
        else
        {
            printf("Invalid index: %d\n", index2);
        }
        printf("Enter the index of the second song to swap [0 .. %" PRIu32
               "] (-1 to abort): ",
               hashes.count - 1);
    }

    if (index2 == -1)
    {
        printf("Aborting..\n");
        return;
    }

    printf("\n");
    struct Song *song1 = NULL, *song2 = NULL;
    if (songmap != NULL)
    {
        song1 = hashToSong(songmap, hashes.values[index1]);
        song2 = hashToSong(songmap, hashes.values[index2]);
    }

    if (song1 != NULL)
    {
        printf("Swapping song %d: %s - %s\n", index1, song1->artist,
               song1->name);
    }
    else
    {
        printf("Swapping song %d: %s\n", index1, hashes.values[index1]);
    }

    if (song2 != NULL)
    {
        printf("    with song %d: %s - %s\n", index2, song2->artist,
               song2->name);
    }
    else
    {
        printf("    with song %d: %s\n", index2, hashes.values[index2]);
    }

    char *temp = hashes.values[index1];
    hashes.values[index1] = hashes.values[index2];
    hashes.values[index2] = temp;

    printf("Done!\n\n");
}

void removeDuplicates()
{
    printf("\nRemoving duplicates..\n");
    removeAllDuplicates(&hashes);
    printf("Done! You need to run the save command before the changes are "
           "final.\n\n");
}

void saveSetlist()
{
    printf("\nSaving to file..\n");
    if (hashes.values == NULL || hashes.count < 0)
    {
        fprintf(stderr, "Error: songs not loaded properly\n");
        return;
    }

    if (hashes.count == 0)
    {
        printf("No songs to save\n");
        return;
    }

    char input;
    printf("Do you want to overwrite the old file? (y/[n])\n");
    scanf(" %c", &input);

    FILE *outPtr;

    if (input == 'y' || input == 'Y')
    {
        // Overwrite
        printf("Overwriting file.. ");
        outPtr = fopen(fp, "wb");
    }
    else
    {
        // Save to {file}_new
        size_t oldLen = strlen(fp);
        size_t newLen = oldLen + 5;
        char fpOut[newLen];

        strcpy_s(fpOut, newLen, fp);
        strcat_s(fpOut, newLen, "_new");

        printf("Saving to %s .. ", fpOut);
        outPtr = fopen(fpOut, "wb");
    }

    // Write header section
    fwrite(HEADER, sizeof(HEADER), 1, outPtr);
    // Write length uint32_t
    fwrite(&hashes.count, sizeof(uint32_t), 1, outPtr);

    for (int i = 0; i < hashes.count; i++)
    {
        fwrite(&PREFIX, sizeof(PREFIX), 1, outPtr);
        fwrite(hashes.values[i], 32, 1, outPtr);
        fwrite(&SUFFIX, sizeof(SUFFIX), 1, outPtr);
    }

    fclose(outPtr);

    printf("done!\n\n");
}

void listSongs()
{
    printf("\nListing songs..\n");
    if (hashes.count == 0)
    {
        printf("No songs to list\n");
        return;
    }
    if (songlistPath == NULL)
    {
        printf("No path to songlist file provided.\n\n");
        printUsage();
        return;
    }

    if (songmap == NULL)
    {
        if (loadSonglist() < 0)
        {
            return;
        }
    }

    struct Song *foundSongs[hashes.count];
    size_t longestArtist = 9;
    size_t longestTitle = 9;

    for (int i = 0; i < hashes.count; i++)
    {
        struct Song *lookup = sc_map_get_sv(songmap, hashes.values[i]);
        if (!sc_map_found(songmap))
        {
            foundSongs[i] = NULL;
        }
        else
        {
            foundSongs[i] = lookup;
            if (lookup->artistLength > longestArtist)
            {
                longestArtist = lookup->artistLength;
            }
            if (lookup->nameLength > longestTitle)
            {
                longestTitle = lookup->nameLength;
            }
        }
    }

    int indexDigits = nrOfDigits(hashes.count);
    int totalLength = indexDigits + longestArtist + longestTitle + 5 + 4 + 1;
    char EQUALS[totalLength + 1];
    char SPACES[totalLength + 1];
    getEqualsAndSpaces(totalLength, EQUALS, SPACES);

    printf("\n  %.*s\n", totalLength, EQUALS);
    printf("  |%.*s|\n", totalLength - 2, SPACES);

    printf("  |   %.*s Artist %.*s  Title%.*s |\n", indexDigits, SPACES,
           (int)(longestArtist - 6), SPACES, (int)(longestTitle - 5), SPACES);

    printf("  |%.*s|\n", totalLength - 2, SPACES);
    printf("  |%.*s|\n", totalLength - 2, EQUALS);
    printf("  | %.*s |%.*s|\n", indexDigits, SPACES,
           totalLength - 5 - indexDigits, SPACES);

    for (int i = 0; i < hashes.count; i++)
    {
        struct Song *lookup = foundSongs[i];
        if (lookup == NULL)
        {
            printf(
                "  | %0*d | _UNKNOWN_ %.*s- _UNKNOWN_%.*s | <----- MD5=[%s]\n",
                indexDigits, i, (int)(longestArtist - 9), SPACES,
                (int)(longestTitle - 9), SPACES, hashes.values[i]);
        }
        else
        {
            printf("  | %0*d | %s %.*s- %s%.*s |\n", indexDigits, i,
                   lookup->artist, (int)(longestArtist - lookup->artistLength),
                   SPACES, lookup->name,
                   (int)(longestTitle - lookup->nameLength), SPACES);
        }
    }

    printf("  | %.*s |%.*s|\n", indexDigits, SPACES,
           totalLength - 5 - indexDigits, SPACES);
    printf("  %.*s\n\n", totalLength, EQUALS);
}

int loadSonglist()
{
    printf("Loading songlist..\n");
    FILE *slPtr = fopen(songlistPath, "r");
    if (slPtr == NULL)
    {
        fprintf(stderr, "Could not open file: %s\n", songlistPath);
        return -1;
    }

    char songCountStr[10];
    if (fgets(songCountStr, 10, slPtr) == NULL)
    {
        fprintf(stderr, "Error while parsing songlist\n");
        return -1;
    }

    int songCount = atoi(songCountStr);
    songmap = malloc(sizeof(struct sc_map_sv));
    sc_map_init_sv(songmap, songCount, 0);
    struct Song *songStructs[songCount];

    for (int i = 0; i < songCount; i++)
    {
        songStructs[i] = malloc(sizeof(struct Song));

        fgets(songStructs[i]->name, 256, slPtr);
        fgets(songStructs[i]->artist, 256, slPtr);
        fgets(songStructs[i]->md5, 36, slPtr);

        // Remove trailing newline
        songStructs[i]->name[strcspn(songStructs[i]->name, "\n")] = '\0';
        songStructs[i]->artist[strcspn(songStructs[i]->artist, "\n")] = '\0';
        songStructs[i]->md5[strcspn(songStructs[i]->md5, "\n")] = '\0';

        songStructs[i]->nameLength = strlen(songStructs[i]->name);
        songStructs[i]->artistLength = strlen(songStructs[i]->artist);

        sc_map_put_sv(songmap, songStructs[i]->md5, songStructs[i]);
    }
    return 0;
}

void handleInput(char input)
{
    switch (input)
    {
    case 'a':
        analysis();
        break;
    case 'b':
        removeSong();
        break;
    case 'c':
        swapSongs();
        break;
    case 'd':
        removeDuplicates();
        break;
    case 'e':
        listSongs();
        break;
    case 'f':
        shufflePlaylist();
        break;
    case 's':
        saveSetlist();
        break;
    case 'q':
        exit(0);
        break;
    default:
        fprintf(stderr, "Error: unknown input \"%c\"\n", input);
        break;
    }
}

int main(int argc, char *argv[])
{
    argv0 = argv[0];
    if (argc < 2)
    {
        printUsage();
        exit(1);
    }

    // Handle file opening
    fp = argv[1];
    ptr = fopen(fp, "rb");

    if (argc > 2)
    {
        songlistPath = argv[2];
    }

    if (ptr == NULL)
    {
        fprintf(stderr, "Could not open file: %s\n", fp);
        exit(1);
    }

    // Read file contents to hashes variable
    unsigned char header[5];
    hashes = readWholeFile(ptr, header);
    fclose(ptr);

    char input = 0;
    while (input != 'q')
    {
        printMenu();
        input = getInput();
        handleInput(input);
    }
    return 0;
}

void printUsage() { printf("Usage: %s <setlist_path> [songlist_path]\n", argv0); }
