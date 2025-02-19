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
strings \"((\\\")|[^"])*\"
number [0-9]+


%%


[a-z]       {   
                char * temp = (char*)malloc(sizeof(char));
                strcpy(temp,yytext);
                yylval.root = createSyntaxNode(T_VAR, D_INT,(data){.varname = temp},NULL,NULL,NULL);
                return VAR;
            }

"+"         {return ADD;}
"-"         {return SUB;}
"*"         {return MUL;}
"/"         {return DIV;}
"="         {return ASSIGN;}

"<"         {return LT;}
">"         {return GT;}
"<="        {return LE;}
">="        {return GE;}
"=="        {return EQ;}
"!="        {return NE;}

"if"        {return IF;}
"else"      {return ELSE;}
"then"      {return THEN;}
"endif"     {return ENDIF;}

"while"     {return WHILE;}
"do"        {return DO;}
"endwhile"  {return ENDWHILE;}
"break"     {return BREAK;}
"continue"  {return CONTINUE;}

"repeat" {return REPEAT;}
"until"     {return UNTIL;}


"read"      {return READ;}
"write"     {return WRITE;}

"begin"     {return SBLOCK;}
"end"       {return EBLOCK;}
";"         {return DELIM;}

"decl"      {return DECL;}
"enddecl"   {return ENDDECL;}
"int"       {return TYPE_INT;}
"str"       {return TYPE_STR;}
","         {return SEP}

{number}    {
                yylval.root = createSyntaxNode(T_CONST,D_INT,(data){.value = atoi(yytext)},NULL,NULL,NULL);
                return CONST;
            }

{identifier} {
                GST* entry = GSTLookup(yytext);
                char* dupname = strdup(yytext);
                yylval.root = createSyntaxNode(T_ID,none,(data){.varname = dupname},NULL,NULL,entry);
                return VAR;
            }
{strings} {
            char* dupstring = strndup(yytext+1, strlen(yytext+1)-1);
            yylval.root = createSyntaxNode(T_CONST, D_STR, (data){.string = dupstring},NULL,NULL,NULL);
        }

[ \t]       {}
[()\[\]]        {return *yytext;}
[\n]        {}

%%

int yywrap(){
    return 1;
}