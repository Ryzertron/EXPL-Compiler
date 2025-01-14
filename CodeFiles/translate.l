%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "label.h"

int isFirst = 1;
FILE* output;
%}

decl    L[0-9]+:\n
ref     L[0-9]+\n
%option prefix="LT"
%%

{decl}  {   
            if (isFirst) {
                newLabel(LINE, LTtext, 2);
            }
        }

{ref}   {   
            if (isFirst) {
                LINE++;
            }
            else {
                int address = getAddr(LTtext,1);
                fprintf(output, "%d\n", address);
            }
        }
[\n]    {
            if (isFirst) {
                LINE++;
            }
            else {
                fprintf(output,"%s", LTtext);
            }
        }
.       {
            if (!isFirst) {
                fprintf(output, "%s", LTtext);
            }
        }

%%
int LTwrap(void) {
    if(isFirst) {
        fseek(LTin, 0, SEEK_SET);
        isFirst = 0;
        return 0;
    }
    return 1;
}

void translate() {
    LTin = fopen("temp.xsm","r");
    output  = fopen("out.xsm","w");
    LTlex();
    fclose(output);
    return;
}