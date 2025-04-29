#include "declaration.h"
#include "tree.h"
#include "SymTable.h"


//==================================Global Declaration Stack=========================

struct GDNode* GDSH = NULL; //Global Declaration Stack head

struct GDNode* NewGDNode(char* name, int type, int rows, int cols) {
    struct GDNode* temp = (struct GDNode*)malloc(sizeof(struct GDNode));
    temp -> name = (char*)malloc(strlen(name));
    strcpy(temp -> name, name);
    temp -> type = type;
    temp -> rows = rows;
    temp -> cols = cols;
    return temp;
}

void pushGDecl(struct GDNode* node) {
    node -> next = GDSH;
    GDSH = node;
}

struct GDNode* popGDecl() {
    if(GDSH == NULL) {
        return NULL;
    }
    struct GDNode* temp = GDSH;
    GDSH = GDSH -> next;
    return temp;
}

void UpdateGST(int type) {
    
    struct GDNode* temp = popGDecl();
    while(temp) {
        if(temp -> type == D_PTR) temp -> type = type + 1;
        else temp -> type = type;
        GSTInstall(temp -> name, temp -> type, temp -> rows, temp -> cols, -1);
        free(temp -> name);
        free(temp);
        temp = popGDecl();  
    }
}

//==================================Local Declaration Stack=========================

struct LDNode* LDSH = NULL; //Local Declaration Stack head

struct LDNode* NewLDNode(char* name, int type) {
    struct LDNode* temp = (struct LDNode*)malloc(sizeof(struct LDNode));
    temp -> name = (char*)malloc(strlen(name));
    strcpy(temp -> name, name);
    temp -> type = type;
    return temp;
}

void pushLDecl(struct LDNode* node) {
    node -> next = LDSH;
    LDSH = node;
}
struct LDNode* popLDecl(struct LDNode* head) {
    if(head == NULL) {
        return NULL;
    }
    struct LDNode* temp = head;
    LDSH = head -> next;
    return head;
}

void UpdateLST(int type) {
    struct LDNode* temp = popLDecl(LDSH);
    while(temp)  {
        if(temp -> type == D_PTR) temp -> type = type + 1;
        else temp -> type = type;
        LSTInstall(temp -> name, temp -> type);
        free(temp -> name);
        free(temp);
        temp = popLDecl(LDSH);
    }
}