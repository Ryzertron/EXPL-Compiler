#pragma once
#include "tree.h"

struct ParaNode {
    char * name;
    int type;
    struct ParaNode* next;
};

struct tnode;
extern int SP;
extern struct LST* LST_HEAD;
extern struct GST* currentFunction;
extern FILE* target;


struct ParaNode* NewParaNode(char* name, int type);
struct ParaNode* pushParaNode(struct ParaNode* node, struct ParaNode* head);
void DeclareFunction(char* name, int type, struct ParaNode* PList);
void defineMain(struct tnode* root);
void defineFunctionHead(char* name, int type, struct ParaNode* PList);
void defineFunctionBody(struct tnode* root);
void pushParametersToLST(struct ParaNode* Plist);
int getNewFLabel();
void printParaList(struct ParaNode* head);
struct ParaNode* reversed(struct ParaNode* head);

//=========================Argument Functions=========================

struct ArgNode {
    struct tnode* root;
    struct ArgNode* next;
};

struct ArgNode* pushArgNode(struct tnode* expr, struct ArgNode* head);
void printArgList(struct ArgNode* head);