#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

bool is_file(char *path);
bool file_exists(char *path);
void clear_lines(int start, int end);
int nr_of_digits(int n);

#endif // UTILS_H
