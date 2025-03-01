#include "SymTable.h"
#include "generator.h"

GST* GST_HEAD = NULL;
GST* GSTLookup(char * name) {
    GST * temp = GST_HEAD;
    while(temp) {
        if(strcmp(temp -> name, name) == 0) return temp;
        temp = temp -> next;

    }
    return NULL;
}

void GSTInstall(struct tnode* root, int type, int offset) {
    GST* temp = GSTLookup(root -> content.varname);
    if(temp) {
       printf("Variable %s already declared\n", root -> content.varname);
       exit(1); 
    }
    temp = (GST*)malloc(sizeof(GST));

    temp -> name = strndup(root -> content.varname, strlen(root -> content.varname)- offset);

    temp -> dtype = type;

    int rows = 0,cols = 0;
    if (root -> left) rows = root -> left -> content.value;
    if (root -> right) cols = root -> right -> content.value;

    temp -> rows = rows;
    temp -> cols = cols;
    temp -> binding = SP;
    temp -> size = max(rows*cols, max(rows,max(cols, 1)));
    SP += temp -> size;
    BP += temp -> size;
    temp -> next = GST_HEAD;
    GST_HEAD = temp;
}
int max(int a, int b) {
    return (a>b) ? a : b;
}

int min(int a, int b) {
    return (a>b) ? b : a;
}

void printGST() {
    GST* temp = GST_HEAD;
    printf("----------------GLOBAL_SYMBOL_TABLE----------------\n\n");
    printf("Name\t\tType\tSize\tBind\tRows\tCols\n");
    while(temp) {
        printf("%s\t\t%d\t%d\t%d\t%d\t%d\n",temp -> name,temp -> dtype,temp -> size,temp -> binding, temp -> rows, temp -> cols);
        temp = temp -> next;
    }
    printf("\n---------------------------------------------------\n");
}