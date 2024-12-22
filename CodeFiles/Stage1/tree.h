#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct tnode {
    int value;
    char* op;
    struct tnode* left;
    struct tnode* right;
}tnode;
typedef tnode* node;

node leafNode(int val);

node opNode(char op,node left, node right);

int eval(node root);

void prefix(node root);

void postfix(node root);
