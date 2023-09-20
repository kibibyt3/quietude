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

typedef enum {NONE, EXIT}Command;

typedef struct{

  char header[DIRECTOR_LENGTH]; // headers and directors are the same length by definition.
  Command command;
  char message[MESSAGE_LENGTH];
  int  responsesCount;
  char responses[MAX_RESPONSES][RESPONSE_LENGTH];
  char directors[MAX_RESPONSES][DIRECTOR_LENGTH];
  char defaultDirector[DIRECTOR_LENGTH];

}Section;

Section sections[MAX_SECTIONS];
int sectionCount = 0;

int chatinit(char*);
static int findSection(char*);
static int findResponse(char*, int);
static int findResponsesBySection(int, char***);
static char* removeNewline(char*);
int chatio(char*, char***);
Command getActiveCommand();

char* director = NULL;
char* treeFileName;
bool treeFileParsed = false;
int command;
int sectionIndex;

int chatio(char* input, char*** output){

  if (!treeFileParsed){
    printf("ERROR: chat not initialized!");
    exit(1);
  }
  
  if (input != NULL){
    char* cleanInput = removeNewline(input);
    int responsesIndex = findResponse(cleanInput, sectionIndex);
    if (responsesIndex == -1){
      director = sections[sectionIndex].defaultDirector;
    } else{
      director = sections[sectionIndex].directors[responsesIndex];
    }
  }
  sectionIndex = findSection(director);
  command = sections[sectionIndex].command;

  *output = malloc(sizeof(intptr_t) * (sections[sectionIndex].responsesCount + 1));

  for (int i = 0; i < sections[sectionIndex].responsesCount + 1; i++){
    (*output)[i] = malloc(RESPONSE_LENGTH);
  }
  
  strcpy(*output[0], sections[sectionIndex].message);
  for (int i = 0; i < sections[sectionIndex].responsesCount; i++){
    strcpy((*output)[i + 1], sections[sectionIndex].responses[i]);
  }

  return sections[sectionIndex].responsesCount;
}

static char* removeNewline(char* dirtyString){
  char* cleanString = malloc(MESSAGE_LENGTH);
  for (int i = 0; i < MESSAGE_LENGTH; i++){
    if (dirtyString[i] == '\0'){
      cleanString[i] = '\0';
      return cleanString;
    }
    else if (dirtyString[i] == '\n'){
      cleanString[i] = '\0';
      return cleanString;
    }
    else{
      cleanString[i] = dirtyString[i];
    }
  }
}

static int findResponsesBySection(int sectionn, char*** output){
  *output = malloc(sections[sectionn].responsesCount);
  for (int i = 0; i < sections[sectionn].responsesCount; i++){
    strcpy((*output)[i], sections[sectionn].responses[i]);
  }
}

/* Find the section index given a section header. */
static int findSection(char* query){
  for (int i = 0; i < sectionCount; i++){
    if (strcmp(sections[i].header, query) == 0){
      return i;
    }
  }
  printf("ERROR: section not found!\n");
  exit(1);

}

/* Find the response index given a response and section index. */
static int findResponse(char* query, int sectionIndex){
  for (int i = 0; i < sections[sectionIndex].responsesCount; i++){
    if (strcmp(query, sections[sectionIndex].responses[i]) == 0){
      return i;
    }
  }
  return -1;
}

Command getActiveCommand(){
  return command;
}

int chatinit(char* filename){

  if (director != NULL){
    free(director);
  }
  director = malloc(DIRECTOR_LENGTH);
  char* expandedFilename = malloc(sizeof(filename) + 50);
  strcpy(expandedFilename, filename);
  FILE *treeFile;

  treeFile = fopen(expandedFilename, "r");
  if (treeFile == NULL){
    strcat(expandedFilename, ".tree");
    treeFile = fopen(expandedFilename, "r");
  }
  
  if (treeFile == NULL){
    printf("%s is not a valid tree file. Try --help.\n\a", expandedFilename);
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

  
  FILE *treeFile2 = fopen(expandedFilename, "r");
  
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
  treeFileParsed = true;
  director = malloc(DIRECTOR_LENGTH);
  strcpy(director, "opening");
}
