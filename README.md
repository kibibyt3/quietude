# Q

This is a game TUI-based game being written by kibibyt3. This repo contains the
dev tools as well as the game proper.

# Installation instructions

1. `git clone https://github.com/kibibyt3/quietude`
2. `cd quietude`
3. `make`
4. `./quietude`

# Quietude

### Controls

- Arrow keys: Navigate

# Development tools

All development tool executables reside [here](devel-utils) in `devel-utils/`.

## devel-walk

Development tool to create playable areas in the qwalk module of quietude.

### Controls

- In the default mode:
  - `wasd` or Arrow keys: Navigate cursor
  - `z`: Toggle cursor altitude
  - `c`: Copy current object
  - `v`: Paste copied object
  - `e`: Switch to edit mode
  - `x`: Save
  - `q`: Exit

- In edit mode:
  - Arrow keys: Navigate
  - Enter: Select attribute to modify.
  - In string entry submode:
    - Enter: Submit string.
    - Backspace: Delete previous character.
    - Delete: Delete current character.
    - SHIFT-Delete: Clear field.
    - Home: Go to beginning of field.
    - End: Go to end of field.
    - F1: leave without saving changes.
  - In choice selection submode:
    - Enter: Submit string.
    - Page Up or CTRL-`b`: Scroll up one page.
    - Page Down or CTRL-`f`: Scroll down one page.
    - Tab: attempt to autocomplete word in entry field.
    - ESC: leave without saving changes.
