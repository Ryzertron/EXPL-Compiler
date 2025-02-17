/*
----------------Syntax Tree Headers-------------------
This file contains definitions of structures, enums and 
function declarations for Syntax Tree building. Changes since the 
last commit is briefed below
------------------------------------------------------

--------Changelog [Previous Commit dbf1d30]----------
    - No Changes
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

enum tnodeType{     // denotes the class of syntax node. Usage is specified below. (No value means content.value = 0)(No subtype means subtype = none)
        T_CONN,     // Connector with two children. No value or subtype.
        T_ASSG,     // Assginment statement with two children. No subtype or values.
        T_VAR,      // Identifier nodes with zero children. Subtype shows type of variable and content.varname holds pointer to name
        T_CONST,    // Nodes that contains immediate values. content.value holds the value. No children
        T_READ,     // Read statements with a single child (left).
        T_WRITE,    // Write statements with a single child (left).
        T_ADD,      // '+' operator
        T_SUB,      // '-' operator
        T_MUL,      // '*' operator
        T_DIV,      // '/' operator 
        T_LT,       // '<' operator    
        T_GT,       // '>' operator
        T_LE,       // '<=' operator
        T_GE,       // '>=' operator
        T_EQ,       // '==' operator
        T_NE,       // '!=' operator
        T_IF,       // IF node with expr and then nodes as children. No subtype and No value.
        T_THEN,     // THEN nodes with true statements and NULL(false statements for IF-ELSE constructs) as children. No subtype and No value.
        T_BREAK,    // Break statements with no children.
        T_BRKP,     // Breakpoint statement used for debugging. (Breakpoint is not a jump statement.)
        T_CONT,     // Continue statement with no children.
        T_WHILE,    // While loop with expr on left and statements on the right.
        T_REPEAT,   // Repeat...until loop with statement on left and expr on right.
        T_DWHILE    // Do...while loop with statement on left and expr on the right. 
}typedef tnodeType;

enum dType{       // Subtypes identify different types of nodes in a node class 
        none,     // placeholder while using other node types
        D_INT,    // INT identifier type  
        D_CHAR    // Char identifier type
}typedef dType;

struct tnode {      // Syntax Node Structure
    tnodeType type;
    dType dtype;
    data content;
    struct tnode* left;
    struct tnode* right;
}typedef tnode;
typedef tnode* node;

node createSyntaxNode(tnodeType type, dType subtype, data content, node left, node right);
void compatible(node type);

int eval(node root, int* variable);
