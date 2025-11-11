#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "globals.h"
#include "load_files.h"
#include "draw.h"
#include "loop.h"

FileEntry *entries = NULL;
size_t entry_count = 0;
int selected = 0;
char cwd[1024];

int max_y, max_x;

size_t scroll_offset = 0;

int threshold = 3;

// Main

void handle_args(int argc, char const *argv[])
{
  if (argc > 1)
  {
    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)
    {
      printf("nav version %s\n", VERSION);
      exit(0);
    }
    else
    {
      printf("nav: invalid option: \"%s\"\n", argv[1]);
      exit(0);
    }
  }
}

int main(int argc, char const *argv[])
{
  handle_args(argc, argv);

  init_draw();

  if (!getcwd(cwd, sizeof(cwd)))
  {
    perror("getcwd");
    exit(1);
  }
  load_directory(cwd);

  refresh();
  draw_entries();

  loop();

  // Cleanup
  for (size_t i = 0; i < entry_count; i++)
    free(entries[i].name);
  free(entries);

  end_draw();

  return 0;
}