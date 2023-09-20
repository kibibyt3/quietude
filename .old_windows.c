#include "chatwindow.c"

#define READ_HEIGHT 15
#define CHAT_WIDTH 40

int main(int argc, char** argv){
  
  WINDOW* readwin;
  WINDOW* chatwin;
  WINDOW* mainwin;

  WINDOW* readwinBorder;
  WINDOW* chatwinBorder;
  WINDOW* mainwinBorder;

  initscr();
  cbreak();
  echo();

  readwin = newwin(READ_HEIGHT, (COLS - 5) - CHAT_WIDTH, (LINES - 1) - READ_HEIGHT, 1);
  chatwin = newwin(LINES - 2, CHAT_WIDTH, 1, (COLS - 1) - CHAT_WIDTH);
  mainwin = newwin((LINES - 4) - READ_HEIGHT, (COLS - 5) - CHAT_WIDTH, 1, 1);
  
  readwinBorder = newwin(READ_HEIGHT + 2, (COLS - 3) - CHAT_WIDTH, (LINES - 2) - READ_HEIGHT, 0);
  chatwinBorder = newwin(LINES, CHAT_WIDTH + 2, 0, (COLS - 2) - CHAT_WIDTH);
  mainwinBorder = newwin((LINES - 2) - READ_HEIGHT, (COLS - 3) - CHAT_WIDTH, 0, 0);

  box(readwinBorder, 0, 0);
  box(chatwinBorder, 0, 0);
  box(mainwinBorder, 0, 0);

  wrefresh(readwinBorder);
  wrefresh(chatwinBorder);
  wrefresh(mainwinBorder);

  wrefresh(readwin);
  wrefresh(chatwin);
  wrefresh(mainwin);

  scrollok(chatwin, TRUE);
  
  chatWindow(chatwin); 
}
