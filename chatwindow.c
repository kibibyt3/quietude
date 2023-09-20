#include <ncurses.h>
#include "chat.c"
#include <stdint.h>

static int chatOut();
static int chatIn();
static int commandHandler();


int chatWindow(WINDOW*);

WINDOW* chat;
WINDOW* chatBorder;
char* chatFile = "1";
char* chatI;
char** chatO;
int optionc;

int chatWindow(WINDOW* argwin){
  chat = argwin;
  chatinit(chatFile);
  
  chatI = malloc(MESSAGE_LENGTH);

  optionc = chatio(NULL, &chatO) + 1;
  chatOut();
  while (true){
    chatIn();
    optionc = chatio(chatI, &chatO) + 1;
    commandHandler();
    chatOut(chat);
  }
}

static int chatOut(){
  for (int i = 0; i < optionc; i++){
    waddstr(chat, chatO[i]);
    waddch(chat, '\n');
  }
}

static int chatIn(){
  waddstr(chat, "> ");
  wgetstr(chat, chatI);
}

static int commandHandler(){
  Command activeCommand = getActiveCommand();
  switch (activeCommand){
    case (EXIT):
      endwin();
      exit(0);
      break;
  }
}
