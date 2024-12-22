#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator.h"

int LR = -1;

int getReg(){
    if(LR == 19){
        printf("Error: No more registers available");
        exit(1);
    }
    LR++;
    return LR;
}

void freeReg(){
    if (LR == -1){
        printf("Error : All registers already free");
        return;
    }
    LR--;
}

reg_index codeGen(node root, FILE* target){

    if(!root) return 0; //Below the leaf
    reg_index lreg = codeGen(root -> left, target);//Walk down the tree
    reg_index rreg = codeGen(root -> right, target);

    if (!(root -> left) && !(root -> right)){   //Leaf Node
        reg_index R = getReg();
        fprintf(target, "MOV R%d,%d\n", R,root -> value);
        return R;
    }

    if(strcmp(root -> op, "+") == 0){   //Addition
        fprintf(target, "ADD R%d,R%d\n",lreg,rreg);
    }

    if(strcmp(root -> op, "-") == 0){   //Subtraction
        fprintf(target, "SUB R%d,R%d\n",lreg,rreg);
    }

    if(strcmp(root -> op, "*") == 0){   //Multiplication
        fprintf(target, "MUL R%d,R%d\n",lreg,rreg);
    }

    if(strcmp(root -> op, "/") == 0){   //Division
        fprintf(target, "DIV R%d,R%d\n",lreg,rreg);
    }



    /*
        We can free one reg as the result is stored
    in the first register after the arithmetic. 
    */
    freeReg();

    
    return lreg;  // Return the register to caller
    
}

void write(reg_index reg, FILE* target){

    for(int i = 0; i <= LR; i++){   // PUSH registers in use
        fprintf(target,"PUSH R%d\n",i);
    }

    fprintf(target, "MOV R1,R%d\n", reg);
    fprintf(target, "MOV R0,\"Write\"\n"); 
    fprintf(target, "PUSH R0\n"); // push function code
    fprintf(target, "MOV R0, -2\n");
    fprintf(target, "PUSH R0\n"); // push file descriptor
    fprintf(target, "PUSH R1\n"); // push argument word
    fprintf(target, "PUSH R0\n"); // push arg 3
    fprintf(target, "PUSH R0\n"); // push ret space
    fprintf(target, "CALL 0\n");  // call library
    fprintf(target, "POP R0\n");  // pop ret val
    fprintf(target, "POP R1\n");  // pop arg 3
    fprintf(target, "POP R1\n");  // pop arg 2
    fprintf(target, "POP R1\n");  // pop arg 1
    fprintf(target, "POP R1\n");  // pop syscall
    
    
    for (int i = LR; i >= 0; i--){ //POP registers that was pushed initially
        fprintf(target, "POP R%d\n", i);
    }
}
