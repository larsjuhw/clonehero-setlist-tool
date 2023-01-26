#ifndef CLONEHERO_H
#define CLONEHERO_H

void printMenu();
char getInput();
void analysis();
void removeSong();
void shufflePlaylist();
void swapSongs();
void removeDuplicates();
void saveSetlist();
void listSongs();
int loadSonglist();
void handleInput(char input);
void printUsage();

const unsigned char HEADER[] = {0xEA, 0xEC, 0x33, 0x01};
const unsigned char SUFFIX[] = {0x64, 0x00};
const unsigned char PREFIX = 0x20;

#endif // CLONEHERO_H
