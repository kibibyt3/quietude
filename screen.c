#include "windows.c"
#include "chatwindow.c"

int main(void){
  
  WINDOW* mainwinParent;
  WINDOW* chatwinParent;
  WINDOW* readwinParent;

  WINDOW* mainwin;
  WINDOW* chatwin;
  WINDOW* readwin;

  /* screen initialisation */
  initscr();
  cbreak();

  mainwin = init_mainwin(&mainwinParent);
  chatwin = init_chatwin(&chatwinParent);
  readwin = init_readwin(&readwinParent);

  wrefresh(mainwinParent);
  wrefresh(chatwinParent);
  wrefresh(readwinParent);

  chatWindow(chatwin);

  /* cleanup */
  endwin();
  exit(0);
}
