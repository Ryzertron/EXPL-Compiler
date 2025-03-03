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
        if (temp -> right -> type == T_ID) {
            compatible(temp -> right);
        }
        if ((temp -> left -> dtype != D_INT) || (temp -> right -> dtype != D_INT)) {
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
        compatible(temp -> left);
        if(temp -> right -> type == T_ID) {
            compatible(temp -> right);
        }
        if(temp -> left -> dtype != temp -> right -> dtype){
            fprintf(stderr, "Type mismatch when assigning to %s\n",temp -> left -> content.varname);
            exit(1);
        }
        temp -> dtype = temp -> left -> dtype;
        return;
    }
    else if (temp -> type == T_DEREF) {
        if (temp -> left -> dtype != D_PTR && temp -> left -> dtype != D_IPTR && temp -> left -> dtype != D_SPTR) {
            fprintf(stderr, "Cannot dereference non-pointer type\n");
            exit(1);
        }
        temp -> dtype = temp -> left -> dtype - 1;
        return;
    }
    else if (temp -> type == T_REF) {
        if (temp -> left -> type != T_ID) {
            fprintf(stderr, "Invalid use of referencing\n");
            exit(1);
        }
        temp -> dtype = temp -> left -> dtype + 1;
        return;
    }
}

