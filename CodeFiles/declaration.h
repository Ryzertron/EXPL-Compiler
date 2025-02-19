#pragma once
#include "tree.h"

struct DNode {
    struct tnode* data;
    struct DNode* next;
}typedef DNode;

extern DNode* DSH;

void pushDecl(tnode* root);

DNode* popDecl();

int DSEmpty();

void CreateGST(dType type);