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

node createSyntaxNode(tnodeType type, subType subtype, data content, node left, node right){
    if(type == T_ARITH || type == T_REL || type == T_ASSG){
        if (!compatible(type,left,right)) {
            fprintf(stderr, "Type Error\n");
            exit(1);
        }
    }
    node temp = (node)malloc(sizeof(tnode));
    temp -> type = type;
    temp -> subtype = subtype;
    temp -> content = content;
    temp -> right = right;
    temp -> left = left;
    return temp;
}

int compatible(tnodeType type, node left, node right) {
    if(type == T_ARITH || T_REL)
        return  (left -> type == T_CONST || left -> type == T_ARITH || left -> subtype == S_INT) && 
                (right -> type == T_CONST || right -> type == T_ARITH || right -> subtype == S_INT);
    if (type == T_ASSG)
        return  (left -> subtype == S_INT) && 
                (right -> type == T_CONST || right -> type == T_ARITH || right -> subtype == S_INT);
}
// int eval(node root){
//     if (root -> op == NULL) return root -> value;
//     else {
//         switch(*(root -> op)){
//             case '+' : return eval(root -> left) + eval(root -> right);
//                 break;
//             case '-' : return eval(root -> left) - eval(root -> right);
//                 break;
//             case '*' : return eval(root -> left) * eval(root -> right);
//                 break;
//             case '/' : return eval(root -> left) / eval(root -> right);
//         }
//     }
// }

