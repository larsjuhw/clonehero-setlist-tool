#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include "setlist.h"
#include "songlist.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

bool is_file(char *path);
bool file_exists(char *path);
void clear_lines(int start, int end);
int nr_of_digits(int n);
void generate_options(Setlist *setlist, struct sc_map_sv *songlist_map,
                      char **options);

#endif // UTILS_H
