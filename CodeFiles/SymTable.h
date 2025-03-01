#pragma once
#include "tree.h"
struct tnode;

typedef struct GST {
    char* name;
    int dtype;
    int size;
    int binding;
    int rows;
    int cols;
    struct GST* next;
}GST;

extern GST* GST_HEAD;

GST* GSTLookup(char* name);
int min(int a, int b);
int max(int a, int b);
void GSTInstall(struct tnode* root, int type, int offset);
void printGST();