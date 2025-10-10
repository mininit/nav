#include <ncurses.h>
#include <stdio.h>

#include "globals.h"
#include "draw.h"

int win_h, win_w;
size_t visible_rows;

void draw_upper_window()
{
  werase(upper_window);
  wattron(upper_window, COLOR_PAIR(2));
  mvwprintw(upper_window, 0, 0, "%s", cwd);
  wattroff(upper_window, COLOR_PAIR(2));

  wrefresh(upper_window);
}

void draw_lower_window()
{
  werase(lower_window);

  char selent[20];
  sprintf(selent, "%d / %zu", selected + 1, entry_count);

  mvwprintw(lower_window, 0, 0, "%s", selent);
  wrefresh(lower_window);
}

void draw_error(){
  werase(lower_window);
  mvwprintw(lower_window, 0, 0, "Error");
  wrefresh(lower_window);
}

void draw_main_window()
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
    case FILE_SYMLINK:
      wattron(main_window, COLOR_PAIR(4));
      break;
    default:
      break;
    }

    // Print the filename
    mvwprintw(main_window, y, 1, "%s", entries[i].name);

    // Turn off color & highlight for filename
    wattroff(main_window, A_REVERSE | COLOR_PAIR(2) | COLOR_PAIR(3) | COLOR_PAIR(4));

    if (entries[i].type == FILE_DIR)
    {
      waddstr(main_window, "/");
    }

    if (entries[i].type == FILE_EXEC)
    {
      waddstr(main_window, "*");
    }

    if (entries[i].type == FILE_SYMLINK)
    {
      waddstr(main_window, "@");
    }
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
