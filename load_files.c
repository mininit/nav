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
  if (lstat(path, &st) == -1)
  {
    /* Could not stat — treat conservatively as regular file */
    return FILE_REGULAR;
  }

  if (S_ISLNK(st.st_mode))
    return FILE_SYMLINK;
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
    _exit(1);
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

int load_directory(const char *dir_path)
{
  DIR *dir = opendir(dir_path);
  if (!dir)
  {
    /* perror("opendir"); */
    return 1;
  }

  // Free old entries
  for (size_t i = 0; i < entry_count; i++)
    free(entries[i].name);
  free(entries);
  entries = NULL;
  entry_count = 0;

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
  {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    FileEntry *tmp = realloc(entries, sizeof(FileEntry) * (entry_count + 1));
    if (!tmp)
    {
      /* perror("realloc"); */
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

  // Keep cwd as the logical path
  strncpy(cwd, dir_path, sizeof(cwd) - 1);
  cwd[sizeof(cwd) - 1] = '\0';

  selected = 0;

  return 0;
}