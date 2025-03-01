/*
---------------------Syntax Tree Functions---------------------------
This file contains the functions that concerns the syntax tree.
As we move up the stages the functions which needs excessive modification or 
refactoring will be reimplemented. Changes since last commit is briefed below.
---------------------------------------------------------------------

-----------------Changelog [Previous Commit 267e5ca]----------------
    - Added Semantic Analysis function | Not tested
---------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

node createTempVarNode(tnodeType type, dType dtype, data content, node left, node right) {
    node temp = (node)malloc(sizeof(tnode));
    temp -> type = type;
    temp -> dtype = dtype;
    temp -> content = content;
    temp -> right = right;
    temp -> left = left;
    temp -> GSTEntry = NULL;
    return temp;
}

node createSyntaxNode(tnodeType type, dType dtype, data content, node left, node right, struct GST* Entry){
    
    node temp = (node)malloc(sizeof(tnode));
    temp -> type = type;
    temp -> dtype = dtype;
    temp -> content = content;
    temp -> right = right;
    temp -> left = left;
    temp -> GSTEntry = Entry;
    compatible(temp);
    return temp;
}

void compatible(node temp) {
    if (temp -> type == T_ID) {
        if (!temp -> GSTEntry) {
            fprintf(stderr, "Undeclared identifier %s\n", temp -> content.varname);
            exit(1);
        }
        if (temp -> left ) {
            if (temp -> left -> dtype != D_INT) {
                fprintf(stderr, "Invlalid subscript type for %s\n", temp -> content.varname);
                exit(1);
            }
            else if (temp -> left -> type == T_CONST && temp -> GSTEntry -> rows <= temp -> left -> content.value) {
                fprintf(stderr, "Array index out of bounds for %s\n", temp -> content.varname);
                exit(1);
            }
        }
        if (temp -> right) {
            if (temp -> right -> dtype != D_INT) {
                fprintf(stderr, "Invlalid subscript type for %s\n", temp -> content.varname);
                exit(1);
            }
            else if (temp -> right -> type == T_CONST && temp -> GSTEntry -> cols <= temp -> right -> content.value) {
                fprintf(stderr, "Array index out of bounds for %s\n", temp -> content.varname);
                exit(1);
            }
        }
    }
    else if (temp -> type >= T_ADD && temp -> type <= T_NE) {
        if (temp -> left -> type == T_ID) {
            compatible(temp -> left);
        }
        else if (temp -> right -> type == T_ID) {
            compatible(temp -> right);
        }
        else if ((temp -> left -> dtype != D_INT) || (temp -> right -> dtype != D_INT)) {
            fprintf(stderr, "Type mismatch between arithmetic\n");
            exit(1);
        }
        else {
            temp -> dtype = D_INT;
            return;
        }
    }
    else if (temp -> type == T_ASSG) {

        if(temp -> left -> type != T_ID) {
            fprintf(stderr, "expected an identifier on the left of assignment\n");
            exit(1);
        }
        else if(temp -> left -> dtype != temp -> right -> dtype){
            fprintf(stderr, "Type mismatch when assigning to %s\n",temp -> left -> content.varname);
            exit(1);
        }
        if(temp -> right -> type == T_ID) {
            compatible(temp -> right);
        }
        compatible(temp -> left);
        temp -> dtype = temp -> left -> dtype;
        return;
    }
}

// int Bflag = 0, Cflag = 0;
// int eval(node root, int* variable){
//     if(!root) return -1;
//     switch(root -> type) {
//         case T_ID: {
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             int index = *(root -> content.varname) - 'a';
//             return variable[index];
//             break;
//         }

//         case T_CONST : {
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             return root -> content.value;
//             break;
//         }

//         case T_ADD:{
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             return lreg + rreg;
//             break;
//         }
//         case T_SUB: {
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             return lreg - rreg;
//             break;
//         }
//         case T_MUL:{
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             return lreg * rreg;
//             break;
//         }
//         case T_DIV:{
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             return lreg/ rreg;
//             break;
//         }
//         case T_ASSG:{
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             variable[*(root -> left ->content.varname)-'a'] = rreg;
//             return -1;
//             break;
//         }

//         case T_LT:{
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             return lreg < rreg;
//             break;
//         }
//         case T_GT:{
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             return lreg > rreg;
//             break;
//         }
//         case T_LE: {
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             return lreg <= rreg;
//             break;
//         }
//         case T_GE : {
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             return lreg >= rreg;
//             break;
//         }
//         case T_EQ : {
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             return lreg == rreg;
//             break;
//         }
//         case T_NE: {
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             return lreg != rreg;
//             break;
//         }
//         case T_READ: {
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             int index = *(root -> left -> content.varname) - 'a';
//             scanf("%d",&variable[index]);
//             return -1;
//             break;
//         }
//         case T_WRITE : {
//             int lreg = eval(root -> left, variable);
//             int rreg = eval(root -> right, variable);
//             printf("%d",lreg);
//             return -1;
//             break;
//         }
//         case T_WHILE:{
//             int lreg = eval(root -> left, variable);
//             int rreg;
//             while(lreg){
//                 rreg = eval(root -> right, variable);
//                 if (Bflag == 1){
//                     Bflag = 0;
//                     break;
//                 }
                
//                 lreg = eval(root -> left, variable);
//                 if (Cflag == 1){
//                     Cflag = 0;
//                     continue;
//                 }
//             }
//             return -1;
//             break;
//         }
//         case T_DWHILE : {
//             int lreg,rreg;
//             do{
//                 lreg = eval(root -> left, variable);
//                 if (Bflag == 1){
//                     Bflag = 0;
//                     break;
//                 }
                
//                 rreg = eval(root -> right, variable);
//                 if (Cflag == 1){
//                     Cflag = 0;
//                     continue;
//                 }
//             }while(rreg);
//             return -1;
//             break;
//         }
//         case T_REPEAT: {
//             int lreg, rreg;
//             do{
//                 lreg = eval(root -> left, variable);
//                 if (Bflag == 1){
//                     Bflag = 0;
//                     break;
//                 }
//                 rreg = eval(root -> right, variable);
//                 if (Cflag == 1){
//                     Cflag = 0;
//                     continue;
//                 }

//             }while(!rreg);
//             return -1;
//             break;
//         }
//         case T_BREAK : {
//             Bflag = 1;
//             return -1;
//             break;
//         }
//         case T_CONT : {
//             Cflag = 1;
//             return -1;
//             break;
//         }

//         case T_IF: {
//             int lreg = eval(root -> left, variable);
//             if (lreg){
//                 eval(root -> right -> left, variable);

//             }else {
//                     eval(root -> right -> right, variable);
//             }
//             return -1;

//             break;
//         }
//         case T_CONN :{
//             eval(root -> left, variable);
//             if (Bflag == 1){
//                 return -1;
//             }
//             if (Cflag == 1){
//                 return -1;
//             }
//             eval(root -> right, variable);
//             return -1;
//         }
//         default: {
//             return -1;
//             break;
//         }

//     }
// }

