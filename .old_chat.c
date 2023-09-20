#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MESSAGE_LENGTH 400
#define RESPONSE_LENGTH 400
#define DIRECTOR_LENGTH 400
#define MAX_RESPONSES 6
#define MAX_SECTIONS 100

enum Command{NONE, EXIT};

typedef struct{

  char header[DIRECTOR_LENGTH]; // headers and directors are the same length by definition.
  enum Command command;
  char message[MESSAGE_LENGTH];
  int  responsesCount;
  char responses[MAX_RESPONSES][RESPONSE_LENGTH];
  char directors[MAX_RESPONSES][DIRECTOR_LENGTH];
  char defaultDirector[DIRECTOR_LENGTH];

}Section;

Section sections[MAX_SECTIONS];
int sectionCount = 0;

int parser(int, char**);
int findSection(char*);
int findResponse(char*, int);
char* removeNewline(char*);

int main(int argc, char *argv[]){
  parser(argc, argv);
  char* response = malloc(RESPONSE_LENGTH);
  char* director = malloc(DIRECTOR_LENGTH);
  strcpy(director, "opening");
  
  while (true){

    int sectionIndex = findSection(director);

    // handles special behaviour from commands
    switch(sections[sectionIndex].command){
      case EXIT:
	exit(0);
	break;
    }

    printf("%s\n", sections[sectionIndex].message);
    fgets(response, MESSAGE_LENGTH, stdin);
    char* cleanResponse = removeNewline(response);
    int responsesIndex = findResponse(cleanResponse, sectionIndex);
    if (responsesIndex == -1){
      director = sections[sectionIndex].defaultDirector;
    } else{
      director = sections[sectionIndex].directors[responsesIndex];
    }
  }
}

char* removeNewline(char* dirtyString){
  char* cleanString = malloc(MESSAGE_LENGTH);
  for (int i = 0; i < MESSAGE_LENGTH; i++){
    if (dirtyString[i] == '\n'){
      cleanString[i] = '\0';
      return cleanString;
    }
    else{
      cleanString[i] = dirtyString[i];
    }
  }
}

/* Find the section index given a section header. */
int findSection(char* query){
  for (int i = 0; i < sectionCount; i++){
    if (strcmp(sections[i].header, query) == 0){
      return i;
    }
  }
  printf("ERROR: section not found!\n");
  exit(1);

}

/* Find the response index given a response and section index. */
int findResponse(char* query, int sectionIndex){
  for (int i = 0; i < sections[sectionIndex].responsesCount; i++){
    if (strcmp(query, sections[sectionIndex].responses[i]) == 0){
      return i;
    }
  }
  return -1;
}

int parser(int argc, char *argv[]){
  FILE *treeFile;
  
  if (argc == 1){
    printf("Invalid usage. Try --help.\n");
    exit(1);
  }

  treeFile = fopen(argv[1], "r");
  if (treeFile == NULL){
    strcat(argv[1], ".tree");
    treeFile = fopen(argv[1], "r");
  }
  
  if (treeFile == NULL){
    printf("%s is not a valid tree file. Try --help.\n\a", argv[1]);
    exit(1);
  }

  fclose(treeFile);

  /* BEGIN PROPER PARSE */
  
  int ch;
  char *emptyStringBuffer;
  char *stringBuffer;                        // set this to MESSAGE_LENGTH because 
  stringBuffer = malloc(MESSAGE_LENGTH);     // it's expected to be the largest
  emptyStringBuffer = malloc(MESSAGE_LENGTH);

  short	sectionIndex = -1;           /* start at -1 because sections are terminated
					ONLY by the initialisation of a new section
					header.                                  */
  short stringBufferIndex = 0;
  short directorIndex = 0;

  bool sectionFlag = false;
  bool commandFlag = false;     // These flags tell us
  bool messageFlag = false;     // where each char we   
  bool directorFlag = false;    // parse is meant to go.
  bool stringFlag = false;
  bool defaultFlag = false;

  
  FILE *treeFile2 = fopen(argv[1], "r");
  
  while ((ch = fgetc(treeFile2)) != EOF){
    
    if (stringFlag && (ch != '"')){                    // if we're in a string, don't treat 
      stringBuffer[stringBufferIndex++] = ch;          // ANY special character (excepting
      continue;                                        // double-quotes) as such. 
    }

    switch (ch){
      
      case '"':
        stringFlag = !stringFlag;
	break;
      
      case '\n':                                       // whitespace (incl. newlines) can be
	break;                                         // safely ignored when we know we
      case ' ':
	break;

      case ';':                                        /* Commands, messages, and directors       
						          all conclude with a semicolon. */
        if (commandFlag){
	  if (strcmp(stringBuffer, "EXIT")){           // when other commands are implemented,
            sections[sectionIndex].command = EXIT;     // check for them here with else ifs.
	  }
	  else{
            sections[sectionIndex].command = NONE;
	  }
          commandFlag = false;
	}
	else if (messageFlag){
	  stringBuffer[stringBufferIndex] = '\0';
          strcpy(sections[sectionIndex].message, stringBuffer);
	  messageFlag = false;
	}
        else if (directorFlag){
	  if (defaultFlag){
	    stringBuffer[stringBufferIndex] = '\0';
            strcpy(sections[sectionIndex].defaultDirector, stringBuffer);
	    defaultFlag = false;
	  }
	  else{
	    stringBuffer[stringBufferIndex] = '\0';
	    strcpy(sections[sectionIndex].directors[directorIndex++], stringBuffer);
	    directorFlag = false;
	  }
	}
	free(stringBuffer);
	stringBuffer = malloc(MESSAGE_LENGTH);
	stringBuffer[0] = '\0';
	stringBufferIndex = 0;
	break;
      
      case '<':
	if (sectionIndex == -1){
	  sectionIndex++;
	}
	else{
	  sections[sectionIndex++].responsesCount = directorIndex;  // We encounter a new section.
	}
	directorIndex = 0;
	sectionFlag = true;
	break;
      
      case '>':
	stringBuffer[stringBufferIndex] = '\0';
	strcpy(sections[sectionIndex].header, stringBuffer);
	free(stringBuffer);
	stringBuffer = malloc(MESSAGE_LENGTH);
	stringBuffer[0] = '\0';
	stringBufferIndex = 0;
	sectionFlag = false;
	break;

      case '.':
	messageFlag = true;
	break;

      case ':':                                        /* This necessarily indicates the
							  beginning of a director AND the
							  end of a response, hence no
							  responseFlag. */
	directorFlag = true;
	if (stringBuffer[0] == '\0'){            // This in part handles default directors.
	  defaultFlag = true;
	}
	else{
	  stringBuffer[stringBufferIndex] = '\0'; 
          strcpy(sections[sectionIndex].responses[directorIndex], stringBuffer);
	  free(stringBuffer);
	  stringBuffer = malloc(MESSAGE_LENGTH);
	  stringBuffer[0] = '\0';
	  stringBufferIndex = 0;
	}
	break;

      case '$':
	commandFlag = true;
	break;

      default:
	stringBuffer[stringBufferIndex++] = ch;
	break;
    }
  }
  sectionCount = sectionIndex + 1;
}
