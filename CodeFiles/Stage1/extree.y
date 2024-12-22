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

%token <root> NUMBER 
%token ADD SUB MUL DIV NEWLINE
%type <root> start expr
%left ADD SUB
%left MUL DIV

%%

start: expr NEWLINE         {
                                FILE * target = fopen("output.xsm", "w");
                                fprintf(target, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n", 0,2056,0,0,0,0,0,0);
                                fprintf(target, "MOV SP, 4095\n");
                                fprintf(target, "MOV BP, 4096\n");
                                write(codeGen($<root>1, target), target);
                                fprintf(target, "INT 10\n");
                                exit(0);
                            };

expr: expr ADD expr         {$$ = opNode('+',$<root>1,$<root>3);}
    | expr SUB expr         {$$ = opNode('-',$<root>1,$<root>3);}
    | expr MUL expr         {$$ = opNode('*',$<root>1,$<root>3);}
    | expr DIV expr         {$$ = opNode('/',$<root>1,$<root>3);}
    | '(' expr ')'          {$$ = $<root>2;}
    | NUMBER                {$$ = $<root>1;};

%%

void yyerror(char const *s) {
    printf("yyerror : %s", s);
}

int main() {
    yyin = fopen("test.txt","r");
    yyparse();
    return 1;
}

