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
#include "SymTable.h"
#include "tree.h"
#include "function.h"
int SP = STACKBASE; // Stack Pointer
int LR = -1;        // Track Registers
int LL = -1;        // Track Labels for control flow
LSE *LTOP = NULL;
int getReg()
{
    if (LR == 19)
    {
        printf("Error: No more registers available\n");
        exit(1);
    }
    LR++;
    return LR;
}

void freeReg()
{
    if (LR == -1)
    {
        printf("Error : All registers already free\n");
        return;
    }
    LR--;
}

reg_index getAddress(node root, FILE *tsrget)
{
    if(root -> type != T_ID){
        fprintf(stderr, "Internal Error: getAddress called on non-ID node\n");
        exit(1);
    }
    reg_index R, lreg, rreg;
    // Local Variable
    if (root->LSTEntry)
    {
        R = getReg();
        fprintf(target, "MOV R%d, BP\n", R);
        fprintf(target, "ADD R%d, %d\n", R, root->LSTEntry->binding);
        return R;
    }
    // Global Variable
    lreg = codeGen(root->left, target);
    rreg = codeGen(root->right, target);
    if (root->GSTEntry == NULL)
    {
        fprintf(stderr, "Internal Error: %s is not declared\n", root->content.varname);
        exit(1);
    }

    if (lreg == -1 && rreg == -1)
    {
        R = getReg();
        fprintf(target, "MOV R%d, %d\n", R, root->GSTEntry->binding);
        return R;
    }
    else if (rreg == -1)
    {
        fprintf(target, "ADD R%d, %d\n", lreg, root->GSTEntry->binding);
        return lreg;
    }
    else
    {
        fprintf(target, "MUL R%d, %d\n", lreg, root->GSTEntry->cols);
        fprintf(target, "ADD R%d, R%d\n", lreg, rreg);
        freeReg();
        fprintf(target, "ADD R%d, %d\n", lreg, root->GSTEntry->binding);
        return lreg;
    }
}

reg_index codeGen(node root, FILE *target)
{

    if (!root)
        return -1;  // Below the leaf
    reg_index lreg; // Walk down the tree
    reg_index rreg;
    reg_index R;
    int Slabel;
    int Elabel;
    switch (root->type)
    {
        case T_ID:
            lreg = getAddress(root, target);
            fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
            return lreg;
            break;

        case T_CONST:
            switch (root->dtype)
            {
            case D_INT:
                R = getReg();
                fprintf(target, "MOV R%d, %d\n", R, root->content.value);
                return R;
                break;

            case D_STR:
                R = getReg();
                fprintf(target, "MOV R%d, \"%s\"\n", R, root->content.string);
                return R;
                break;
            }
            break;

        case T_READ:
            lreg = getAddress(root->left, target);
            read(lreg, target);
            freeReg();
            return -1;
            break;

        case T_WRITE:
            lreg = codeGen(root->left, target);
            write(lreg, target);
            freeReg();
            return -1;
            break;

        case T_CONN:
            lreg = codeGen(root->left, target);
            rreg = codeGen(root->right, target);
            if (lreg != -1)
                freeReg();
            if (rreg != -1)
                freeReg();
            return -1;
            break;

        case T_ADD:
            lreg = codeGen(root->left, target);
            rreg = codeGen(root->right, target);
            fprintf(target, "ADD R%d, R%d\n", lreg, rreg);
            freeReg();
            return lreg;
            break;

        case T_SUB:
            lreg = codeGen(root->left, target);
            rreg = codeGen(root->right, target);
            fprintf(target, "SUB R%d, R%d\n", lreg, rreg);
            freeReg();
            return lreg;
            break;

        case T_MUL:
        
            lreg = codeGen(root->left, target);
            
            rreg = codeGen(root->right, target);
            
            fprintf(target, "MUL R%d, R%d\n", lreg, rreg);
            freeReg();
            return lreg;
            break;

        case T_DIV:
            lreg = codeGen(root->left, target);
            rreg = codeGen(root->right, target);
            fprintf(target, "DIV R%d, R%d\n", lreg, rreg);
            freeReg();
            return lreg;
            break;

        case T_MOD:
            lreg = codeGen(root->left, target);
            rreg = codeGen(root->right, target);
            R = getReg();
            fprintf(target, "MOV R%d, R%d\n", R, lreg);    // Save lreg value
            fprintf(target, "DIV R%d, R%d\n", lreg, rreg); // lreg = lreg / rreg
            fprintf(target, "MUL R%d, R%d\n", lreg, rreg); // lreg = (lreg / rreg) * rreg
            fprintf(target, "SUB R%d, R%d\n", R, lreg);    // R = R - lreg
            fprintf(target, "MOV R%d, R%d\n", lreg, R);    // lreg = R
            freeReg();                                     // Free R
            freeReg();                                     // Free rreg
            return lreg;
            break;

        case T_LE:
            lreg = codeGen(root->left, target);
            rreg = codeGen(root->right, target);
            fprintf(target, "LE R%d, R%d\n", lreg, rreg);
            freeReg();
            return lreg;
            break;

        case T_GE:
            lreg = codeGen(root->left, target);
            rreg = codeGen(root->right, target);
            fprintf(target, "GE R%d, R%d\n", lreg, rreg);
            freeReg();
            return lreg;
            break;

        case T_LT:
            lreg = codeGen(root->left, target);
            rreg = codeGen(root->right, target);
            fprintf(target, "LT R%d, R%d\n", lreg, rreg);
            freeReg();
            return lreg;
            break;

        case T_GT:
            lreg = codeGen(root->left, target);
            rreg = codeGen(root->right, target);
            fprintf(target, "GT R%d, R%d\n", lreg, rreg);
            freeReg();
            return lreg;
            break;

        case T_NE:
            lreg = codeGen(root->left, target);
            rreg = codeGen(root->right, target);
            fprintf(target, "NE R%d, R%d\n", lreg, rreg);
            freeReg();
            return lreg;
            break;

        case T_EQ:
            lreg = codeGen(root->left, target);
            rreg = codeGen(root->right, target);
            fprintf(target, "EQ R%d, R%d\n", lreg, rreg);
            freeReg();
            return lreg;
            break;

        case T_ASSG:
        
            rreg = codeGen(root->right, target);
            
            if (root->left->type == T_DEREF)
            {
                lreg = getAddress(root-> left -> left, target);
                fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
            }
            else {
                lreg = getAddress(root->left, target);
            }
            fprintf(target, "MOV [R%d], R%d\n", lreg, rreg);
            freeReg();
            freeReg();
            return -1;
            break;

        case T_THEN:
            return -1;
            break;

        case T_IF:
        
            lreg = codeGen(root->left, target);
            Slabel = getLabel();
            fprintf(target, "JZ R%d, L%d\n", lreg, Slabel);
            freeReg();
            codeGen(root->right->left, target);
            

            if (root->right->right)
            {
                Elabel = getLabel();
                fprintf(target, "JMP L%d\n", Elabel);
                fprintf(target, "L%d:\n", Slabel);
                codeGen(root->right->right, target);
                
                fprintf(target, "L%d:\n", Elabel);
            }
            else
            {
                fprintf(target, "L%d:\n", Slabel);
            }
            
            return -1;
            break;

        case T_WHILE:
            Slabel = getLabel();
            Elabel = getLabel();
            LSPush(Slabel, Elabel);
            fprintf(target, "L%d:\n", Slabel);
            lreg = codeGen(root->left, target);
            fprintf(target, "JZ R%d, L%d\n", lreg, Elabel);
            codeGen(root->right, target);
            fprintf(target, "JMP L%d\n", Slabel);
            fprintf(target, "L%d:\n", Elabel);
            freeReg();
            LSPop();
            return -1;
            break;

        case T_DWHILE:
            Slabel = getLabel();
            Elabel = getLabel();
            LSPush(Slabel, Elabel);
            fprintf(target, "L%d:\n", Slabel);
            codeGen(root->left, target);
            rreg = codeGen(root->right, target);
            fprintf(target, "JNZ R%d, L%d\n", rreg, Slabel);
            fprintf(target, "L%d:\n", Elabel);
            freeReg();
            LSPop();
            return -1;
            break;

        case T_REPEAT:
            Slabel = getLabel();
            Elabel = getLabel();
            LSPush(Slabel, Elabel);
            fprintf(target, "L%d:\n", Slabel);
            codeGen(root->left, target);
            rreg = codeGen(root->right, target);
            fprintf(target, "JZ R%d, L%d\n", rreg, Slabel);
            fprintf(target, "L%d:\n", Elabel);
            freeReg();
            LSPop();
            return -1;
            break;

        case T_BREAK:
            Elabel = getLSEnd();
            fprintf(target, "JMP L%d\n", Elabel);
            return -1;
            break;

        case T_CONT:
            Slabel = getLSStart();
            fprintf(target, "JMP L%d\n", Slabel);
            return -1;
            break;

        case T_BRKP:
            fprintf(target, "BRKP\n");
            return -1;
            break;

        case T_DEREF:
            lreg = getAddress(root->left, target);
            fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
            fprintf(target, "MOV R%d, [R%d]\n", lreg, lreg);
            return lreg;
            break;

        case T_REF:
            R = getReg();
            fprintf(target, "MOV R%d, %d\n", R, getAddress(root->left, target));
            return R;

        case T_FUNC:
            if (root->GSTEntry == NULL)
            {
                fprintf(stderr, "Internal Error: %s is not declared\n", root->left->content.varname);
                exit(1);
            }
            for(int i = 0; i <= LR; i++)
             {
                fprintf(target, "PUSH R%d\n", i);
             }
             //Push args in reverse order
            struct ArgNode *arg = root->argList;
            while(arg) {
                lreg = codeGen(arg -> root, target);
                fprintf(target, "PUSH R%d\n", lreg);
                freeReg();
                arg = arg -> next;
        
            }
            
            //Push space for return value
            fprintf(target, "PUSH R0\n");
            //CALL function Label
            fprintf(target, "CALL F%d\n", root->GSTEntry->Flabel);
            //Pop return value
            R = getReg();
            fprintf(target, "POP R%d\n", R);
            //Pop args
            arg = root->argList;
            
            while(arg) {
                fprintf(target, "POP R19\n");
                //Free arg nodes
                struct ArgNode *next = arg -> next;
                free(arg);
                arg = next;
            }
            
            for(int i = LR-1; i >= 0; i--) {
                fprintf(target, "POP R%d\n", i);
            }
            
            return R;
            break;

        case T_RET:
            if(currentFunction -> Flabel == 0) {
                fprintf(target, "INT 10\n");
                return -1;
            }
            lreg = codeGen(root->left, target);
            R = getReg();
            fprintf(target, "ADD SP, %d\n", (0-LSTSize));
            fprintf(target, "MOV R%d, BP\n", R);
            fprintf(target, "ADD R%d, %d\n", R , -2);
            fprintf(target, "MOV [R%d], R%d\n", R, lreg);
            fprintf(target, "POP BP\n");
            fprintf(target, "RET\n");
            freeReg();
            freeReg();
            return -1;
            break;    
        default:
            printf("Error : Invalid Syntax Node type\n");
            exit(1);
            break;
    }
    return -1;
}



void write(reg_index reg, FILE *target)
{

    for (int i = 0; i <= LR; i++)
    { // PUSH registers in use
        fprintf(target, "PUSH R%d\n", i);
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

    for (int i = LR; i >= 0; i--)
    { // POP registers that was pushed initially
        fprintf(target, "POP R%d\n", i);
    }
}

void read(reg_index reg, FILE *target)
{ // Macro that generates code to read in a value from

    for (int i = 0; i <= LR; i++)
    { // PUSH registers in use
        fprintf(target, "PUSH R%d\n", i);
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

    for (int i = LR; i >= 0; i--)
    { // POP registers that was pushed initially
        fprintf(target, "POP R%d\n", i);
    }
}

// int getAddress(char* varname){
//     return STACKBASE + *varname - 'a';   // varname - 'a' gives the number of the letter in the alphabhet.
// }

void initxsm(FILE *target)
{
    fprintf(target, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n", 0, 2056, 0, 0, 0, 0, 0, 0);
    fprintf(target, "MOV SP, %d\n", SP);
    fprintf(target, "MOV BP, SP\n");
    fprintf(target, "JMP F0\n");
    printGST();
}

void endxsm(FILE *target)
{
    if (LR != -1)
    {
        printf("Warning: Register Leak!\n");
    }
    fprintf(target, "INT 10\n");
}

int getLabel()
{
    return ++LL;
}

void LSPush(int start, int end)
{
    LSE *temp = (LSE *)malloc(sizeof(LSE));
    temp->start = start;
    temp->end = end;
    temp->down = NULL;
    if (!LTOP)
        LTOP = temp;
    else
    {
        temp->down = LTOP;
        LTOP = temp;
    }
}

void LSPop()
{
    if (LTOP)
    {
        LSE *temp = LTOP;
        LTOP = temp->down;
        free(temp);
    }
    else
    {
        fprintf(stderr, "Error: Loop Stack Empty\n");
        exit(1);
    }
}

int getLSStart()
{
    if (LTOP)
        return LTOP->start;
    fprintf(stderr, "Jump Construct used outside a loop\n");
    exit(1);
}

int getLSEnd()
{
    if (LTOP)
        return LTOP->end;
    fprintf(stderr, "Jump Construct used outside a loop\n");
    exit(1);
}
