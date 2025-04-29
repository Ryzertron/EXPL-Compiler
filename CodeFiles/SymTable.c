#include "SymTable.h"
#include "generator.h"


// ===============================Global Symbol Table==============================
struct GST* GST_HEAD = NULL;
struct GST* GSTLookup(char * name) {
    
    struct GST * temp = GST_HEAD;
    while(temp) {
        if(strcmp(temp -> name, name) == 0) return temp;
        temp = temp -> next;

    }
    return NULL;
}

void GSTInstall(char* name, int dtype, int rows, int cols, int Flabel) {

    struct GST* temp = GSTLookup(name);
    if(temp) {
        fprintf(stderr, "Redeclaration of variable %s\n", name);
        exit(1);
    }
    temp = (struct GST*)malloc(sizeof(struct GST));

    //Inititalise Values
    temp -> name = (char*)malloc(strlen(name)+1);
    strcpy(temp -> name, name);
    temp -> dtype = dtype;
    temp -> rows = rows;
    temp -> cols = cols;
    temp -> Flabel = Flabel;
    temp -> size = max(rows*cols, max(rows,1));
    temp -> binding = SP;
    temp -> paraList = NULL;

    //Increment SP
    SP += temp -> size;

    //Install as Head
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
    struct GST* temp = GST_HEAD;
    printf("----------------GLOBAL_SYMBOL_TABLE----------------\n\n");
    printf("Name\t\tType\tSize\tBind\tRows\tCols\tFuncLabel\n");
    while(temp) {
        printf("%s\t\t%d\t%d\t%d\t%d\t%d\t%d\n",temp -> name,temp -> dtype,temp -> size,temp -> binding, temp -> rows, temp -> cols, temp -> Flabel);
        temp = temp -> next;
    }
    printf("\n---------------------------------------------------\n");
}


// ===============================Local Symbol Table==============================

struct LST* LST_HEAD = NULL;
int LSTBinding = 1;
int LSTSize = 0; //Not including parameters

struct LST* LSTLookup(char * name) {
    struct LST * temp = LST_HEAD;
    while(temp) {
        if(strcmp(temp -> name, name) == 0) return temp;
        temp = temp -> next;

    }
    return NULL;
}

void LSTInstall(char* name, int dtype) {
    if(LSTLookup(name)) {
        fprintf(stderr, "Redeclaration of variable %s\n", name);
        exit(1);
    }
    struct LST* temp = (struct LST*)malloc(sizeof(struct LST));
    temp -> name = (char*)malloc(strlen(name)+1);
    strcpy(temp -> name, name);
    temp -> dtype = dtype;
    temp -> size = 1;
    temp -> binding = LSTBinding++;
    temp -> next = LST_HEAD;
    LST_HEAD = temp;
}

void clearLST() {
    struct LST* temp = LST_HEAD;
    while(temp) {
        struct LST* next = temp -> next;
        free(temp -> name);
        free(temp);
        temp = next;
    }
    LST_HEAD = NULL;
    LSTBinding = 1;
}

int LSTLength() {
    int count = 0;
    struct LST* temp = LST_HEAD;
    while(temp) {
        count++;
        temp = temp -> next;
    }
    LSTSize = count;
    return count;
}

void printLST() {
    struct LST* temp = LST_HEAD;
    printf("----------------LOCAL_SYMBOL_TABLE----------------\n\n");
    printf("Name\t\tType\tSize\tBind\n");
    while(temp) {
        printf("%s\t\t%d\t%d\t%d\n",temp -> name,temp -> dtype,temp -> size,temp -> binding);
        temp = temp -> next;
    }
    printf("\n---------------------------------------------------\n");
}

