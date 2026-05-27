#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define MAX_LINES    100
#define MAX_CHARS     40
#define MAX_DISPLAY   30
#define GC_THRESHOLD  10


struct node {
    char statement[MAX_CHARS];
    int  next;
    int  prev;
};


struct node textbuffer[MAX_LINES];
int head;
int tail;
int freeIdx;
int opCount;
char currentFile[256];


void  initBuffer();
void  edit(char *filename);
int   cursorLine(int screenRow);
int   cursorChar(int screenRow, int screenCol);
void  deleteNode(int index);
void  insertNode(int index, const char *newLine);
void  replaceChar(int lineIdx, int charPos, char newChar);
void  printText(int highlightLine, int inCharMode, int selectedCol);
void  save();
int   garbageCollection();
void  checkAutoGC();
void  runEditor();



void initBuffer() {
    int i;
    for (i = 0; i < MAX_LINES; i++) {
        textbuffer[i].statement[0] = '\0';
        textbuffer[i].next = -1;
        textbuffer[i].prev = -1;
    }

    head    = -1;
    tail    = -1;
    freeIdx =  0;
    opCount =  0;
    currentFile[0] = '\0';
}


void edit(char *filename) {
    FILE *fp;
    char  line[MAX_CHARS + 2];
    int   i;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        strncpy(currentFile, filename, 255);
        return;
    }

    initBuffer();
    strncpy(currentFile, filename, 255);

    i = 0;
    while (fgets(line, sizeof(line), fp) != NULL && i < MAX_LINES) {

        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        strncpy(textbuffer[i].statement, line, MAX_CHARS - 1);
        textbuffer[i].statement[MAX_CHARS - 1] = '\0';

        if (i == 0) {
            textbuffer[i].prev = -1;
            head = 0;
        } else {
            textbuffer[i].prev     = i - 1;
            textbuffer[i-1].next   = i;
        }

        textbuffer[i].next = -1;
        tail = i;
        i++;
    }

    freeIdx = i;
    fclose(fp);
}

