#pragma once
#include "tree.h"


//==================================Global Declaration Stack=========================

struct GDNode {
    char * name;
    int  type;
    int rows;
    int cols;
    struct GDNode* next;
};

struct GDNode* NewGDNode(char* name, int type, int rows, int cols);
void pushGDecl(struct GDNode* node);
struct GDNode* popGDecl();
void UpdateGST(int type);

//==================================Local Declaration Stack=========================

struct LDNode {
    char * name;
    int type;
    struct LDNode* next;
};

struct LDNode* NewLDNode(char* name, int type);
void pushLDecl(struct LDNode* node);
struct LDNode* popLDecl(struct LDNode* head);
void UpdateLST(int type);
