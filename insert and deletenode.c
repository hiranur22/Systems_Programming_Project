void insertNode(int index, const char *newLine)
{
    // Overflow check. (ze safety)
    if (freeIdx >= MAX_LINES)
    {
        printf("Ze Buffer is full.\n");
        return;
    }
    // how to use the function:
    //  char *strncpy(char *dest, const char *src, size_t n);

    //Ekleme 
    strncpy(textbuffer[freeIdx].statement, newLine, MAX_CHARS - 1);
    textbuffer[freeIdx].statement[MAX_CHARS - 1] = '\0'; // make sure end plug the "End" via null
    int newIdx = freeIdx;

    //note: next= -1 last sentence
            prev = -1 first sentence
            head = -1 editor empty 

    // link Güncelleme(head/tail güncelleme)
    if (head == -1) // first sentence.
    {
        textbuffer[newIdx].prev = -1;
        textbuffer[newIdx].next = -1;
        head = newIdx;
        tail = newIdx;
    }
    else if (index == tail || index == -1) // adding it to end
    {
        textbuffer[newIdx].prev = tail;
        textbuffer[newIdx].next = -1;
        textbuffer[tail].next = newIdx;
        tail = newIdx;
    }
    else // between first and last sentence
    {
        int nextIdx = textbuffer[index].next;
        textbuffer[newIdx].prev = index;
        textbuffer[newIdx].next = nextIdx;
        textbuffer[index].next = newIdx;
        textbuffer[nextIdx].prev = newIdx;
    }


    // uptade the Global counts.
    freeIdx++;
    opCount++;
}

void deleteNode(int index)
{
    //Safety(güvenlik)
    if (index < 0 || index >= MAX_LINES || head == -1){
        return;
    }


    //Remove link and tail/head
    if (index == head && index == tail)
    {
        head = -1;
        tail = -1;
    }
    else if (index == head)
    {
        head = textbuffer[index].next;
        textbuffer[head].prev = -1;
    }
    else if (index == tail)
    {
        tail = textbuffer[index].prev;
        textbuffer[tail].next = -1;
    }
    else
    {
        int prevIdx = textbuffer[index].prev;
        int nextIdx = textbuffer[index].next;

        textbuffer[prevIdx].next = nextIdx;
        textbuffer[nextIdx].prev = prevIdx;
    }


    textbuffer[index].next = -1;
    textbuffer[index].prev = -1;

    opCount++;
}