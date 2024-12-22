
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#define reg_index int

#define STACKBASE 4096
#define SP 5096
#define BP 5097

extern int LR; //Track Last Used Register

int getReg(); // Allocate a free register

void freeReg(); // Free the last used register

reg_index codeGen( node root, FILE * target_file); //Code Generator function

void write(reg_index reg_number, FILE * target_file); // Macro for calling write system call

void read(reg_index reg, FILE * target); // Macro for calling read system call

int getAddress(char* varname); // Macro for fetching address from variable name (provisional)

void initxsm(FILE * target); // Initialising the .xsm file with headers and instruction to set SP and BP

void endxsm(FILE * target); // macro for calling exit system call

