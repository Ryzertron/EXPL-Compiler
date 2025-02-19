#pragma once
#include "tree.h"

struct GlobalSymbolTable {
    char* name;
    int dtype;
    int size;
    int binding;
    int rows;
    int cols;
    struct GlobalSymbolTable* next;
}typedef GST;

GST* GST_HEAD = NULL;

GST* GSTLookup(char* name);
int min(int a, int b);
int max(int a, int b);
void GSTInstall(tnode* root, dType type, int offset);
void printGST();