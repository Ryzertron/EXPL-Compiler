/*
----------------Lexer For Expl--------------------
Lexer generate tokens from the input expl program.
Changes since previous commit is briefed below
--------------------------------------------------

-------Changelog [Previous Commit dbf1d30]-------
    - Added new rules for relational operators.
    - Added new rules for control,jump,loop statements.
 -------------------------------------------------
*/

%{
    #include "y.tab.h"
    #include "tree.h"
%}

%%

//Immediate
[0-9]+      {
                yylval.root = createSyntaxNode(T_CONST,none,(data){.value = atoi(yytext)},NULL,NULL);
                return NUMBER;
            }

//Identifier
[a-z]       {   
                char * temp = (char*)malloc(sizeof(char));
                strcpy(temp,yytext);
                yylval.root = createSyntaxNode(T_VAR,S_CHAR,(data){.varname = temp},NULL,NULL);
                return VAR;
            }

//Arithmetic
"+"         {return ADD;}
"-"         {return SUB;}
"*"         {return MUL;}
"/"         {return DIV;}
"="         {return ASSIGN;}

//Relational
"<"         {return LT;}
">"         {return GT;}
"<="        {return LE;}
">="        {return GE;}
"=="        {return EQ;}
"!="        {return NE;}

//Control
"if"        {return IF;}
"else"      {return ELSE;}
"endif"     {return ENDIF;}

//Loops
"while"     {return WHILE;}
"do"        {return DO;}
"endwhile"  {return ENDWHILE;}

//IO
"read"      {return READ;}
"write"     {return WRITE;}

//Syntax
"begin"     {return SBLOCK;}
"end"       {return EBLOCK;}
";"         {return DELIM;}

//whitespace
[ \t]       {}
[()]        {return *yytext;}
[\n]        {}

%%

int yywrap(){
    return 1;
}