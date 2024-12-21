%{
#include "tree.h"
void yyerror(char const * s);
extern FILE* yyin;
int yylex(void);

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
                                printf("Prefix:");
                                prefix($<root>1);
                                printf("\n");
                                printf("Postfix:");
                                postfix($<root>1);
                                printf("\n");
                                printf("Answer: %d\n",eval($<root>1));
                                exit(0);
                            };

expr: expr ADD expr        {$$ = opNode('+',$<root>1,$<root>3);}
    | expr SUB expr       {$$ = opNode('-',$<root>1,$<root>3);}
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

