/*
------------------Pasrser for Expl------------------
This file contains rules that verify the syntax for EXPL.
As we move up the stages changes will be made to the existing rules.
Changes that was made since the previous commit will be briefed below.
------------------------------------------------

------Changelog [Previous Commit 0e9da633]------

    - Non terminal 'start' is replaced by 'program' along with some refactoring
    - Added Rules to support variables
    - Added Rules for begin...end syntax
    - Function calls refactored to match the modification in the definitions
------------------------------------------------
*/

%{
#include "tree.h"
void yyerror(char const * s);
extern FILE* yyin;
int yylex(void);
#include "generator.h"
%}

%union{
    struct tnode* root;
}

%start program
%token <root> NUMBER VAR
%token ADD SUB MUL DIV ASSIGN DELIM SBLOCK EBLOCK READ WRITE
%type <root> program expr statements stmnt ipstmnt opstmnt astmnt
%left ADD SUB
%left MUL DIV

%%


program: SBLOCK statements EBLOCK DELIM {
                                        FILE* target = fopen("output.xsm","w");
                                        initxsm(target);
                                        codeGen($<root>2,target);
                                        endxsm(target);
                                        fclose(target);
                                        exit(0);
                                    }
       | SBLOCK EBLOCK DELIM            {
                                        printf("No Logic found\n");
                                        exit(1);
                                    };

statements: statements stmnt        {$$ = createSyntaxNode(T_CONN, none, (data){.value = 0}, $<root>1, $<root>2);}
          | stmnt                   {$$ = $<root>1;};

stmnt: ipstmnt                      {$$ = $<root>1;}
     | opstmnt                      {$$ = $<root>1;}
     | astmnt                       {$$ = $<root>1;};

ipstmnt: READ '(' VAR ')' DELIM     {$$ = createSyntaxNode(T_READ, none, (data){.value = 0}, $<root>3, NULL);};

opstmnt: WRITE '(' expr ')' DELIM   {$$ = createSyntaxNode(T_WRITE, none, (data){.value = 0}, $<root>3, NULL);};

astmnt : VAR ASSIGN expr DELIM      {$$ = createSyntaxNode(T_OPER, OP_ASSG, (data){.value = 0}, $<root>1, $<root>3);};

expr: expr ADD expr                 {$$ = createSyntaxNode(T_OPER, OP_ADD, (data){.value = 0}, $<root>1, $<root>3);}
    | expr SUB expr                 {$$ = createSyntaxNode(T_OPER, OP_SUB, (data){.value = 0}, $<root>1, $<root>3);}
    | expr MUL expr                 {$$ = createSyntaxNode(T_OPER, OP_MUL, (data){.value = 0}, $<root>1, $<root>3);}
    | expr DIV expr                 {$$ = createSyntaxNode(T_OPER, OP_DIV, (data){.value = 0}, $<root>1, $<root>3);}
    | '(' expr ')'                  {$$ = $<root>2;}
    | NUMBER                        {$$ = $<root>1;}
    | VAR                           {$$ = $<root>1;};

%%

void yyerror(char const *s) {
    printf("yyerror : %s", s);
}

int main() {
    yyin = fopen("test.txt","r");
    yyparse();
    return 1;
}

