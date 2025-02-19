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

node createSyntaxNode(tnodeType type, dType dtype, data content, node left, node right, GST* Entry){
    
    node temp = (node)malloc(sizeof(tnode));
    temp -> type = type;
    temp -> dtype = dtype;
    temp -> content = content;
    temp -> right = right;
    temp -> left = left;
    temp -> GSTEntry = Entry;
    //compatible(temp);
    return temp;
}

void compatible(node temp) {

    if (temp -> type >= 6 && temp -> type <= 15) {
        if ((temp -> left -> dtype ==D_INT) && (temp -> left -> dtype == temp -> right -> dtype)) {
            temp -> dtype = D_INT;
            return;
        }
        else {
            fprintf(stderr, "Type mismatch between arithmetic\n");
            exit(1);
        }
    }
    else if (temp -> type == T_ASSG) {
        if((temp -> left -> type == T_ID) && (temp -> left -> dtype == temp -> right -> dtype)){
            temp -> dtype = none;
            return;
        }
        else {
            fprintf(stderr, "Type mismatch between assignment\n");
            exit(1);
        }
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

