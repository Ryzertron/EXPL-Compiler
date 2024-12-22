#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#define reg_index int

extern int LR; //Track Last Used Register

int getReg(); // Allocate a free register

void freeReg(); // Free the last used register

reg_index codeGen( node root, FILE * target_file) ;

void write(reg_index reg_number, FILE * target_file);