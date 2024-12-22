/*
----------------XSM Code Generator--------------------
This file contains functions that concern the assembly code generation
for xsm simulator. Changes since the last commit is briefed below
------------------------------------------------------
----------Changelog [Previous Commit 0e9da633]--------
    - initxsm() and endxsm() functions to prevent redundant routines
    - Modified codeGen() to handle other type of nodes in the syntax tree.
    - getAddress() function is defined but it will change in the future when
        string variable names are allowed
------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator.h"

int LR = -1;

int getReg(){
    if(LR == 19){
        printf("Error: No more registers available\n");
        exit(1);
    }
    LR++;
    return LR;
}

void freeReg(){
    if (LR == -1){
        printf("Error : All registers already free\n");
        return;
    }
    LR--;
}

reg_index codeGen(node root, FILE* target){

    if(!root) return -1; //Below the leaf
    reg_index lreg = codeGen(root -> left, target);//Walk down the tree
    reg_index rreg = codeGen(root -> right, target);
    reg_index R;
    switch(root -> type){
        case T_VAR :    R = getReg();
                        fprintf(target, "MOV R%d, [%d]\n", R, getAddress(root -> content.varname));
                        return R;
                        break;

        case T_CONST :  R = getReg();
                        fprintf(target, "MOV R%d, %d\n", R, root -> content.value);
                        return R;
                        break;
         
        case T_READ :   fprintf(target, "MOV R%d, %d\n", lreg, getAddress(root -> left -> content.varname));
                        read(lreg,target);
                        freeReg();
                        return -1;
                        break;
        
        case T_WRITE :  write(lreg,target);
                        freeReg();
                        return -1;
                        break;
        
        case T_CONN :   //if(LR != -1) printf(" %d Register Leak found\n",LR);
                        if (lreg != -1) freeReg();
                        if (rreg != -1) freeReg();
                        return -1;
                        break;
        
        case T_OPER :   switch(root -> subtype) // Checking which operator
                        {
                            case OP_ADD :   fprintf(target, "ADD R%d, R%d\n", lreg, rreg);
                                            freeReg();
                                            return lreg;
                                            break;
                            
                            case OP_SUB :   fprintf(target, "SUB R%d, R%d\n", lreg, rreg);
                                            freeReg();
                                            return lreg;
                                            break;

                            case OP_MUL :   fprintf(target, "MUL R%d, R%d\n", lreg, rreg);
                                            freeReg();
                                            return lreg;
                                            break;

                            case OP_DIV :   fprintf(target, "DIV R%d, R%d\n", lreg, rreg);
                                            freeReg();
                                            return lreg;
                                            break;

                            case OP_ASSG :  fprintf(target, "MOV [%d], R%d\n", getAddress(root -> left -> content.varname), rreg);
                                            freeReg();
                                            return lreg;
                                            break;
                            
                            default :       printf("Error : Invalid operator type\n");
                                            exit(1);   

                        }
                        break;
        
        default :       printf("Error : Invalid Syntax Node type\n");
                        exit(1);        
    }                      
}

void write(reg_index reg, FILE* target){

    for(int i = 0; i <= LR; i++){   // PUSH registers in use
        fprintf(target,"PUSH R%d\n",i);
    }

    fprintf(target, "MOV R1,R%d\n", reg);
    fprintf(target, "MOV R0,\"WRITE\"\n"); 
    fprintf(target, "PUSH R0\n"); // push function code
    fprintf(target, "MOV R0, -2\n");
    fprintf(target, "PUSH R0\n"); // push arg 1 (-2 means terminal)
    fprintf(target, "PUSH R1\n"); // push argument word
    fprintf(target, "PUSH R0\n"); // push arg 3
    fprintf(target, "PUSH R0\n"); // push ret space
    fprintf(target, "CALL 0\n");  // call library
    fprintf(target, "POP R0\n");  // pop ret val
    fprintf(target, "POP R1\n");  // pop arg 3
    fprintf(target, "POP R1\n");  // pop arg 2
    fprintf(target, "POP R1\n");  // pop arg 1
    fprintf(target, "POP R1\n");  // pop syscall
    
    
    for (int i = LR; i >= 0; i--){ //POP registers that was pushed initially
        fprintf(target, "POP R%d\n", i);
    }
}

void read(reg_index reg, FILE * target){ // Macro that generates code to read in a value from  

    for(int i = 0; i <= LR; i++){   // PUSH registers in use
        fprintf(target,"PUSH R%d\n",i);
    }

    fprintf(target, "MOV R1,R%d\n", reg);
    fprintf(target, "MOV R0,\"READ\"\n"); 
    fprintf(target, "PUSH R0\n"); // push function code
    fprintf(target, "MOV R0, -1\n");
    fprintf(target, "PUSH R0\n"); // push arg 1 (-1 means terminal)
    fprintf(target, "PUSH R1\n"); // push address
    fprintf(target, "PUSH R0\n"); // push arg 3
    fprintf(target, "PUSH R0\n"); // push ret space
    fprintf(target, "CALL 0\n");  // call library
    fprintf(target, "POP R0\n");  // pop ret val
    fprintf(target, "POP R1\n");  // pop arg 3
    fprintf(target, "POP R1\n");  // pop arg 2
    fprintf(target, "POP R1\n");  // pop arg 1
    fprintf(target, "POP R1\n");  // pop syscall

    for (int i = LR; i >= 0; i--){ //POP registers that was pushed initially
        fprintf(target, "POP R%d\n", i);
    }
}

int getAddress(char* varname){
    return STACKBASE + *varname - 'a';   // varname - 'a' gives the number of the letter in the alphabhet.
}

void initxsm(FILE* target){
    fprintf(target, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",0,2056,0,0,0,0,0,0);
    fprintf(target, "MOV SP, %d\n",SP); // leaving space for 26 variables
    fprintf(target, "MOV BP, %d\n", BP);
}

void endxsm(FILE * target) {
    if(LR != -1)
    {
        printf("Warning: Register Leak!\n");
    }
    fprintf(target, "INT 10\n");
}
