#include "chatwindow.c"

#define CHAT_WIDTH   40
#define READ_HEIGHT  10
#define COLOR_WHITE1 9
#define COLOR_GREY1  10
#define COLOR_BLACK1 11

int main(int argc, char** argv){
  
  WINDOW* readwin;
  WINDOW* chatwin;
  WINDOW* mainwin;

  WINDOW* readwinBorder;
  WINDOW* chatwinBorder;
  WINDOW* mainwinBorder;

  initscr();
  cbreak();
  noecho();
  start_color();

  init_color(COLOR_GREY1, 300, 300, 300);
  init_color(COLOR_WHITE1, 1000, 1000, 1000);
  init_color(COLOR_BLACK1, 0, 0, 0);

  init_pair(1, COLOR_WHITE1, COLOR_BLACK1);
  init_pair(2, COLOR_GREY1, COLOR_BLACK1);

  readwin = newwin(READ_HEIGHT, (COLS - 5) - CHAT_WIDTH, (LINES - 1) - READ_HEIGHT, 1);
  chatwin = newwin(LINES - 2, CHAT_WIDTH, 1, (COLS - 1) - CHAT_WIDTH);
  mainwin = newwin((LINES - 4) - READ_HEIGHT, (COLS - 5) - CHAT_WIDTH, 1, 1);
  
  readwinBorder = newwin(READ_HEIGHT + 2, (COLS - 3) - CHAT_WIDTH, (LINES - 2) - READ_HEIGHT, 0);
  chatwinBorder = newwin(LINES, CHAT_WIDTH + 2, 0, (COLS - 2) - CHAT_WIDTH);
  mainwinBorder = newwin((LINES - 2) - READ_HEIGHT, (COLS - 3) - CHAT_WIDTH, 0, 0);

  keypad(readwin, TRUE);
  keypad(chatwin, TRUE);
  keypad(mainwin, TRUE);


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
  
  wrefresh(readwin);
  wrefresh(chatwin);
  wrefresh(mainwin);

  int ch;
  int active = 0;
  while (true){
    switch (active){
      case 0:
	wattron(mainwinBorder, COLOR_PAIR(1));
	box(mainwinBorder, 0, 0);
	wrefresh(mainwinBorder);
	wrefresh(mainwin);
	wattron(chatwinBorder, COLOR_PAIR(2));
	wattron(readwinBorder, COLOR_PAIR(2));
	box(chatwinBorder, 0, 0);
	box(readwinBorder, 0, 0);
	wrefresh(chatwinBorder);
	wrefresh(readwinBorder);
	wrefresh(chatwin);
	wrefresh(readwin);
	wrefresh(mainwin);
        ch = wgetch(mainwin);
	break;
      case 1:
	wattron(chatwinBorder, COLOR_PAIR(1));
	box(chatwinBorder, 0, 0);
	wrefresh(chatwinBorder);
	wrefresh(chatwin);
	wattron(mainwinBorder, COLOR_PAIR(2));
	wattron(readwinBorder, COLOR_PAIR(2));
	box(mainwinBorder, 0, 0);
	box(readwinBorder, 0, 0);
	wrefresh(mainwinBorder);
	wrefresh(readwinBorder);
	wrefresh(mainwin);
	wrefresh(readwin);
	wrefresh(chatwin);
        ch = wgetch(chatwin);
	break;
      case 2:
	wattron(readwinBorder, COLOR_PAIR(1));
	box(readwinBorder, 0, 0);
	wrefresh(readwinBorder);
	wrefresh(readwin);
	wattron(mainwinBorder, COLOR_PAIR(2));
	wattron(chatwinBorder, COLOR_PAIR(2));
	box(mainwinBorder, 0, 0);
	box(chatwinBorder, 0, 0);
	wrefresh(mainwinBorder);
	wrefresh(chatwinBorder);
	wrefresh(mainwin);
	wrefresh(chatwin);
	wrefresh(readwin);
        ch = wgetch(readwin);
	break;
    }
    switch (ch){
      case KEY_UP:
	active = 0;
        break;
      case KEY_RIGHT:
	active = 1;
	break;
      case KEY_DOWN:
	active = 2;
	break;
      case KEY_LEFT:
	active = 0;
	break;
      case KEY_F(1):
	endwin();
	exit(0);
	break;
      default:
        if (active == 0){waddch(mainwin, ch);}
        else if (active == 1){waddch(chatwin, ch);}
  	else if (active == 2){waddch(readwin, ch);}
  	break;
    }
  }

  endwin();
  exit(0);
  //chatWindow(chatwin); 
}
