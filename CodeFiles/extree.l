/*
----------------Lexer For Expl--------------------
Lexer generate tokens from the input expl program.
Changes since previous commit is briefed below
--------------------------------------------------

-------Changelog [Previous Commit c3b8b4]-------
    - Comments caused compilation error so it was removed.
------------------------------------------------
*/

%{
    #include "y.tab.h"
    #include "tree.h"
    #include "SymTable.h"
%}
identifier [a-zA-Z][a-zA-Z0-9]*
strings ["][A-Za-z_!@#$%^&\(\)\\]+["]
number [0-9]+


%%
"+"             {return ADD;}
"-"             {return SUB;}
"*"             {return MUL;}
"/"             {return DIV;}
"%"             {return MOD;}
"="             {return ASSIGN;}
    
"<"             {return LT;}
">"             {return GT;}
"<="            {return LE;}
">="            {return GE;}
"=="            {return EQ;}
"!="            {return NE;}
    
"if"            {return IF;}
"else"          {return ELSE;}
"then"          {return THEN;}
"endif"         {return ENDIF;}
    
"while"         {return WHILE;}
"do"            {return DO;}
"endwhile"      {return ENDWHILE;}
"break"         {return BREAK;}
"continue"      {return CONTINUE;}
    
"repeat"        {return REPEAT;}
"until"         {return UNTIL;}
    
    
"read"          {return READ;}
"write"         {return WRITE;}
    
"{"             {return BOPEN;}
"}"             {return BCLOSE;}
"begin"         {return SBLOCK;}
"end"           {return EBLOCK;}
";"             {return DELIM;}
"breakpoint"    {return BRKP;}

"decl"          {return DECL;}
"enddecl"       {return ENDDECL;}
"int"           {return TYPE_INT;}
"str"           {return TYPE_STR;}
","             {return SEP;}
"&"             {return REF;}
"main"          {return MAIN;}
"return"        {return RET;}

{number}    {
                yylval.value = atoi(yytext);
                return NUMBER;
            }

{identifier} {
                char* dupname = strdup(yytext);
                yylval.name = dupname;
                return ID;
            }
{strings} {
            char* dupstring = strndup(yytext+1, strlen(yytext+1)-1);
            yylval.syn = createSyntaxNode(T_CONST, D_STR, (data){.string = dupstring},NULL,NULL);
            return STRING;
        }

[ \t]       {}
[()\[\]]        {return *yytext;}
[\n]        {}

%%

int yywrap(){
    return 1;
}