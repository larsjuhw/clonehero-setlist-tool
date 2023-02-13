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
