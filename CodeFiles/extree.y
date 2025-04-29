%{
#include "tree.h"
#include "generator.h"
#include "label.h"
#include "declaration.h"
#include "function.h"
void yyerror(char const * s);
extern FILE* yyin;
FILE* target;
int yylex(void);
tnode* SyntaxRoot;
%}

%union {
    struct tnode* syn;
    struct GDNode* gdecl;
    struct LDNode* ldecl;
    struct ParaNode* para;
    struct ArgNode* arg;
    char * string;
    char * name;
    int type;
    int value;
}

%start program
%token MAIN RET
%token DELIM SEP SBLOCK EBLOCK BOPEN BCLOSE
%token <syn> NUMBER
%token READ WRITE
%token ADD SUB MUL DIV MOD ASSIGN 
%token GT LT NE EQ LE GE 
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE REPEAT UNTIL
%token BREAK CONTINUE
%token BRKP
%token DECL ENDDECL
%token TYPE_INT TYPE_STR
%token ID
%token <string> STRING
%token REF
%type <syn> expr statements stmnt IdUsage
%type <type> dtype
%type <gdecl> GV
%type <ldecl> LV
%type <para> Parameter PList
%type <arg> ArgList
%left NE EQ
%left GT LT GE LE
%left ADD SUB
%left MUL DIV MOD
%right DEREF
%right REF


%%

program : GDBlock FDBlock MainFunc
        | GDBlock MainFunc
        | FDBlock MainFunc
        | MainFunc
        ;

GDBlock : DECL GDList ENDDECL           {initxsm(target);}
        | DECL ENDDECL                  {initxsm(target);}
        ;

GDList : GDList GD
       | GD
       ;

GD : dtype GVList DELIM                 {UpdateGST($<type>1);}
   | dtype ID '(' PList ')' DELIM       {DeclareFunction($<name>2, $<type>1, $<para>4);}
   ;

dtype : TYPE_INT         {$$ = 1;}
      | TYPE_STR         {$$ = 3;}
      ;

GVList : GVList SEP GV   {pushGDecl($<gdecl>3);}
       | GV              {pushGDecl($<gdecl>1);}  
       ;

GV : ID                                 {$$ = NewGDNode($<name>1, none, 0,0);}
   | ID '[' NUMBER']'                   {$$ = NewGDNode($<name>1, none, $<value>3, 0);}  
   | ID '[' NUMBER ']' '[' NUMBER ']'   {$$ = NewGDNode($<name>1, none, $<value>3, $<value>6);}
   | MUL ID %prec DEREF                 {$$ = NewGDNode($<name>2, D_PTR, 0,0);}
   ;

PList : PList SEP Parameter             {$$ = pushParaNode($<para>3, $<para>1);}
      | Parameter                       {$$ = pushParaNode($<para>1, NULL);}
      |                                 {$$ = NULL;}
      ;
      

Parameter : dtype ID                    {$$ = NewParaNode($<name>2, $<type>1);}
          | dtype MUL ID %prec DEREF    {$$ = NewParaNode($<name>3, ($<type>1 + 1));}
          ;

FDBlock : FDBlock FD
        | FD
        ;

FD : dtype ID '(' PList ')' BOPEN LDBlock SBLOCK {defineFunctionHead($<name>2, $<type>1, $<para>4);}
        statements EBLOCK BCLOSE          {defineFunctionBody($<syn>10);}
        ;


LDBlock : DECL LDList ENDDECL
        | DECL ENDDECL
        |
        ;

LDList : LDList LD              
       | LD
       ;

LD : dtype LVList DELIM                 {UpdateLST($<type>1);}
   ;

LVList : LVList SEP LV                  {pushLDecl($<ldecl>3);}
       | LV                             {pushLDecl($<ldecl>1);}        
       ;

LV : ID                                 {$$ = NewLDNode($<name>1, none);}    
   | MUL ID %prec DEREF                 {$$ = NewLDNode($<name>1, D_PTR);} 
   ;

MainFunc : TYPE_INT MAIN '('')' BOPEN LDBlock SBLOCK statements EBLOCK BCLOSE   {defineMain($<syn>8);}
         ;

statements: statements stmnt                                            {$$ = createSyntaxNode(T_CONN, none, (data){.value = 0}, $<syn>1, $<syn>2);}
          | stmnt                                                       {$$ = $<syn>1;};

stmnt: READ '(' IdUsage ')' DELIM                                       {$$ = createSyntaxNode(T_READ, none, (data){.value = 0}, $<syn>3, NULL);}
     | WRITE '(' expr ')' DELIM                                         {$$ = createSyntaxNode(T_WRITE, none, (data){.value = 0}, $<syn>3, NULL);}
     | IdUsage ASSIGN expr DELIM                                        {$$ = createSyntaxNode(T_ASSG, none, (data){.value = 0}, $<syn>1, $<syn>3);}
     | MUL IdUsage ASSIGN expr DELIM                                    {
                                                                            node temp = createSyntaxNode(T_DEREF,none,(data){.value = 0},$<syn>2,NULL);
                                                                            $$ = createSyntaxNode(T_ASSG, none, (data){.value = 0}, temp, $<syn>4);
                                                                        }
     | IF '(' expr ')' THEN statements ELSE statements ENDIF DELIM      {
                                                                            node then = createSyntaxNode(T_THEN, none, (data){.value = 0}, $<syn>6, $<syn>8); 
                                                                            $$ = createSyntaxNode(T_IF, none, (data){.value = 0}, $<syn>3, then);
                                                                        }
     | IF '(' expr ')' THEN statements ENDIF DELIM                      {
                                                                            node then = createSyntaxNode(T_THEN, none, (data){.value = 0}, $<syn>6, NULL);
                                                                            $$ = createSyntaxNode(T_IF, none, (data){.value = 0}, $<syn>3, then);
                                                                        }
     | WHILE '(' expr ')' DO statements ENDWHILE DELIM                  {$$ = createSyntaxNode(T_WHILE, none, (data){.value = 0}, $<syn>3, $<syn>6);}
     | BREAK DELIM                                                      {$$ = createSyntaxNode(T_BREAK, none, (data){.value = 0}, NULL, NULL);}
     | CONTINUE DELIM                                                   {$$ = createSyntaxNode(T_CONT, none, (data){.value = 0}, NULL, NULL);}
     | BRKP DELIM                                                       {$$ = createSyntaxNode(T_BRKP, none, (data){.value = 0}, NULL, NULL);}
     | DO statements WHILE '(' expr ')' DELIM                           {$$ = createSyntaxNode(T_DWHILE, none, (data){.value = 0}, $<syn>2, $<syn>5);}               
     | REPEAT statements UNTIL '(' expr ')' DELIM                       {$$ = createSyntaxNode(T_REPEAT, none, (data){.value = 0}, $<syn>2, $<syn>5);}
     | RET expr DELIM                                                   {$$ = createSyntaxNode(T_RET, none, (data){.value = 0}, $<syn>2, NULL);}            
     ;

expr: expr ADD expr             {$$ = createSyntaxNode(T_ADD, none, (data){.value = 0}, $<syn>1, $<syn>3);}
    | expr SUB expr             {$$ = createSyntaxNode(T_SUB, none, (data){.value = 0}, $<syn>1, $<syn>3);}
    | expr MUL expr             {$$ = createSyntaxNode(T_MUL, none, (data){.value = 0}, $<syn>1, $<syn>3);}
    | expr DIV expr             {$$ = createSyntaxNode(T_DIV, none, (data){.value = 0}, $<syn>1, $<syn>3);}
    | expr MOD expr             {$$ = createSyntaxNode(T_MOD, none, (data){.value = 0}, $<syn>1, $<syn>3);}
    | expr LT expr              {$$ = createSyntaxNode(T_LT, none, (data){.value = 0}, $<syn>1, $<syn>3);}
    | expr GT expr              {$$ = createSyntaxNode(T_GT, none, (data){.value = 0}, $<syn>1, $<syn>3);}
    | expr LE expr              {$$ = createSyntaxNode(T_LE, none, (data){.value = 0}, $<syn>1, $<syn>3);}
    | expr GE expr              {$$ = createSyntaxNode(T_GE, none, (data){.value = 0}, $<syn>1, $<syn>3);}
    | expr EQ expr              {$$ = createSyntaxNode(T_EQ, none, (data){.value = 0}, $<syn>1, $<syn>3);}
    | expr NE expr              {$$ = createSyntaxNode(T_NE, none, (data){.value = 0}, $<syn>1, $<syn>3);}
    | '(' expr ')'              {$$ = $<syn>2;}
    | NUMBER                    {$$ = createSyntaxNode(T_CONST,D_INT,(data){.value = $<value>1},NULL,NULL);}
    | STRING                    {$$ = $<syn>1;}
    | IdUsage                   {$$ = $<syn>1;}
    | MUL IdUsage %prec DEREF   {$$ = createSyntaxNode(T_DEREF,none,(data){.value = 0},$<syn>2,NULL);}
    | REF IdUsage               {$$ = createSyntaxNode(T_REF,none,(data){.value = 0},$<syn>2,NULL);}
    | ID '(' ArgList ')'        {$$ = createFuncCallNode($<name>1, $<arg>3);}
    ;

IdUsage : ID                             {$$ = createSyntaxNode(T_ID, none, (data){.varname = $<name>1}, NULL, NULL);}
        | ID '[' expr ']'                {$$ = createSyntaxNode(T_ID, none, (data){.varname = $<name>1}, $<syn>3, NULL);}
        | ID '[' expr ']' '[' expr ']'   {$$ = createSyntaxNode(T_ID, none, (data){.varname = $<name>1}, $<syn>3, $<syn>6);}
        ;

ArgList : ArgList SEP expr      {$$ = pushArgNode($<syn>3, $<arg>1);}
        | expr                  {$$ = pushArgNode($<syn>1, NULL);}
        |                       {$$ = NULL;}
        ;

%%

void yyerror(char const *s) {
    printf("yyerror : %s\n", s);
    exit(1);
}

int main() {
    yyin = fopen("test.txt","r");
    target = fopen("temp.xsm","w");
    yyparse();
    endxsm(target);
    fclose(target);
    translate();
    remove("temp.xsm");
    return 0;
}

