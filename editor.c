#include <stdio.h>
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
 
extern struct node textbuffer[MAX_LINES];
extern int  head;
extern int  tail;
extern int  freeIdx;
extern int  opCount;
extern char currentFile[256];
 
void printText(int highlightLine, int inCharMode, int selectedCol);
void insertNode(int index, const char *newLine);
void deleteNode(int index);
void replaceChar(int lineIdx, int charPos, char newChar);
void save();
int  garbageCollection();
void checkAutoGC();
 
int cursorLine(int screenRow);
int cursorChar(int screenRow, int screenCol);

{
    int  selectedRow = 0;    
    int  selectedCol = 0;     
    int  inCharMode  = 0;     
    int  ch;                  
    char newLine[MAX_CHARS];  
    int  idx;                
    int  lineIdx;             
    int  newChar;             
 
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
 
    while (1)
    {
        printText(selectedRow, inCharMode, selectedCol);
        ch = getch();
 
 
        if (ch == KEY_UP)
        {
            if (inCharMode == 0 && selectedRow > 0)
                selectedRow--;
        }
        else if (ch == KEY_DOWN)
        {
            if (inCharMode == 0)
                selectedRow++;
        }
        else if (ch == KEY_LEFT)
        {
            if (inCharMode && selectedCol > 0)
                selectedCol--;
        }
        else if (ch == KEY_RIGHT)
        {
            if (inCharMode)
                selectedCol++;
        }
 
 
        else if (ch == 'i' || ch == 'I')
        {
            echo();
            mvprintw(MAX_DISPLAY + 2, 0, "New Line: ");
            refresh();
            getnstr(newLine, MAX_CHARS - 1);
            noecho();
 
            idx = cursorLine(selectedRow);
            insertNode(idx, newLine);
            checkAutoGC();
        }
 
 
        else if (ch == 'd' || ch == 'D')
        {
            idx = cursorLine(selectedRow);
            deleteNode(idx);
 
            if (selectedRow > 0)
                selectedRow--;
 
            checkAutoGC();
        }
 
 
        else if (ch == 'r' || ch == 'R')
        {
            selectedCol = 0;
            inCharMode  = 1;
        }
 
 
        else if (inCharMode && (ch == 'e' || ch == 'E'))
        {
            lineIdx = cursorLine(selectedRow);
 
            mvprintw(MAX_DISPLAY + 2, 0, "Yeni karakter: ");
            refresh();
            newChar = getch();  
 
            replaceChar(lineIdx, selectedCol, (char)newChar);
            inCharMode = 0;      /* Satır moduna geri dön */
        }
 
 
        else if (ch == 's' || ch == 'S')
        {
            save();
        }
 
 
        else if (ch == 'g' || ch == 'G')
        {
            garbageCollection();
        }
 
 
        else if (ch == 'q' || ch == 'Q')
        {
            break;
        }
    }
 
    endwin();
}
 
 
int cursorLine(int screenRow)
{
    int cur = head;
    int row = 0;
 
    while (cur != -1)
    {
        if (row == screenRow)
            return cur;
 
        cur = textbuffer[cur].next;
        row++;
    }
 
    return tail;
}
 
 
int cursorChar(int screenRow, int screenCol)
{
    int lineIdx;
    int len;
 
    lineIdx = cursorLine(screenRow);
 
    if (lineIdx == -1)
        return -1;
 
    len = strlen(textbuffer[lineIdx].statement);
 
    if (len == 0)
        return -1;
 
    if (screenCol >= len)
        return len - 1;
 
    return screenCol;
}
