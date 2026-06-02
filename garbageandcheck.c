#include <string.h>

int garbageCollection()
{
    struct node newBuffer[MAX_LINES];
    int oldIdx;
    int newIdx;
    int i;

    for (i = 0; i < MAX_LINES; i++)
    {
        newBuffer[i].statement[0] = '\0';
        newBuffer[i].next = -1;
        newBuffer[i].prev = -1;
    }

    oldIdx = head;
    newIdx = 0;

    while (oldIdx != -1 && newIdx < MAX_LINES)
    {
        strncpy(newBuffer[newIdx].statement,
                textbuffer[oldIdx].statement,
                MAX_CHARS - 1);

        newBuffer[newIdx].statement[MAX_CHARS - 1] = '\0';
        newBuffer[newIdx].prev = newIdx - 1;

        if (newIdx > 0)
            newBuffer[newIdx - 1].next = newIdx;

        oldIdx = textbuffer[oldIdx].next;
        newIdx++;
    }

    if (newIdx == 0)
    {
        head = -1;
        tail = -1;
        freeIdx = 0;
    }
    else
    {
        head = 0;
        tail = newIdx - 1;
        newBuffer[tail].next = -1;
        freeIdx = newIdx;
    }

    for (i = 0; i < MAX_LINES; i++)
        textbuffer[i] = newBuffer[i];

    opCount = 0;

    return freeIdx;
}

void checkAutoGC()
{
    if (freeIdx >= MAX_LINES || opCount >= GC_THRESHOLD)
        garbageCollection();
}
