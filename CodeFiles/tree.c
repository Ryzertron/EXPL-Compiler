/*
---------------------Syntax Tree Functions---------------------------
This file contains the functions that concerns the syntax tree.
As we move up the stages the functions which needs excessive modification or 
refactoring will be reimplemented. Changes since last commit is briefed below.
---------------------------------------------------------------------

-----------------Changelog [Previous Commit 0e9da633]----------------
    - Generalised existing functions for node creation of any type.
    - eval function is commented for now. Might use it for debugging in future.
    - Removed the old function as they are redundant.
---------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

node createSyntaxNode(tnodeType type, subType subtype, data content, node left, node right){
    node temp = (node)malloc(sizeof(tnode));
    temp -> type = type;
    temp -> subtype = subtype;
    temp -> content = content;
    temp -> right = right;
    temp -> left = left;
    return temp;
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

