#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

node leafNode(int val){
    node temp = (node)malloc(sizeof(tnode));
    temp -> value = val;
    temp -> op = NULL;
    temp -> left = NULL;
    temp -> right = NULL;
    return temp;
}

node opNode(char op,node left, node right){
    node temp = (node)malloc(sizeof(tnode));
    temp -> op = (char*)malloc(sizeof(char));
    *(temp -> op) = op;
    temp -> left = left;
    temp -> right = right;
    return temp;
}

int eval(node root){
    if (root -> op == NULL) return root -> value;
    else {
        switch(*(root -> op)){
            case '+' : return eval(root -> left) + eval(root -> right);
                break;
            case '-' : return eval(root -> left) - eval(root -> right);
                break;
            case '*' : return eval(root -> left) * eval(root -> right);
                break;
            case '/' : return eval(root -> left) / eval(root -> right);
        }
    }
}

void prefix(node root){
    if (!root) return;
    if (root -> op == NULL) printf(" %d",root -> value);
    else                    printf(" %s",root -> op);
    prefix(root -> left);
    prefix(root -> right);
}

void postfix(node root){
    if (!root) return;
    postfix(root -> left);
    postfix(root -> right);
    if (root -> op == NULL) printf(" %d", root -> value);
    else printf(" %s", root -> op);
}





