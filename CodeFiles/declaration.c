#include "declaration.h"
#include "SymTable.h"

DNode* DSH = NULL; //Declaration Stack head

void pushDecl(tnode* root){
    GST* temp = GSTLookup(root -> content.varname);
    if (temp) {
        fprintf(stderr,"Error: Redeclaration of identifier %s\n", root -> content.varname);
        exit(1);
    }

    DNode* head = DSH;
    while(head){
        if(strcmp(head -> data -> content.varname, root -> content.varname) == 0){
            fprintf(stderr, "Redclaration of identifier %s\n", root -> content.varname);
            exit(1);
        }
        head = head -> next;
    }

    DNode* New = (DNode*)malloc(sizeof(DNode));
    New -> data = root;
    New -> next = DSH;
    DSH = New;
}

DNode* popDecl(){
    if(DSH) {
        DNode* temp = DSH;
        DSH = DSH -> next;
        return temp;
    }
    return NULL;
}

int DSEmpty() {
    return DSH == NULL;
}

void CreateGST (dType type) {
    DNode* temp = popDecl();
    while(temp) {
        printf("%s ---- %d--%d|",temp -> data -> content.varname,temp -> data -> dtype,type);
        if(temp -> data -> dtype == 5) temp -> data -> dtype = type+1;
        else temp -> data -> dtype = type;
        printf("%s ---- %d--%d|",temp -> data -> content.varname,temp -> data -> dtype,type);
        GSTInstall(temp -> data, type, 0);
        free(temp);
        temp = popDecl();
    }
    printGST();
}