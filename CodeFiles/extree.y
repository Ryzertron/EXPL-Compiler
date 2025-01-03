/*
------------------Pasrser for Expl------------------
This file contains rules that verify the syntax for EXPL.
As we move up the stages changes will be made to the existing rules.
Changes that was made since the previous commit will be briefed below.
------------------------------------------------

------Changelog [Previous Commit c3b8b4]------
    - Some of the CFGs were found useless and hence refactored
    - Bug related to rptstmnt is resolved.
------------------------------------------------
*/

%{
#include "tree.h"
#include "generator.h"
void yyerror(char const * s);
extern FILE* yyin;
int yylex(void);
%}

%union{
    struct tnode* root;
}

%start program
%token DELIM SBLOCK EBLOCK 
%token <root> NUMBER VAR
%token READ WRITE
%token ADD SUB MUL DIV ASSIGN 
%token GT LT NE EQ LE GE
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE REPEAT UNTIL
%token BREAK CONTINUE
%token BRKP
%type <root> program expr statements stmnt
%left ADD SUB
%left MUL DIV
%left GE LE NE
%left GT LT EQ

%%


program: SBLOCK statements EBLOCK DELIM {
                                        FILE* target = fopen("temp.xsm","w");
                                        initxsm(target);
                                        codeGen($<root>2,target);
                                        endxsm(target);
                                        if (target) fclose(target);
                                        exit(0);
                                    }
       | SBLOCK EBLOCK DELIM            {
                                        printf("No Logic found\n");
                                        exit(1);
                                    };

statements: statements stmnt        {$$ = createSyntaxNode(T_CONN, none, (data){.value = 0}, $<root>1, $<root>2);}
          | stmnt                   {$$ = $<root>1;};

stmnt: READ '(' VAR ')' DELIM                                           {$$ = createSyntaxNode(T_IO, S_READ, (data){.value = 0}, $<root>3, NULL);}
     | WRITE '(' expr ')' DELIM                                         {$$ = createSyntaxNode(T_IO, S_WRITE, (data){.value = 0}, $<root>3, NULL);}
     | VAR ASSIGN expr DELIM                                            {$$ = createSyntaxNode(T_ASSG, none, (data){.value = 0}, $<root>1, $<root>3);}
     | IF '(' expr ')' THEN statements ELSE statements ENDIF DELIM      {
                                                                            node then = createSyntaxNode(T_CTRL, S_THEN, (data){.value = 0}, $<root>6, $<root>8); 
                                                                            $$ = createSyntaxNode(T_CTRL, S_IF, (data){.value = 0}, $<root>3, then);
                                                                        }
     | IF '(' expr ')' THEN statements ENDIF DELIM                      {
                                                                            node then = createSyntaxNode(T_CTRL, S_THEN, (data){.value = 0}, $<root>6, NULL); //Supply NULL instead of ELSE
                                                                            $$ = createSyntaxNode(T_CTRL, S_IF, (data){.value = 0}, $<root>3, then);
                                                                        }
     | WHILE '(' expr ')' DO statements ENDWHILE DELIM                  {$$ = createSyntaxNode(T_LOOP, S_WHILE, (data){.value = 0}, $<root>3, $<root>6);}
     | BREAK DELIM                                                      {$$ = createSyntaxNode(T_JUMP, S_BREAK, (data){.value = 0}, NULL, NULL);}
     | CONTINUE DELIM                                                   {$$ = createSyntaxNode(T_JUMP, S_CONT, (data){.value = 0}, NULL, NULL);};
     | BRKP DELIM                                                       {$$ = createSyntaxNode(T_JUMP, S_BRKP, (data){.value = 0}, NULL, NULL);};
     | DO statements WHILE '(' expr ')' DELIM                           {$$ = createSyntaxNode(T_LOOP, S_DWHILE, (data){.value = 0}, $<root>2, $<root>5);}               
     | REPEAT statements UNTIL '(' expr ')' DELIM                       {$$ = createSyntaxNode(T_LOOP, S_REPEAT, (data){.value = 0}, $<root>2, $<root>5);};

expr: expr ADD expr                 {$$ = createSyntaxNode(T_ARITH, S_ADD, (data){.value = 0}, $<root>1, $<root>3);}
    | expr SUB expr                 {$$ = createSyntaxNode(T_ARITH, S_SUB, (data){.value = 0}, $<root>1, $<root>3);}
    | expr MUL expr                 {$$ = createSyntaxNode(T_ARITH, S_MUL, (data){.value = 0}, $<root>1, $<root>3);}
    | expr DIV expr                 {$$ = createSyntaxNode(T_ARITH, S_DIV, (data){.value = 0}, $<root>1, $<root>3);}
    | expr LT expr                  {$$ = createSyntaxNode(T_REL, S_LT, (data){.value = 0}, $<root>1, $<root>3);}
    | expr GT expr                  {$$ = createSyntaxNode(T_REL, S_GT, (data){.value = 0}, $<root>1, $<root>3);}
    | expr LE expr                  {$$ = createSyntaxNode(T_REL, S_LE, (data){.value = 0}, $<root>1, $<root>3);}
    | expr GE expr                  {$$ = createSyntaxNode(T_REL, S_GE, (data){.value = 0}, $<root>1, $<root>3);}
    | expr EQ expr                  {$$ = createSyntaxNode(T_REL, S_EQ, (data){.value = 0}, $<root>1, $<root>3);}
    | expr NE expr                  {$$ = createSyntaxNode(T_REL, S_NE, (data){.value = 0}, $<root>1, $<root>3);}
    | '(' expr ')'                  {$$ = $<root>2;}
    | NUMBER                        {$$ = $<root>1;}
    | VAR                           {$$ = $<root>1;};

%%

void yyerror(char const *s) {
    printf("yyerror : %s\n", s);
}

int main() {
    yyin = fopen("test.txt","r");
    yyparse();
    return 1;
}

