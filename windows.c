#include <ncurses.h>

#define READ_HEIGHT 15
#define CHAT_WIDTH 40

WINDOW* init_readwin(WINDOW**);
WINDOW* init_mainwin(WINDOW**);
WINDOW* init_chatwin(WINDOW**);

WINDOW* init_readwin(WINDOW** winBorder){

  WINDOW* win;
  
  *winBorder = newwin(READ_HEIGHT, (COLS - 1) - CHAT_WIDTH, (LINES - READ_HEIGHT), 0);
  win = derwin(*winBorder, READ_HEIGHT - 2, (COLS - 3) - CHAT_WIDTH, 1, 1);
  box(*winBorder, 0, 0);
  scrollok(win, TRUE);

  return win;

}


WINDOW* init_mainwin(WINDOW** winBorder){

  WINDOW* win;
  
  *winBorder = newwin(LINES - READ_HEIGHT, (COLS - 1) - CHAT_WIDTH, 0, 0);
  win = derwin(*winBorder, (LINES - 2) - READ_HEIGHT, (COLS - 3) - CHAT_WIDTH, 1, 1);
  box(*winBorder, 0, 0);
  scrollok(win, TRUE);

  return win;

}


WINDOW* init_chatwin(WINDOW** winBorder){

  WINDOW* win;
   
  *winBorder = newwin(LINES, CHAT_WIDTH, 0, (COLS - CHAT_WIDTH));
  win = derwin(*winBorder, LINES - 2, CHAT_WIDTH - 2, 1, 1);
  box(*winBorder, 0, 0);
  scrollok(win, TRUE);

  return win;

}
