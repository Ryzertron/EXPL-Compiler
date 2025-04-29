#pragma once
#include "tree.h"
#include "function.h"

//================================Global Symbol Table==============================
struct GST {
    char* name;
    int dtype;
    int size;
    int binding;
    int rows;
    int cols;
    int Flabel;
    struct ParaNode* paraList;
    struct GST* next;
};

extern struct GST* GST_HEAD;

struct GST* GSTLookup(char* name);
int min(int a, int b);
int max(int a, int b);
void GSTInstall(char* name, int dtype, int rows, int cols, int Flabel);
void printGST();

//==================================Local Symbol Table=========================
struct LST {
    char* name;
    int dtype;
    int size;
    int binding;
    struct LST* next;
};

extern struct LST* LST_HEAD;
extern int LSTSize;

struct LST* LSTLookup(char* name);
void LSTInstall(char* name, int dtype);
void clearLST();
int LSTLength();