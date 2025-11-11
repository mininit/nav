#include <ncurses.h>
#include <stdio.h>

#include "globals.h"
#include "draw.h"

int win_h, win_w;
size_t visible_rows;

WINDOW *main_window = NULL;
WINDOW *upper_window = NULL;
WINDOW *lower_window = NULL;

void add_suffix(int i)
{
  if (entries[i].type == FILE_DIR)
  {
    waddstr(main_window, "/");
  }

  if (entries[i].type == FILE_EXEC)
  {
    waddstr(main_window, "*");
  }

  if (entries[i].type == FILE_SYMLINK_FILE || entries[i].type == FILE_BROKEN_SYMLINK)
  {
    waddstr(main_window, "@");
  }

  if (entries[i].type == FILE_SYMLINK_DIR)
  {
    waddstr(main_window, "/");
  }
}

void init_draw()
{
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
}

static void draw_upper_window()
{
  werase(upper_window);
  wattron(upper_window, COLOR_PAIR(2));
  mvwprintw(upper_window, 0, 0, "%s", cwd);
  wattroff(upper_window, COLOR_PAIR(2));

  wrefresh(upper_window);
}

static void draw_lower_window()
{
  werase(lower_window);

  char selent[64];
  if (entry_count == 0)
    snprintf(selent, sizeof(selent), "0 / 0");
  else
    snprintf(selent, sizeof(selent), "%d / %zu", selected + 1, entry_count);

  mvwprintw(lower_window, 0, 0, "%s", selent);
  wrefresh(lower_window);
}

void draw_error()
{
  werase(lower_window);
  mvwprintw(lower_window, 0, 0, "Error");
  wrefresh(lower_window);
}

static void draw_main_window()
{
  werase(main_window);

  size_t end = scroll_offset + visible_rows;
  if (end > entry_count)
    end = entry_count;

  for (size_t i = scroll_offset; i < end; i++)
  {
    int y = (int)(i - scroll_offset) + 1;

    // Highlight the selected line for the filename
    if ((int)i == selected)
      wattron(main_window, A_REVERSE);

    // Set color for main filename
    switch (entries[i].type)
    {
    case FILE_DIR:
      wattron(main_window, COLOR_PAIR(2));
      break;
    case FILE_EXEC:
      wattron(main_window, COLOR_PAIR(3));
      break;
    case FILE_BROKEN_SYMLINK:
    case FILE_SYMLINK_FILE:
    case FILE_SYMLINK_DIR:
      wattron(main_window, COLOR_PAIR(4));
      break;
    default:
      break;
    }

    // Print the filename
    mvwprintw(main_window, y, 1, "%s", entries[i].name);

    // Turn off color & highlight for filename
    wattroff(main_window, A_REVERSE | COLOR_PAIR(2) | COLOR_PAIR(3) | COLOR_PAIR(4));

    add_suffix(i);
  }

  if (scroll_offset >= 1)
  {
    mvwprintw(main_window, 0, 1, "...");
  }

  if (entry_count > visible_rows && scroll_offset < entry_count - visible_rows)
  {
    mvwprintw(main_window, win_h - 1, 1, "...");
  }

  wrefresh(main_window);
}

void draw_entries()
{
  getmaxyx(main_window, win_h, win_w);
  visible_rows = win_h - 2;
  draw_main_window();
  draw_upper_window();
  draw_lower_window();
}

void end_draw()
{
  delwin(main_window);
  delwin(upper_window);
  delwin(lower_window);
  endwin();
}