#include "label.h"

int LINE = -7;
lnode HEAD = NULL;

void newLabel(int line, char * name, int offset) {
    lnode temp = (lnode)malloc(sizeof(LinkedList));
    temp -> addr = CODE_START + 2 * (line - 1);
    temp -> name = (char*)malloc(sizeof(char) * 10);
    strcpy(temp -> name, name);
    temp -> name[strlen(temp -> name)-offset] = '\0';
    temp -> next = HEAD;
    HEAD = temp;
}

int getAddr(char * name, int offset) {
    if(!HEAD){
        fprintf(stderr, "Label list Empty\n");
        exit(1);
    }

    name[strlen(name) - offset] = '\0';
    lnode curr = HEAD;
    while(curr) {
        if (!strcmp(curr -> name, name)) {
            return curr -> addr;
        }
        curr = curr -> next;
    }
    fprintf(stderr, "No Labels found\n");
    exit(1);
}

void printLabels(){
    if(!HEAD){
        fprintf(stderr, "List Empty\n");
    }
    lnode curr = HEAD;
    while(curr) {
        fprintf(stdout, "Name: %s\nAddr: %d\n\n",curr -> name, curr -> addr);
        curr = curr -> next;
    }
}