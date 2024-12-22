/*
----------------Lexer For Expl--------------------
Lexer generate tokens from the input expl program.
Changes since previous commit is briefed below
--------------------------------------------------

-------Changelog [Previous Commit 0e9da633]-------
 - Added rules for variable names. only one character is allowed for now.
 - Changed function calls according to their change in definition.
 -------------------------------------------------
*/

%{
    #include "y.tab.h"
    #include "tree.h"
%}

%%

[0-9]+  {
            yylval.root = createSyntaxNode(T_CONST,none,(data){.value = atoi(yytext)},NULL,NULL);
            return NUMBER;
        }

[a-z]   {   
            char * temp = (char*)malloc(sizeof(char));
            strcpy(temp,yytext);
            yylval.root = createSyntaxNode(T_VAR,VAR_CHAR,(data){.varname = temp},NULL,NULL);
            return VAR;
        }

"+"     {return ADD;}
"-"     {return SUB;}
"*"     {return MUL;}
"/"     {return DIV;}
"="     {return ASSIGN;}
";"     {return DELIM;}
"read"  {return READ;}
"write" {return WRITE;}
"begin" {return SBLOCK;}
"end"   {return EBLOCK;}
[ \t]    {}
[()]    {return *yytext;}
[\n]    {}

%%

int yywrap(){
    return 1;
}