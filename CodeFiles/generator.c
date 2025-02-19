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
        case T_ID :     lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);

                        if(root -> GSTEntry == NULL) {
                            fprintf(stderr,"%s is not declared\n", root -> content.varname);
                            exit(1);
                        }

                        if (lreg == -1 && rreg == -1) {
                            R = getReg();
                            fprintf(target, "MOV R%d, %d\n", R, root -> GSTEntry -> binding);
                            return R;
                        }
                        else if (rreg == -1) {
                            if (root -> left -> type == T_ID) {
                                fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
                            }

                            fprintf(target, "ADD R%d, %d\n",lreg, root -> GSTEntry -> binding);
                            return lreg;
                        }
                        else {
                            if(root -> left -> type == T_ID) {
                                fprintf("MOV R%d, [R%d]\n", lreg, lreg);
                            }
                            fprintf(target, "MUL R%d, %d\n", lreg, root -> GSTEntry -> cols);

                            if (root -> right -> type == T_ID) {
                                fprintf("MOV R%d, [R%d]\n", rreg, rreg);
                            }
                            fprintf(target,"ADD R%d, R%d\n", lreg, rreg);
                            freeReg();
                            fprintf(target, "ADD R%d, %d\n", lreg, root -> GSTEntry -> binding);

                        }
                        break;

        case T_CONST :  lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        switch(root -> dtype) {
                            case D_INT :    R = getReg();
                                            fprintf(target, "MOV R%d, %d\n", R, root -> content.value);
                                            return R;
                                            break;

                            case D_STR :    R = getReg();
                                            fprintf(target, "MOV R%d, \"%s\"\n", R, root -> content.string);
                                            return R;
                                            break;
                        }
                        break;
         
        
        
        case T_READ :   lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        read(lreg,target);
                        freeReg();
                        return -1;
                        break;
        
        case T_WRITE :  lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        if(root -> left -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n",lreg, lreg);
                        }
                        write(lreg,target);
                        freeReg();
                        return -1;
                        break;
                            
                            
                        
        
        case T_CONN :   lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        if (lreg != -1) freeReg();
                        if (rreg != -1) freeReg();
                        return -1;
                        break;
        
                        
   
        case T_ADD :    lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        if(root -> left -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
                        }
                        if(root -> right -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", rreg, rreg);
                        }
                        fprintf(target, "ADD R%d, R%d\n", lreg, rreg);
                        freeReg();
                        return lreg;
                        break;
        
        case T_SUB :    lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        if(root -> left -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
                        }
                        if(root -> right -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", rreg, rreg);
                        }
                        fprintf(target, "SUB R%d, R%d\n", lreg, rreg);
                        freeReg();
                        return lreg;
                        break;

        case T_MUL :    lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        if(root -> left -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
                        }
                        if(root -> right -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", rreg, rreg);
                        }
                        fprintf(target, "MUL R%d, R%d\n", lreg, rreg);
                        freeReg();
                        return lreg;
                        break;

        case T_DIV :    lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        if(root -> left -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
                        }
                        if(root -> right -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", rreg, rreg);
                        }
                        fprintf(target, "DIV R%d, R%d\n", lreg, rreg);
                        freeReg();
                        return lreg;
                        break;
        
        
                        
   
        case T_LE :     lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        if(root -> left -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
                        }
                        if(root -> right -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", rreg, rreg);
                        }
                        fprintf(target, "LE R%d, R%d\n", lreg, rreg);
                        freeReg();
                        return lreg;
                        break;
        
        case T_GE :     lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        if(root -> left -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
                        }
                        if(root -> right -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", rreg, rreg);
                        }
                        fprintf(target, "GE R%d, R%d\n", lreg, rreg);
                        freeReg();
                        return lreg;
                        break;
        
        case T_LT :     lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        if(root -> left -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
                        }
                        if(root -> right -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", rreg, rreg);
                        }
                        fprintf(target, "LT R%d, R%d\n", lreg, rreg);
                        freeReg();
                        return lreg;
                        break;
        
        case T_GT :     lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        if(root -> left -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
                        }
                        if(root -> right -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", rreg, rreg);
                        }
                        fprintf(target, "GT R%d, R%d\n", lreg, rreg);
                        freeReg();
                        return lreg;
                        break;
        
        case T_NE :     lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        if(root -> left -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
                        }
                        if(root -> right -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", rreg, rreg);
                        }
                        fprintf(target, "NE R%d, R%d\n", lreg, rreg);
                        freeReg();
                        return lreg;
                        break;

        case T_EQ :     lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        if(root -> left -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
                        }
                        if(root -> right -> type == T_ID) {
                            fprintf(target, "MOV R%d, [R%d]\n", rreg, rreg);
                        }
                        fprintf(target, "EQ R%d, R%d\n", lreg, rreg);
                        freeReg();
                        return lreg;
                        break;
        
        case T_ASSG :   lreg = codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        if(root -> right -> type == T_ID) {
                            fprintf(target, "MOV R%d, R%d\n",rreg, rreg);
                        }
                        fprintf(target, "MOV [R%d], R%d\n", lreg, rreg);
                        freeReg();
                        freeReg();
                        return -1;
                        break;
        
        case T_THEN :   return -1;
                        break;

        case T_IF :     lreg = codeGen(root -> left, target);
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
          
        
        case T_WHILE :  Slabel = getLabel();
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

        case T_DWHILE : Slabel = getLabel();
                        Elabel = getLabel();
                        LSPush(Slabel,Elabel);
                        fprintf(target, "L%d:\n", Slabel);
                        codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        fprintf(target, "JNZ R%d, L%d\n", rreg, Slabel);
                        fprintf(target, "L%d:\n", Elabel);
                        freeReg();
                        LSPop();
                        return -1;
                        break;
        
        case T_REPEAT : Slabel = getLabel();
                        Elabel = getLabel();
                        LSPush(Slabel, Elabel);
                        fprintf(target, "L%d:\n", Slabel);
                        codeGen(root -> left, target);
                        rreg = codeGen(root -> right, target);
                        fprintf(target, "JZ R%d, L%d\n", rreg, Slabel);
                        fprintf(target, "L%d:\n", Elabel);
                        freeReg();
                        LSPop();
                        return -1;
                        break;
        
        
        
        case T_BREAK :  Elabel = getLSEnd();
                        fprintf(target, "JMP L%d\n", Elabel);
                        return -1;
                        break;
        
        case T_CONT :   Slabel = getLSStart();
                        fprintf(target, "JMP L%d\n", Slabel);
                        return -1;
                        break;

        case T_BRKP :  fprintf(target, "BRKP\n");
                        return -1; 
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

// int getAddress(char* varname){
//     return STACKBASE + *varname - 'a';   // varname - 'a' gives the number of the letter in the alphabhet.
// }

void initxsm(FILE* target){
    fprintf(target, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",0,2056,0,0,0,0,0,0);
    fprintf(target, "MOV SP, %d\n",SP); // leaving space for 26 variables
    fprintf(target, "MOV BP, %d\n", BP);
    fprintf(target,"BRKP\n");
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

