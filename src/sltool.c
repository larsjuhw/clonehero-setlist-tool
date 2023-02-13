#include <errno.h>
#include <getopt.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "sltool.h"
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
    printf("Usage: %s -s <path> [-o output_path] [-S songlist_path]\n",
           PROGRAM_NAME);
}

void display_menu(char *setlist_path, char *songlist_path, char *output_path)
{
    const char *options[] = {"Show Setlist", "Save (s)", "Exit (q)"};

    const int n_choices = sizeof(options) / sizeof(char *);
    int ch;
    int current = 0;
    int largest_option = strlen(options[0]);
    uint32_t duplicate_count = setlist_getDuplicateCount(setlist);

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
        mvprintw(3, 0, "Setlist has %" PRIu32 " duplicates", duplicate_count);
        if (songlist_path != NULL)
            mvprintw(4, 0, "Songlist has %" PRIu32 " songs",
                     songlist_map->size);

        for (int i = 0; i < n_choices; ++i)
        {
            int row = i + 6;
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
                list_songs(setlist_path, -1);
                duplicate_count = setlist_getDuplicateCount(setlist);
            }
            else if (current == 1)
            {
                // save the setlist
                save(output_path, n_choices);
            }
            else if (current == 2)
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
    if (setlist->count == 0)
    {
        PRINTW_STATUSBAR(n_choices, "Setlist is empty. Saving aborted.");
        getch();
        return;
    }

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

int list_songs(char *setlist_path, int swap_index)
{
    int list_size = 10;
    int setlist_changed = 1;
    int ch;
    int current = 0;
    int highest = 0;
    int n_choices = setlist->count;
    int options_size = n_choices;
    int index_digits = nr_of_digits(n_choices);
    char *options[options_size];

    while (1)
    {
        if (setlist_changed)
        {
            n_choices = setlist->count;
            list_size = MIN(list_size, n_choices);
            generate_options(setlist, songlist_map, options);
            if (current >= n_choices)
                current = n_choices - 1;
            if (highest + list_size > n_choices)
                highest = setlist->count - list_size;
            setlist_changed = 0;
        }
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
            if (i == swap_index)
                attron(A_BOLD);
            mvprintw(row, col, "[%0*d] %s", index_digits, i + 1, options[i]);
            attroff(A_REVERSE);
            attroff(A_BOLD);
        }
        if (swap_index == -1)
        {
            PRINTW_STATUSBAR(
                list_size - 5,
                " [r] remove song | [s] swap song | [z] shuffle list | [d] "
                "remove "
                "duplicates\n Press [UP] and [DOWN] to scroll. Press "
                "Press [q] to quit.");
        }
        else
        {
            PRINTW_STATUSBAR(
                list_size - 5,
                " Press [UP] and [DOWN] to scroll.\n"
                " Press [q] to abort. Press [ENTER] to swap song.");
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

        if (swap_index == -1)
        {
            if (ch == 'r')
            {
                if (setlist_remove(setlist, current) == 0)
                {
                    setlist_changed = 1;
                }
            }
            else if (ch == 's')
            {
                int swap_index = list_songs(setlist_path, current);
                if (swap_index != -1)
                {
                    setlist_swap(setlist, current, swap_index);
                    PRINTW_STATUSBAR(list_size - 5, "Swapped [%s] with [%s].",
                                     options[current], options[swap_index]);
                    setlist_changed = 1;
                }
                else
                {
                    PRINTW_STATUSBAR(list_size - 5, "Swap aborted.");
                }
                getch();
            }
            else if (ch == 'z')
            {
                PRINTW_STATUSBAR(list_size - 5, "Shuffling list...");
                setlist_shuffle(setlist);
                setlist_changed = 1;
                PRINTW_STATUSBAR(list_size - 5, "List shuffled.");
                getch();
            }
            else if (ch == 'd')
            {
                remove_duplicates(list_size - 5);
                setlist_changed = 1;
            }
        }
        refresh();
    }

    for (int i = 0; i < options_size; ++i)
    {
        free(options[i]);
    }

    if (ch == 'q')
        return -1;

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
    uint32_t old_count = setlist->count;
    if (setlist_removeDuplicates(setlist) == 0)
    {
        PRINTW_STATUSBAR(n_choices, "Removed %d duplicates.",
                         old_count - setlist->count);
    }
    else
    {
        PRINTW_STATUSBAR(n_choices, "Removing duplicates failed!");
    }
    getch();
}
