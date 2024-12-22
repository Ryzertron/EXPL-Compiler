/*
----------------Syntax Tree Headers-------------------
This file contains definitions of structures, enums and 
function declarations for Syntax Tree building. Changes since the 
last commit is briefed below
------------------------------------------------------

--------Changelog [Previous Commit 0e9da633]----------
    - defined new enums and a union for generalising node creation
    - deleted old function declarations.
------------------------------------------------------
*/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

union data{         // union is used to hold data for different type of nodes
    char * varname; // varnames for var nodes
    int value;      // value for constants
} typedef data;

enum tnodeType{     // denotes the type of the syntax node
        T_READ,
        T_WRITE,
        T_CONN,
        T_OPER,
        T_VAR,
        T_CONST
}typedef tnodeType;

enum subType{       // denotes the type of variable or operator if the node is type of variable or operator
        none,       // placeholder while using other node types
        VAR_INT,
        VAR_FLOAT,
        VAR_CHAR,
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_ASSG,
}typedef subType;

struct tnode {      // Syntax Node Structure
    tnodeType type;
    subType subtype;
    data content;
    struct tnode* left;
    struct tnode* right;
}typedef tnode;
typedef tnode* node;

node createSyntaxNode(tnodeType type, subType subtype, data content, node left, node right);

// int eval(node root);
