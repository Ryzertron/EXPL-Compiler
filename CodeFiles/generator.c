/*
----------------XSM Code Generator--------------------
This file contains functions that concern the assembly code generation
for xsm simulator. Changes since the last commit is briefed below
------------------------------------------------------
----------Changelog [Previous Commit 1e6ad85]--------
    - implemented generation of if, loop and jump constructs
    - defined new helper functions required for label handling
    - tweaked the recursive calls to improve compile time.
------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator.h"

int LR = -1;    // Track Registers
int LL = -1;    // Track Labels
LSE* LTOP = NULL;
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
    reg_index lreg;//Walk down the tree
    reg_index rreg;
    reg_index R;
    int Slabel;
    int Elabel;
    switch(root -> type){
        case T_VAR :    lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        R = getReg();
                        fprintf(target, "MOV R%d, [%d]\n", R, getAddress(root -> content.varname));
                        return R;
                        break;

        case T_CONST :  lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        R = getReg();
                        fprintf(target, "MOV R%d, %d\n", R, root -> content.value);
                        return R;
                        break;
         
        case T_IO :     lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        switch(root -> subtype) {
                            case S_READ :   fprintf(target, "MOV R%d, %d\n", lreg, getAddress(root -> left -> content.varname));
                                            read(lreg,target);
                                            freeReg();
                                            return -1;
                                            break;
                            
                            case S_WRITE :  write(lreg,target);
                                            freeReg();
                                            return -1;
                                            break;
                            
                            default :       printf("Invalid I/O method\n");
                                            exit(1);
                                            break;
                        }
                        break;
        
        case T_CONN :   lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        int l2;
                        if (lreg != -1) freeReg();
                        if (rreg != -1) freeReg();
                        return -1;
                        break;
        
        case T_ARITH :  lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        switch(root -> subtype) // Checking which operator
                        {
                            case S_ADD :   fprintf(target, "ADD R%d, R%d\n", lreg, rreg);
                                            freeReg();
                                            return lreg;
                                            break;
                            
                            case S_SUB :   fprintf(target, "SUB R%d, R%d\n", lreg, rreg);
                                            freeReg();
                                            return lreg;
                                            break;

                            case S_MUL :   fprintf(target, "MUL R%d, R%d\n", lreg, rreg);
                                            freeReg();
                                            return lreg;
                                            break;

                            case S_DIV :   fprintf(target, "DIV R%d, R%d\n", lreg, rreg);
                                            freeReg();
                                            return lreg;
                                            break;
                            
                            default :       printf("Error : Invalid Arithmetic Node\n");
                                            exit(1);   

                        }
                        break;
        
        case T_REL :    lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        switch(root -> subtype) {
                            case S_LE :     fprintf(target, "LE R%d, R%d\n", lreg, rreg);
                                            freeReg();
                                            return lreg;
                                            break;
                            
                            case S_GE :     fprintf(target, "GE R%d, R%d\n", lreg, rreg);
                                            freeReg();
                                            return lreg;
                                            break;
                            
                            case S_LT :     fprintf(target, "LT R%d, R%d\n", lreg, rreg);
                                            freeReg();
                                            return lreg;
                                            break;
                            
                            case S_GT :     fprintf(target, "GT R%d, R%d\n", lreg, rreg);
                                            freeReg();
                                            return lreg;
                                            break;
                            
                            case S_NE :     fprintf(target, "NE R%d, R%d\n", lreg, rreg);
                                            freeReg();
                                            return lreg;
                                            break;

                            case S_EQ :     fprintf(target, "EQ R%d, R%d\n", lreg, rreg);
                                            freeReg();
                                            return lreg;
                                            break;

                            default :       fprintf(stderr, "Unknown Relational Node in AST\n");
                                            exit(1);
                                            break;
                        }
                        break;
        
        case T_ASSG :   lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        fprintf(target, "MOV [%d], R%d\n", getAddress(root -> left -> content.varname), rreg);
                        freeReg();
                        return lreg;
                        break;
        
        case T_CTRL :   switch(root -> subtype){
                            case S_THEN :   return -1;
                                            break;

                            case S_IF :     lreg = codeGen(root -> left, target);
                                            Slabel = getLabel();
                                            fprintf(target, "JZ R%d, L%d\n", lreg, Slabel);
                                            freeReg();
                                            codeGen(root -> right -> left, target);
                                            
                                            if (root -> right -> right) {
                                                Elabel = getLabel();
                                                fprintf(target, "JMP L%d\n", Elabel);
                                                fprintf(target, "L%d:\n", Slabel);
                                                codeGen(root -> right -> right, target);
                                                fprintf(target, "L%d:\n", Elabel);
                                            }
                                            else {
                                                fprintf(target, "L%d:\n", Slabel);
                                            }
                                            return -1;
                                            break;
                                default :   fprintf(stderr, "Error: Unknown Control Node in AST\n");
                                            exit(1);
                                            break;                                            
                        }   
                        break;
        
        case T_LOOP :   switch(root -> subtype) {
                            case S_WHILE :  Slabel = getLabel();
                                            Elabel = getLabel();
                                            LSPush(Slabel,Elabel);
                                            fprintf(target, "L%d:\n", Slabel);
                                            lreg = codeGen(root -> left, target);
                                            fprintf(target, "JZ R%d, L%d\n", lreg, Elabel);
                                            codeGen(root -> right, target);
                                            fprintf(target, "JMP L%d\n", Slabel);
                                            fprintf(target, "L%d:\n", Elabel);
                                            freeReg();
                                            LSPop();
                                            return -1;
                                            break;

                            case S_DWHILE : Slabel = getLabel();
                                            Elabel = getLabel();
                                            LSPush(Slabel,Elabel);
                                            fprintf(target, "L%d:\n", Slabel);
                                            codeGen(root -> left, target);
                                            rreg = codeGen(root -> right, target);
                                            fprintf(target, "JNZ R%d L%d\n", rreg, Slabel);
                                            fprintf(target, "L%d:\n", Elabel);
                                            freeReg();
                                            LSPop();
                                            return -1;
                                            break;
                            
                            case S_REPEAT : Slabel = getLabel();
                                            Elabel = getLabel();
                                            LSPush(Slabel, Elabel);
                                            fprintf(target, "L%d:\n", Slabel);
                                            codeGen(root -> left, target);
                                            rreg = codeGen(root -> right, target);
                                            fprintf(target, "JZ R%d L%d\n", rreg, Slabel);
                                            fprintf(target, "L%d:\n", Elabel);
                                            freeReg();
                                            LSPop();
                                            return -1;
                                            break;
                            
                            default :       fprintf(stderr, "Unknown Loop Construct\n");
                                            exit(1);
                                            break;


                        }
                        break;
        
        case T_JUMP :   switch(root -> subtype) {
                            case S_BREAK : Elabel = getLSEnd();
                                           fprintf(target, "JMP L%d\n", Elabel);
                                           return -1;
                                           break;
                            
                            case S_CONT :  Slabel = getLSStart();
                                           fprintf(target, "JMP L%d\n", Slabel);
                                           return -1;
                                           break;

                            case S_BRKP :  fprintf(target, "BRKP\n");
                                           return -1; 
                                           break;

                            default :      fprintf(stderr, "Unknown jump construct\n");
                                           exit(1);
                                           break;                   

                        }
                        break;

        default :       printf("Error : Invalid Syntax Node type\n");
                        exit(1);
                        break;        
    }
    return -1;                      
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

int getLabel(){
    return ++LL;
}

void LSPush(int start, int end) {
    LSE* temp = (LSE*) malloc (sizeof(LSE));
    temp -> start = start;
    temp -> end = end;
    temp -> down = NULL;
    if (!LTOP) LTOP = temp;
    else {
        temp -> down = LTOP;
        LTOP = temp;
    }
}

void LSPop() {
    if(LTOP) {
        LSE * temp = LTOP;
        LTOP = temp -> down;
        free(temp);
    }
    else {
        fprintf(stderr, "Error: Loop Stack Empty\n");
        exit(1);
    }
}

int getLSStart() {
    if (LTOP) return LTOP -> start;
    fprintf(stderr, "Jump Construct used outside a loop\n");
    exit(1);
}

int getLSEnd() {
    if (LTOP) return LTOP -> end;
    fprintf(stderr, "Jump Construct used outside a loop\n");
    exit(1);
}