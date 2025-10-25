#include <ncurses.h>
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
WINDOW *main_window = NULL;
WINDOW *upper_window = NULL;
WINDOW *lower_window = NULL;

int max_y, max_x;

size_t scroll_offset = 0;

int threshold = 3;

// Main

int main(int argc, char const *argv[])
{
  if (argc > 1)
  {
    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)
    {
      printf("nav version %s\n", VERSION);
      return 0;
    }
    else
    {
      printf("nav: invalid option: \"%s\"\n", argv[1]);
      return 0;
    }
  }

  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);

  if (has_colors())
  {
    start_color();
    use_default_colors();
    init_pair(2, COLOR_CYAN, -1);    // directories
    init_pair(3, COLOR_GREEN, -1);   // executables
    init_pair(4, COLOR_MAGENTA, -1); // symlinks
  }

  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);
  main_window = newwin(max_y - 2, max_x, 1, 0);
  upper_window = newwin(1, max_x, 0, 0);
  lower_window = newwin(1, max_x, max_y - 1, 0);

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
  delwin(main_window);
  delwin(upper_window);
  delwin(lower_window);
  endwin();
  return 0;
}