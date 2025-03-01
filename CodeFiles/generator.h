#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#define reg_index int

#define STACKBASE 4096
extern int SP;
extern int BP;

struct loopStackElement {
    int start; // Start Label
    int end; // End Label
    struct loopStackElement * down;
} typedef LSE;

extern LSE * LTOP;

extern int LR; //Track Last Used Register

extern int LL; //Track Last used Label

int getReg(); // Allocate a free register

void freeReg(); // Free the last used register

reg_index codeGen( node root, FILE * target_file); //Code Generator function

void write(reg_index reg_number, FILE * target_file); // Macro for calling write system call

void read(reg_index reg, FILE * target); // Macro for calling read system call

int getAddress(char* varname); // Macro for fetching address from variable name (provisional)

void initxsm(FILE * target); // Initialising the .xsm file with headers and instruction to set SP and BP

void endxsm(FILE * target); // macro for calling exit system call

int getLabel(); // Create and return a new label

void LSPush(int start, int end); // Push a label set into the Loopstack 

void LSPop(); // Pop the top element from the loop stack

int getLSStart(); // get start label of the top loop

int getLSEnd(); // get end label of the top loop




