%{
    #include "y.tab.h"
    #include "tree.h"
%}

%%

[0-9]+  {
            yylval.root = leafNode(atoi(yytext));
            return NUMBER;
        }

"+"     {return ADD;}
"-"     {return SUB;}
"*"     {return MUL;}
"/"     {return DIV;}
[\t]    {}
[()]    {return *yytext;}
[\n]    {return NEWLINE;}

%%

int yywrap(){
    return 1;
}