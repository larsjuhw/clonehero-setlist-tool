#include "utils.h"
#include "songlist.h"
#include "setlist.h"
#include <ncurses.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>

bool is_file(char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

bool file_exists(char *path) { return access(path, F_OK) == 0; }

void clear_lines(int start, int end)
{
    int x, y;
    getyx(stdscr, y, x);
    for (int i = start; i <= end; ++i)
    {
        move(i, 0);
        clrtoeol();
    }
    move(y, x);
}

int nr_of_digits(int n)
{
    int count = 0;
    while (n != 0)
    {
        n /= 10;
        ++count;
    }
    return count;
}

void generate_options(Setlist *setlist, struct sc_map_sv *songlist_map,
                      char **options)
{
    for (int i = 0; i < setlist->count; ++i)
    {
        SetlistEntry *current_entry = &(setlist->entries[i]);

        char *hash_str = malloc(sizeof(char) * (current_entry->length + 1));
        memcpy(hash_str, current_entry->hash, current_entry->length);
        hash_str[current_entry->length] = '\0';

        if (songlist_map != NULL)
        {
            Song *song = lookup_song(songlist_map, hash_str);

            if (song != NULL)
            {
                char *name = song->title;
                char *artist = song->artist;
                char *option =
                    malloc((strlen(name) + strlen(artist) + 4) * sizeof(char));
                sprintf(option, "%s - %s", artist, name);
                free(hash_str);
                options[i] = option;
            }
            else
            {
                options[i] = hash_str;
            }
        }
        else
        {
            options[i] = hash_str;
        }
    }
}
