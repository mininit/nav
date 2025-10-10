#ifndef LOAD_FILE_H
#define LOAD_FILE_H

#include "globals.h"

FileType get_file_type(const char *path);
void open_in_vim(const char *filepath);
void path_join(char *dest, size_t size, const char *base, const char *child);
int load_directory(const char *dir_path);

#endif // LOAD_FILE_H