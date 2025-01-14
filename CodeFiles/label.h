#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CODE_START 2056
struct LinkedList {
    int addr;
    char * name;
    struct LinkedList* next;
}typedef LinkedList;
typedef LinkedList* lnode;
extern lnode HEAD;
extern int LINE;
int getAddr(char * name, int offset);
void newLabel(int line, char * name, int offset);
void printLabels();
void translate();
