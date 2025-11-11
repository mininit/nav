#ifndef GLOBALS_H
#define GLOBALS_H

#include <ncurses.h>

typedef enum
{
  FILE_REGULAR,
  FILE_EXEC,
  FILE_DIR,
  FILE_SYMLINK_FILE,
  FILE_SYMLINK_DIR,
  FILE_BROKEN_SYMLINK
} FileType;

typedef struct
{
  char *name;
  FileType type;
} FileEntry;

extern FileEntry *entries;
extern size_t entry_count;
extern int selected;
extern char cwd[1024];

extern size_t scroll_offset;

extern int threshold;

#endif // GLOBALS_H