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
        T_IO,       // Node for IO statement. Subtype determines input or output.
        T_ASSG,     // Assginment statement with two children. No subtype or values.
        T_ARITH,    // Arithmetic ops with two children. Subtype defines the operation.
        T_REL,      // Relational ops with two children. Subtype defines the operation.
        T_VAR,      // Identifier nodes with zero children. Subtype shows type of variable and content.varname holds pointer to name
        T_CONST,    // Nodes that contains immediate values. content.value holds the value. No children
        T_JUMP,     // Jump statements like break, continue etc. Subtype identifies the the statement. No value. No children
        T_CTRL,     // Control statements like IF and IF-ELSE constructs
        T_LOOP,     // Loop nodes. Subtype identify which type of loop.
}typedef tnodeType;

enum subType{       // Subtypes identify different types of nodes in a node class 
        none,       // placeholder while using other node types
        S_READ,     // Read statements with a single child (left).
        S_WRITE,    // Write statements with a single child (left).
        S_INT,      // Identifier class - 'int'
        S_FLOAT,    // Identifier class - 'float'
        S_CHAR,     // Identifier class - 'char'
        S_ADD,      // '+' operator
        S_SUB,      // '-' operator
        S_MUL,      // '*' operator
        S_DIV,      // '/' operator 
        S_LT,       // '<' operator    
        S_GT,       // '>' operator
        S_LE,       // '<=' operator
        S_GE,       // '>=' operator
        S_EQ,       // '==' operator
        S_NE,       // '!=' operator
        S_IF,       // IF node with expr and then nodes as children. No subtype and No value.
        S_THEN,     // THEN nodes with true statements and NULL(false statements for IF-ELSE constructs) as children. No subtype and No value.
        S_BREAK,    // Break statements with no children.
        S_BRKP,     // Breakpoint statement used for debugging. (Breakpoint is not a jump statement.)
        S_CONT,     // Continue statement with no children.
        S_WHILE,    // While loop with expr on left and statements on the right.
        S_REPEAT,   // Repeat...until loop with statement on left and expr on right.
        S_DWHILE    // Do...while loop with statement on left and expr on the right. 
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
