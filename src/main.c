#include <errno.h>
#include <getopt.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "main.h"
#include "setlist.h"
#include "songlist.h"
#include "utils.h"

#define PROGRAM_NAME "sltool"

#define PRINTW_STATUSBAR(n_choices, ...)                                       \
    clear_lines(n_choices + 9, n_choices + 12);                                \
    attron(A_BOLD);                                                            \
    mvprintw(n_choices + 9, 0, __VA_ARGS__);                                   \
    attroff(A_BOLD);                                                           \
    refresh();

static Setlist *setlist = NULL;
static struct sc_map_sv *songlist_map = NULL;
int output_is_setlist = 0;

int main(int argc, char *argv[])
{
    setlist = malloc(sizeof(Setlist));

    char **setlist_path = malloc(sizeof(char *));
    char **output_path = malloc(sizeof(char *));
    char **songlist_path = malloc(sizeof(char *));

    if (parse_args(argc, argv, setlist_path, output_path, songlist_path) == -1)
    {
        usage();
        return -1;
    }

    if (setlist_load(setlist, *setlist_path) == -1)
    {
        printf("Failed to load setlist\n");
        return -1;
    }

    if (*songlist_path != NULL)
    {
        songlist_map = malloc(sizeof(struct sc_map_sv));
        if (songlist_load(songlist_map, *songlist_path) == -1)
        {
            printf("Failed to load songlist\n");
            return -1;
        }
    }

    // At this point, we have a setlist and a songlist (if one was provided)
    display_menu(*setlist_path, *songlist_path, *output_path);
}

int parse_args(int argc, char *argv[], char **setlist_path, char **output_path,
               char **songlist_path)
{
    static struct option long_options[] = {
        {"setlist", required_argument, 0, 's'},
        {"output", required_argument, 0, 'o'},
        {"songlist", required_argument, 0, 'S'},
        {0, 0, 0, 0}};

    int c;
    int setlist_path_set = 0;
    int output_path_set = 0;
    int songlist_path_set = 0;

    while ((c = getopt_long(argc, argv, "s:o:S:", long_options, NULL)) != -1)
    {
        switch (c)
        {
        case 's':
            *setlist_path = optarg;
            setlist_path_set = 1;
            break;
        case 'o':
            *output_path = optarg;
            output_path_set = 1;
            break;
        case 'S':
            *songlist_path = optarg;
            songlist_path_set = 1;
            break;
        default:
            return -1;
        }
    }

    if (!setlist_path_set)
    {
        printf("No setlist provided.\n");
        return -1;
    }

    *setlist_path = realpath(*setlist_path, NULL);
    if (*setlist_path == NULL)
    {
        perror("Invalid setlist path");
        return -1;
    }

    if (!output_path_set)
    {
        *output_path = *setlist_path;
        output_is_setlist = 1;
    }

    if (songlist_path_set)
    {
        *songlist_path = realpath(*songlist_path, NULL);
        if (*songlist_path == NULL)
        {
            perror("Invalid songlist path");
            return -1;
        }
    }
    else
    {
        *songlist_path = NULL;
    }

    return 0;
}

void usage()
{
    printf("Usage: %s (-s/--setlist) <path> (-o/--output) [path] "
           "(-S/--songlist) [path]\n",
           PROGRAM_NAME);
}

void display_menu(char *setlist_path, char *songlist_path, char *output_path)
{
    const char *options[] = {"Show Setlist", "Remove Duplicates", "Shuffle",
                             "Remove Song",  "Swap Songs",        "Save (s)",
                             "Exit (q)"};

    const int n_choices = sizeof(options) / sizeof(char *);
    int ch;
    int current = 0;
    int largest_option = strlen(options[1]);

    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    while (1)
    {
        clear();

        attron(A_STANDOUT);
        printw("%s\n\n", setlist_path);
        attroff(A_STANDOUT);

        mvprintw(2, 0, "Setlist has %" PRIu32 " songs", setlist->count);
        mvprintw(3, 0, "Setlist has %" PRIu32 " duplicates",
                 setlist_getDuplicateCount(setlist));
        if (songlist_path != NULL)
            mvprintw(4, 0, "Songlist has %" PRIu32 " songs",
                     songlist_map->size);

        for (int i = 0; i < n_choices; ++i)
        {
            int row = i + 7;
            int col = 2 + (largest_option - strlen(options[i])) / 2;
            if (i >= n_choices - 2)
                row++;
            if (current == i)
                attron(A_REVERSE);
            mvprintw(row, col, "%s", options[i]);
            attroff(A_REVERSE);
        }

        ch = getch();

        if (ch == KEY_UP)
        {
            if (current == 0)
                current = n_choices - 1;
            else
                --current;
        }
        else if (ch == KEY_DOWN)
        {
            if (current == n_choices - 1)
                current = 0;
            else
                ++current;
        }
        else if (ch == 10)
        {
            if (current == 0)
            {
                // list all songs in the setlist
                list_songs(setlist_path);
            }
            else if (current == 1)
            {
                // remove duplicates from the setlist
                remove_duplicates(n_choices);
            }
            else if (current == 2)
            {
                // shuffle the setlist
                shuffle(n_choices);
            }
            else if (current == 3)
            {
                // remove a song from the setlist
                remove_song(n_choices, setlist_path);
            }
            else if (current == 4)
            {
                // swap two songs in the setlist
                swap_songs(n_choices, setlist_path);
            }
            else if (current == 5)
            {
                // save the setlist
                save(output_path, n_choices);
            }
            else if (current == 6)
            {
                // exit
                break;
            }
        }
        else if (ch == 'q')
        {
            // exit
            break;
        }
        else if (ch == 's')
        {
            // save the setlist
            save(output_path, n_choices);
        }

        refresh();
    }
    endwin();
}

void save(char *output_path, int n_choices)
{
    if (output_is_setlist || file_exists(output_path))
    {
        if (output_is_setlist)
        {
            PRINTW_STATUSBAR(
                n_choices,
                "Are you sure you want to overwrite the setlist? (y/n)");
        }
        else
        {
            PRINTW_STATUSBAR(
                n_choices,
                "File already exists. Do you want to overwrite it? (y/n)");
        }

        int ch = getch();
        if (ch != 'y')
        {
            PRINTW_STATUSBAR(n_choices, "Saving setlist aborted.");
            getch();
            return;
        }
    }

    if (setlist_save(setlist, output_path) == -1)
    {
        PRINTW_STATUSBAR(n_choices, "Saving setlist failed!");
    }
    else
    {
        PRINTW_STATUSBAR(n_choices, "Succesfully saved setlist to [%s]\n",
                         output_path);
    }
    getch();
}

int list_songs(char *setlist_path)
{
    const int list_size = 10;
    int ch;
    int current = 0;
    int highest = 0;
    int n_choices = setlist->count;
    int index_digits = nr_of_digits(n_choices);
    char *options[n_choices];

    for (int i = 0; i < n_choices; ++i)
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

    while (1)
    {
        clear();

        attron(A_STANDOUT);
        mvprintw(0, 0, "%s", setlist_path);
        attroff(A_STANDOUT);

        for (int i = highest; i < highest + list_size; ++i)
        {
            int row = i + 3 - highest;
            int col = 2;
            if (current == i)
                attron(A_REVERSE);
            mvprintw(row, col, "[%0*d] %s", index_digits, i + 1, options[i]);
            attroff(A_REVERSE);
        }

        ch = getch();

        if (ch == KEY_UP)
        {
            if (current > 0)
                --current;
            if (current < highest)
                --highest;
        }
        else if (ch == KEY_DOWN)
        {
            if (current < n_choices - 1)
                ++current;
            if (current >= highest + list_size)
                ++highest;
        }
        else if (ch == 10)
        {
            break;
        }
        else if (ch == 'q')
        {
            current = -1;
            break;
        }

        refresh();
    }

    for (int i = 0; i < n_choices; ++i)
    {
        free(options[i]);
    }

    return current;
}

void remove_duplicates(int n_choices)
{
    if (setlist_getDuplicateCount(setlist) == 0)
    {
        PRINTW_STATUSBAR(n_choices, "The setlist contains no duplicates.");
        getch();
        return;
    }

    PRINTW_STATUSBAR(n_choices, "Removing duplicates...");
    setlist_removeDuplicates(setlist);
    PRINTW_STATUSBAR(n_choices, "Removed %d duplicates.", setlist->count);
    getch();
}

void shuffle(int n_choices)
{
    PRINTW_STATUSBAR(n_choices, "Shuffling setlist...");
    setlist_shuffle(setlist);
    PRINTW_STATUSBAR(n_choices, "Setlist shuffled.");
    getch();
}

void remove_song(int n_choices, char *setlist_path)
{
    PRINTW_STATUSBAR(n_choices, "Select a song to remove");
    getch();
    int index = list_songs(setlist_path);

    if (index == -1)
    {
        PRINTW_STATUSBAR(n_choices, "Removing song aborted.");
        getch();
        return;
    }

    if (songlist_map != NULL)
    {
        SetlistEntry *current_entry = &(setlist->entries[index]);
        char *hash_str = malloc(sizeof(char) * (current_entry->length + 1));
        memcpy(hash_str, current_entry->hash, current_entry->length);
        hash_str[current_entry->length] = '\0';
        Song *song = lookup_song(songlist_map, hash_str);
        free(hash_str);

        if (song != NULL)
        {
            PRINTW_STATUSBAR(n_choices, "Removing song %d: %s - %s..", index + 1,
                             song->artist, song->title);
            if (setlist_remove(setlist, index) == 0)
            {
                PRINTW_STATUSBAR(n_choices, "Removed song %d: %s - %s.",
                                 index + 1, song->artist, song->title);
            }
            else
            {
                PRINTW_STATUSBAR(n_choices, "Failed to remove song %d: %s - %s.",
                                 index + 1, song->artist, song->title);
            }
            getch();
            return;
        }
    }

    PRINTW_STATUSBAR(n_choices, "Removing song %d..", index + 1);
    setlist_remove(setlist, index);
    PRINTW_STATUSBAR(n_choices, "Removed song %d.", index + 1);
    getch();
}

void swap_songs(int n_choices, char *setlist_path)
{

}
