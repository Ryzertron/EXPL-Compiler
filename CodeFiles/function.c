#include "function.h"
#include "SymTable.h"
#include "tree.h"
#include "generator.h"

struct GST* currentFunction = NULL;

struct ParaNode* NewParaNode(char* name, int type) {
    struct ParaNode* temp = (struct ParaNode*)malloc(sizeof(struct ParaNode));
    temp -> name = (char*)malloc(strlen(name));
    strcpy(temp -> name, name);
    temp -> type = type;
    return temp;
}

struct ParaNode* pushParaNode(struct ParaNode* node, struct ParaNode* head) {
    if(!node){
        fprintf(stderr, "Internal Error: NULL node passed to pushParaNode\n");
        exit(1);
    }
    node -> next = head;
    return node;
}


int getNewFLabel() {
    static int Flabel = 1;
    return Flabel++;
}



void DeclareFunction(char* name, int type, struct ParaNode* PList) {
    if(GSTLookup(name)) {
        fprintf(stderr, "Error: Redeclaration of Identifier %s\n", name);
        exit(1);
    }

    struct GST* temp = (struct GST*)malloc(sizeof(struct GST));
    temp -> name = (char*)malloc(strlen(name));
    strcpy(temp -> name, name);
    temp -> dtype = type;
    temp -> size = 0;
    temp -> binding = -1;
    temp -> rows = 0;
    temp -> cols = 0;
    temp -> Flabel = getNewFLabel();
    temp -> paraList = PList;
    temp -> next = GST_HEAD;
    GST_HEAD = temp;
}

struct ParaNode* reversed(struct ParaNode* head) {
    struct ParaNode* prev = NULL;
    struct ParaNode* curr = head;
    struct ParaNode* next = NULL;
    while(curr) {
        next = curr -> next;
        curr -> next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

void pushParametersToLST(struct ParaNode* Plist) {
    Plist = reversed(Plist);
    int count = -3;
    struct ParaNode* temp = Plist;
    while(temp) {
        struct LST* node = (struct LST*)malloc(sizeof(struct LST));
        node -> name = (char*)malloc(strlen(temp -> name));
        strcpy(node -> name, temp -> name);
        node -> dtype = temp -> type;
        node -> size = 1;
        node -> binding = count--;
        node -> next = LST_HEAD;
        LST_HEAD = node;
        struct ParaNode* next = temp -> next;
        free(temp -> name);
        free(temp);
        temp = next;
    }
    
}

void printParaList(struct ParaNode* head) {
    
    struct ParaNode* temp = head;
    while(temp) {
        
        printf("(%s --- %d)", temp -> name, temp -> type);
        
        temp = temp -> next;
    }
    
    printf("\n");
}


void defineFunctionHead(char* name, int type, struct ParaNode* PList) {
    
    struct GST* entry= GSTLookup(name);
    
    if(!entry || entry -> Flabel == -1) {
        fprintf(stderr, "Error: Function %s not declared\n", name);
        exit(1);
    }

    if(entry -> dtype != type) {
        fprintf(stderr, "Error: Mismatch in function return type for %s\n", name);
        exit(1);
    }
    
    struct ParaNode* paraProto = entry -> paraList;
    struct ParaNode* paraDef = PList;

    

    //Checking prototype compatability
    while(paraProto || paraDef) {
        if(!paraProto || !paraDef || strcmp(paraProto -> name, paraDef -> name) != 0 || paraProto -> type != paraDef -> type) {
            fprintf(stderr, "Error: Definition incompatible with declaration for %s\n", name);
            exit(1);
        }
        paraProto = paraProto -> next;
        paraDef = paraDef -> next;
    }
    
    currentFunction = entry;

    fprintf(target, "F%d:\n", entry -> Flabel);
    fprintf(target, "PUSH BP\n");
    fprintf(target, "MOV BP, SP\n");
    int length = LSTLength();
    fprintf(target, "ADD SP, %d\n", length);
    pushParametersToLST(PList);
    
}

void defineFunctionBody(struct tnode* root) {
    
    
    codeGen(root, target);
    
    //Force Return
    fprintf(target, "ADD SP, %d\n", 0-LSTSize);
    fprintf(target, "POP BP\n");
    fprintf(target, "RET\n");
    clearLST();
    currentFunction = NULL;
    
}

void defineMain(struct tnode* root) {
    struct GST* entry = (struct GST*)malloc(sizeof(struct GST));
    entry -> name = (char*)malloc(strlen("main"));
    strcpy(entry -> name, "main");
    entry -> dtype = D_INT;
    entry -> size = 0;
    entry -> binding = -1;
    entry -> rows = 0;
    entry -> cols = 0;
    entry -> Flabel = 0;
    entry -> paraList = NULL;
    entry -> next = NULL;

    currentFunction = entry;
    fprintf(target, "F0:\n");
    fprintf(target, "PUSH BP\n");
    fprintf(target, "MOV BP, SP\n");
    int length = LSTLength();
    fprintf(target, "ADD SP, %d\n", length);
    codeGen(root, target);
    //Force Exit
    fprintf(target, "INT 10\n");
    clearLST();
    currentFunction = NULL;
    free(entry -> name);
    free(entry);
}



//==========================Argument Functions=========================


struct ArgNode* pushArgNode(struct tnode* expr, struct ArgNode* head) {
    if(!expr){
        fprintf(stderr, "Internal Error: NULL argNode passed to pushArgNode\n");
        exit(1);
    }
    struct ArgNode* temp = (struct ArgNode*)malloc(sizeof(struct ArgNode));
    temp -> root = expr;
    temp -> next = head;
    head = temp;
    return head;
}

void printArgList(struct ArgNode* head) {
    struct ArgNode* temp = head;
    
    while(temp) {
        printf("%d ", temp -> root -> dtype);
        temp = temp -> next;
    }
    printf("\n");
}

