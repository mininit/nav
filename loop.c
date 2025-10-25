#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

#include "globals.h"
#include "draw.h"
#include "loop.h"
#include "load_files.h"

typedef struct
{
  int selected;
  int scroll_offset;
} HistoryEntry;

HistoryEntry *history = NULL;
size_t history_capacity = 0;
size_t history_index = 0;

void push_history(int selected, int scroll_offset)
{
  if (history_index == history_capacity)
  {
    size_t new_capacity = (history_capacity == 0) ? 4 : history_capacity * 2;
    HistoryEntry *tmp = realloc(history, new_capacity * sizeof(*history));
    if (!tmp)
    {
      perror("realloc");
      exit(1);
    }
    history = tmp;
    history_capacity = new_capacity;
  }

  history[history_index++] = (HistoryEntry){selected, scroll_offset};
}

void loop()
{
  int ch;

  while ((ch = getch()) != 'q')
  {
    bool error = FALSE;
    switch (ch)
    {
    case KEY_UP:
      if (selected == 0)
      {
        // wrap to last entry
        selected = entry_count;
        if ((int)entry_count > visible_rows)
          scroll_offset = entry_count - visible_rows;
        if (selected == 0)
        {
          selected = entry_count;
          if (entry_count >= visible_rows)
          {
            scroll_offset = selected - visible_rows;
          }
        }
      }

      if (selected > 0)
        selected--;

      if (selected < (int)scroll_offset + threshold && scroll_offset > 0)
      {
        scroll_offset--;
      }
      break;

    case KEY_DOWN:
      // Reset to start if end of entries
      if (selected + 1 == entry_count)
      {
        selected = -1;
        scroll_offset = 0;
      }

      if (selected < (int)entry_count - 1)
        selected++;

      if (selected >= (int)(scroll_offset + visible_rows - threshold))
      {
        // Only scroll if more entries below
        if (scroll_offset + visible_rows < entry_count)
          scroll_offset++;
      }
      break;

    case '\n':
    case KEY_RIGHT:
    case 'e':
    {
      if (entry_count == 0)
        break;
      push_history(selected, scroll_offset);

      char new_path[PATH_MAX];
      path_join(new_path, sizeof(new_path), cwd, entries[selected].name);

      struct stat st;
      int is_dir = 0;

      // Check if it's a symlink pointing to a directory
      if (lstat(new_path, &st) == 0)
      {
        if (S_ISDIR(st.st_mode))
        {
          is_dir = 1;
        }
        else if (S_ISLNK(st.st_mode))
        {
          // Follow symlink target to see if it's a directory
          char target[PATH_MAX];
          ssize_t len = readlink(new_path, target, sizeof(target) - 1);
          if (len != -1)
          {
            target[len] = '\0';
            if (stat(new_path, &st) == 0 && S_ISDIR(st.st_mode))
            {
              is_dir = 1;
            }
          }
        }
      }

      if (is_dir)
      {
        // Enter directory (or symlink to directory)
        if (load_directory(new_path) == 0)
        {
          selected = 0;
          scroll_offset = 0;
        }
        else
        {
          error = TRUE;
        }
      }
      else
      {
        endwin();
        open_in_vim(new_path);
        refresh();
        draw_entries();
      }
      break;
    }

    case KEY_LEFT:
    {
      // Don’t go above root
      if (strcmp(cwd, "/") == 0)
        break;

      char *last_slash = strrchr(cwd, '/');
      if (last_slash)
      {
        if (last_slash == cwd)
          cwd[1] = '\0'; // Stay at root (/)
        else
          *last_slash = '\0';
      }

      load_directory(cwd);

      if (history_index > 0)
      {
        history_index--;
        selected = history[history_index].selected;
        scroll_offset = history[history_index].scroll_offset;
      }
      else
      {
        selected = 0;
        scroll_offset = 0;
      }

      break;
    }

    case 'H':
    {
      selected = 0;
      scroll_offset = 0;
      break;
    }

    case 'M':
    {
      selected = entry_count / 2;
      if (entry_count > visible_rows)
      {
        scroll_offset = (entry_count - visible_rows) / 2;
      }
      break;
    }

    case 'L':
    {
      selected = entry_count - 1;
      if (entry_count > visible_rows)
      {
        scroll_offset = entry_count - visible_rows;
      }
      break;
    }
    }

    draw_entries();
    if (error == TRUE)
    {
      draw_error();
    }
  }
}