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
#include "SymTable.h"

node createSyntaxNode(tnodeType type, dType dtype, data content, node left, node right){
    
    //Debugging Purposes
    
    if(type == T_FUNC) {
        fprintf(stderr, "Internal Error: createSyntaxNode called with T_FUNC type\n");
        exit(1);
    }
    node temp = (node)malloc(sizeof(tnode));
    temp -> type = type;
    temp -> dtype = dtype;
    temp -> content = content;
    temp -> right = right;
    temp -> left = left;
    temp -> argList = NULL;
    
    compatible(temp);
    
    return temp;
}

//Specialized function to create function call nodes
node createFuncCallNode(char* name, struct ArgNode* arglist) {
    node temp = (node)malloc(sizeof(tnode));
    temp -> type = T_FUNC;
    temp -> dtype = none;
    temp -> content.varname = (char*)malloc(strlen(name));
    strcpy(temp -> content.varname, name);
    temp -> left = NULL;
    temp -> right = NULL;
    temp -> argList = arglist;
    
    compatible(temp);
    
    return temp;
}


//Semantic Checking function
void compatible(node temp) {
    if (temp -> type == T_ID) {
        if(temp -> content.varname == NULL) {
            fprintf(stderr, "Internal Error: NULL varname in ID node\n");
            exit(1);
        }
        struct LST* lstentry = LSTLookup(temp -> content.varname);

        if(!lstentry) {
            //Check Global Symbol Table
            struct GST* gstentry = GSTLookup(temp -> content.varname);
            if(!gstentry) {
                fprintf(stderr, "Error: Undeclared identifier %s\n", temp -> content.varname);
                exit(1);
            }
            //Global Symbol
            else {
                temp -> GSTEntry = gstentry;
                temp -> dtype = gstentry -> dtype;
                temp -> LSTEntry = NULL;
                //Checking for array
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
        }
        //Local Symbol
        else {
            temp -> LSTEntry = lstentry;
            temp -> GSTEntry = NULL;
            temp -> dtype = lstentry -> dtype;
            if(temp -> left || temp -> right) {
                fprintf(stderr, "Invalid subscripting of identifier %s\n", temp -> content.varname);
                exit(1);
            }
        }    
    }
    else if (temp -> type >= T_ADD && temp -> type <= T_NE) {
        if ((temp -> left -> dtype != D_INT) || (temp -> right -> dtype != D_INT)) {
            fprintf(stderr, "Type mismatch for arithmetic\n");
            exit(1);
        }
        else {
            temp -> dtype = D_INT;
            return;
        }
    }
    else if (temp -> type == T_ASSG) {
        if(temp -> left -> type != T_ID && temp -> left -> type != T_DEREF) {
            fprintf(stderr, "expected a valid identifier on the left of assignment\n");
            exit(1);
        }
        if(temp -> left -> dtype != temp -> right -> dtype){
            printf("type of %s --- %d\n", temp -> left -> content.varname, temp -> left -> dtype);
            fprintf(stderr, "Type mismatch when assigning to %s\n",temp -> left -> content.varname);
            exit(1);
        }
        temp -> dtype = temp -> left -> dtype;
        return;
    }
    else if (temp -> type == T_DEREF) {
        if(temp -> left == NULL) {
            fprintf(stderr, "Internal Error: NULL left child in dereference node\n");
            exit(1);
        }
        if (temp -> left -> dtype != D_IPTR && temp -> left -> dtype != D_SPTR) {
            fprintf(stderr, "Cannot dereference non-pointer type\n");
            exit(1);
        }
        temp -> dtype = temp -> left -> dtype - 1;
        return;
    }
    else if (temp -> type == T_REF) {
        if(temp -> left == NULL) {
            fprintf(stderr, "Internal Error: NULL left child in reference node\n");
            exit(1);
        }
        if (temp -> left -> type != T_ID || temp -> left -> dtype % 2 == 0) {   //do not allow double pointer and non variable referencing
            fprintf(stderr, "Invalid use of referencing\n");
            exit(1);
        }
        temp -> dtype = temp -> left -> dtype + 1;
        return;
    }

    else if (temp -> type == T_FUNC) {
        
        struct GST* gstentry = GSTLookup(temp -> content.varname);
        if (!gstentry || gstentry -> Flabel == -1) {
            fprintf(stderr, "Function %s not declared\n", temp -> left -> content.varname);
            exit(1);
        }
        
        temp -> GSTEntry = gstentry;
        temp -> LSTEntry = NULL;
        temp -> dtype = gstentry -> dtype;

        //Check for function arguments types
        struct ParaNode* paraProto = gstentry -> paraList;
        struct ArgNode* arg = temp -> argList;

        while(paraProto || arg) {
            
            if(!paraProto || !arg || paraProto -> type != arg -> root -> dtype) {
                fprintf(stderr, "Error: Function \'%s()\' called with incompatible arguments\n", temp -> content.varname);
                exit(1);
            }
            paraProto = paraProto -> next;
            arg = arg -> next;
        }
        
    }
    
}


