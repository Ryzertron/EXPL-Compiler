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
#include "label.h"
//int variable[26];
void yyerror(char const * s);
extern FILE* yyin;
int yylex(void);
tnode* SyntaxRoot;
%}

%union {
    struct tnode* root;
    char * string;
    dType type;
    Dnode* droot;


}

%start program
%token DELIM SEP SBLOCK EBLOCK 
%token <root> CONST
%token READ WRITE
%token ADD SUB MUL DIV ASSIGN 
%token GT LT NE EQ LE GE
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE REPEAT UNTIL
%token BREAK CONTINUE
%token BRKP
%token DECL ENDDECL
%token TYPE_INT TYPE_STR
%token VAR
%token STRING
%type <root> program expr statements stmnt Vsin
%left ADD SUB
%left MUL DIV
%left GE LE NE
%left GT LT EQ

%%

decBlock: DECL declist ENDDECL
        | DECL ENDDECL;

declist: declist decl
        | decl;

decl : type VList DELIM     {CreateGST($<dType>1);}

type : TYPE_INT
     | TYPE_STR;

VList : VList SEP Vsin {pushDecl($<root>2);}
      | Vsin           {pushDecl($<root>1);}

Vsin : VAR                                      {$$ = $<root>1;}
     | VAR '[' TYPE_INT ']'                     {
                                                    $<root>1 -> left = $<root>3;
                                                    $$ = $<root>1;
                                                }
     | VAR '[' TYPE_INT ']' '[' TYPE_INT ']'    {
                                                    $<root>1 -> left = $<root>3;
                                                    $<root>1 -> right = $<root>6;
                                                    $$ = $<root>1;
                                                }
     | VAR '[' TYPE_INT ']' '[' VAR ']'         {
                                                    $<root>1 -> left = $<root>3;
                                                    $<root>1 -> right = $<root>6;
                                                    $$ = $<root>1;
                                                }
     | VAR '[' VAR ']' '[' VAR ']'              {
                                                    $<root>1 -> left = $<root>3;
                                                    $<root>1 -> right = $<root>6;
                                                    $$ = $<root>1;
                                                }
     | VAR '[' VAR ']' '[' TYPE_INT ']'         {
                                                    $<root>1 -> left = $<root>3;
                                                    $<root>1 -> right = $<root>6;
                                                    $$ = $<root>1;
                                                }
     | VAR '[' VAR ']'                          {
                                                    $<root>1 -> left = $<root>3;
                                                    $$ = $<root>1;
                                                }

program: SBLOCK statements EBLOCK DELIM { 
                                            SyntaxRoot = $<root>2;
                                            return 0;

                                        }
       | SBLOCK EBLOCK DELIM            {
                                            printf("No Logic found\n");
                                            exit(1);
                                        };

statements: statements stmnt            {$$ = createSyntaxNode(T_CONN, none, (data){.value = 0}, $<root>1, $<root>2,NULL);}
          | stmnt                       {$$ = $<root>1;};

stmnt: READ '(' VAR ')' DELIM                                           {$$ = createSyntaxNode(T_READ, none, (data){.value = 0}, $<root>3, NULL,NULL);}
     | WRITE '(' expr ')' DELIM                                         {$$ = createSyntaxNode(T_WRITE, none, (data){.value = 0}, $<root>3, NULL,NULL);}
     | VAR ASSIGN expr DELIM                                            {$$ = createSyntaxNode(T_ASSG, none, (data){.value = 0}, $<root>1, $<root>3,NULL);}
     | IF '(' expr ')' THEN statements ELSE statements ENDIF DELIM      {
                                                                            node then = createSyntaxNode(T_THEN, none, (data){.value = 0}, $<root>6, $<root>8,NULL); 
                                                                            $$ = createSyntaxNode(T_IF, none, (data){.value = 0}, $<root>3, then,NULL);
                                                                        }
     | IF '(' expr ')' THEN statements ENDIF DELIM                      {
                                                                            node then = createSyntaxNode(T_THEN, none, (data){.value = 0}, $<root>6, NULL,NULL); //Supply NULL instead of ELSE
                                                                            $$ = createSyntaxNode(T_IF, none, (data){.value = 0}, $<root>3, then,NULL);
                                                                        }
     | WHILE '(' expr ')' DO statements ENDWHILE DELIM                  {$$ = createSyntaxNode(T_WHILE, none, (data){.value = 0}, $<root>3, $<root>6,NULL);}
     | BREAK DELIM                                                      {$$ = createSyntaxNode(T_BREAK, none, (data){.value = 0}, NULL, NULL,NULL);}
     | CONTINUE DELIM                                                   {$$ = createSyntaxNode(T_CONT, none, (data){.value = 0}, NULL, NULL,NULL);}
     | BRKP DELIM                                                       {$$ = createSyntaxNode(T_BRKP, none, (data){.value = 0}, NULL, NULL,NULL);}
     | DO statements WHILE '(' expr ')' DELIM                           {$$ = createSyntaxNode(T_DWHILE, none, (data){.value = 0}, $<root>2, $<root>5,NULL);}               
     | REPEAT statements UNTIL '(' expr ')' DELIM                       {$$ = createSyntaxNode(T_REPEAT, none, (data){.value = 0}, $<root>2, $<root>5,NULL);}
     | decBlock;

expr: expr ADD expr         {$$ = createSyntaxNode(T_ADD, none, (data){.value = 0}, $<root>1, $<root>3,NULL);}
    | expr SUB expr         {$$ = createSyntaxNode(T_SUB, none, (data){.value = 0}, $<root>1, $<root>3,NULL);}
    | expr MUL expr         {$$ = createSyntaxNode(T_MUL, none, (data){.value = 0}, $<root>1, $<root>3,NULL);}
    | expr DIV expr         {$$ = createSyntaxNode(T_DIV, none, (data){.value = 0}, $<root>1, $<root>3,NULL);}
    | expr LT expr          {$$ = createSyntaxNode(T_LT, none, (data){.value = 0}, $<root>1, $<root>3,NULL);}
    | expr GT expr          {$$ = createSyntaxNode(T_GT, none, (data){.value = 0}, $<root>1, $<root>3,NULL);}
    | expr LE expr          {$$ = createSyntaxNode(T_LE, none, (data){.value = 0}, $<root>1, $<root>3,NULL);}
    | expr GE expr          {$$ = createSyntaxNode(T_GE, none, (data){.value = 0}, $<root>1, $<root>3,NULL);}
    | expr EQ expr          {$$ = createSyntaxNode(T_EQ, none, (data){.value = 0}, $<root>1, $<root>3,NULL);}
    | expr NE expr          {$$ = createSyntaxNode(T_NE, none, (data){.value = 0}, $<root>1, $<root>3,NULL);}
    | '(' expr ')'          {$$ = $<root>2;}
    | CONST                 {$$ = $<root>1;}
    | VAR                   {$$ = $<root>1;}
    | STRING                {$<root>$ = $<root>1;};

%%

void yyerror(char const *s) {
    printf("yyerror : %s\n", s);
}

int main() {
    SP = STACKBASE;
    BP = STACKBASE+1;
    yyin = fopen("test.txt","r");
    yyparse();
    FILE* target = fopen("temp.xsm","w");
    initxsm(target);
    codeGen(SyntaxRoot,target);
    endxsm(target);
    fclose(target);
    translate();
    remove("temp.xsm");
    return 0;
}

