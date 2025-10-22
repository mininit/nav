# nav

**nav** is a lightweight terminal-based file navigator built with `ncurses`. It allows you to browse directories, follow symlinks, and open files directly in `vim`.

## Features

- Navigate directories in a TUI interface.
- Supports directory symlinks.
- Open files in `vim` without leaving the program permanently.
- Color-coded file types.

## Installation

### Homebrew

```bash
brew install mininit/tap/nav
```

## Key Bindings

| Key                       | Action                                      |
|---------------------------|---------------------------------------------|
| `Up Arrow`/`Down Arrow`   | Move selection up or down                   |
| `Left Arrow`              | Go up to the parent directory               |
| `Right Arrow`/`Enter`/`e` | Open selected directory or open file in Vim |
| `Shift + H`               | Select the first entry in list              |
| `Shift + M`               | Select the middle entry in list             |
| `Shift + L`               | Select the last entry in list               |
| `q`                       | Quit the program                            |

### Notes
- When entering a directory, your scroll and selection positions are remembered.
- When returning (`Left Arrow`), the previously selected directory is highlighted.
- Symlinked directories are followed, while file symlinks open normally in Vim.
- Vim runs as a subprocess — when you exit Vim, you'll return to the navigator.
