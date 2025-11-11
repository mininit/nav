#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>

#include "load_files.h"
#include "globals.h"

FileType get_file_type(const char *path)
{
  struct stat st;

  if (lstat(path, &st) != 0)
    return FILE_REGULAR; // or FILE_UNKNOWN if you prefer

  // Handle symlink separately
  if (S_ISLNK(st.st_mode))
  {
    struct stat target;
    // Try to follow the link
    if (stat(path, &target) == 0)
    {
      if (S_ISDIR(target.st_mode))
        return FILE_SYMLINK_DIR;
      else if (S_ISREG(target.st_mode))
        return FILE_SYMLINK_FILE;
      else if (target.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
        return FILE_SYMLINK_FILE; // executable symlink target
    }
    // If stat fails → broken symlink or dangling target
    return FILE_BROKEN_SYMLINK;
  }

  if (S_ISDIR(st.st_mode))
    return FILE_DIR;

  if (S_ISREG(st.st_mode))
  {
    if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
      return FILE_EXEC;
    return FILE_REGULAR;
  }

  return FILE_REGULAR;
}

void open_in_vim(const char *filepath)
{
  pid_t pid = fork();
  if (pid == 0)
  {
    // Child process: replace with vim
    execlp("vim", "vim", filepath, (char *)NULL);
    // If execlp fails
    exit(1);
  }
  else if (pid > 0)
  {
    // Parent process: wait for vim to exit
    int status;
    waitpid(pid, &status, 0);
  }
  else
  {
    // Fork failed
    perror("fork");
  }
}

void path_join(char *dest, size_t size, const char *base, const char *child)
{
  if (strcmp(base, "/") == 0)
    snprintf(dest, size, "/%s", child); // avoid double slash
  else
    snprintf(dest, size, "%s/%s", base, child);
}

static int compare_entries(const void *a, const void *b)
{
  const FileEntry *ea = (const FileEntry *)a;
  const FileEntry *eb = (const FileEntry *)b;
  return strcasecmp(ea->name, eb->name); // or strcmp for case-sensitive
}

int load_directory(const char *dir_path)
{
  DIR *dir = opendir(dir_path);

  // Free old entries
  for (size_t i = 0; i < entry_count; i++)
    free(entries[i].name);
  free(entries);
  entries = NULL;
  entry_count = 0;

  if (!dir)
  {
    // Can't list contents, but still "enter" directory
    strncpy(cwd, dir_path, sizeof(cwd) - 1);
    cwd[sizeof(cwd) - 1] = '\0';
    selected = 0;
    return 0;
  }

  // Normal directory reading
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
  {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    FileEntry *tmp = realloc(entries, sizeof(FileEntry) * (entry_count + 1));
    if (!tmp)
    {
      closedir(dir);
      return 1;
    }
    entries = tmp;

    char full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

    entries[entry_count].name = strdup(entry->d_name);
    entries[entry_count].type = get_file_type(full_path);
    entry_count++;
  }

  closedir(dir);

  // Sort alphabetically
  if (entry_count > 1)
    qsort(entries, entry_count, sizeof(FileEntry), compare_entries);

  strncpy(cwd, dir_path, sizeof(cwd) - 1);
  cwd[sizeof(cwd) - 1] = '\0';
  selected = 0;

  return 0;
}

bool is_dir(const char *path)
{
  struct stat st;
  return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}