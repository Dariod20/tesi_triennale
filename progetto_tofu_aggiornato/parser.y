%{
#include "def.h"
#define YYSTYPE Pnode
extern char *yytext;
extern Value lexval;
extern int line;
extern FILE *yyin;
Pnode root = NULL;
%}

%token VARIABLES FUNCTIONS IF THEN ELSE END BODY SHOW AND OR EQ NE GE LE 
%token APP NOT FUNC_RET INT STRING BOOL INTCONST STRCONST BOOLCONST ID
%token ERROR

%%

program : var_section func_section body_section {root = $$ = nontermnode(NPROGRAM); $$->p1 = $1;
									    $1->p3 = $2; $2->p3 = $3;}
	   ;

var_section : VARIABLES var_decl_list {$$ = nontermnode(NVAR_SECTION); $$->p1 = $2;}
		  | {$$ = nontermnode(NVAR_SECTION);}
		  ;

var_decl_list : var_decl var_decl_list {$$ = $1; $1->p3 = $2;}
		    | var_decl
		    ;

var_decl : id_list ':' type ';' {$$ = nontermnode(NVAR_DECL); $$->p1 = nontermnode(NID_LIST); $$->p1->p1 = $1; $$->p1->p3 = $3;}
	    ;

id_list : ID {$$ = idnode();} ',' id_list {$$ = $2; $2->p3 = $4;}
	   | ID {$$ = idnode();}
	   ;

type : INT {$$ = nontermnode(NTYPE); $$->p1 = keynode(T_INT);}
	| STRING {$$ = nontermnode(NTYPE); $$->p1 = keynode(T_STRING);}
	| BOOL {$$ = nontermnode(NTYPE); $$->p1 = keynode(T_BOOL);}
	| list_type {$$ = nontermnode(NTYPE); $$->p1 = $1;}
	;

list_type : '[' type ']' {$$ = nontermnode(NLIST_TYPE); $$->p1 = $2;}
		;

func_section : FUNCTIONS func_decl_list {$$ = nontermnode(NFUNC_SECTION); $$->p1 = $2;}
		   | {$$ = nontermnode(NFUNC_SECTION);}
		   ;


func_decl_list : func_decl func_decl_list {$$ = $1; $1->p3 = $2;}
			| func_decl
			;

func_decl : ID {$$ = idnode();} '(' opt_formal_list ')' FUNC_RET type expr ';' {$$ = nontermnode(NFUNC_DECL); 
																$$->p1 = $2; $2->p3 = $4; $4->p3 = $7; 
																$7->p3 = $8;}
		;

opt_formal_list : formal_list {$$ = nontermnode(NOPT_FORMAL_LIST); $$->p1 = $1;}
			 | {$$ = nontermnode(NOPT_FORMAL_LIST);}
		      ;

formal_list : formal_decl ',' formal_list {$$ = $1; $1->p3 = $3;}
		  | formal_decl
		  ;

formal_decl : ID {$$ = idnode();} ':' type {$$ = nontermnode(NFORMAL_DECL); $$->p1 = $2; $$->p2 = $4;}
		  ;


expr : expr bool_op bool_term {$$ = $2; $$->p1 = $1; $$->p2 = $3;}
	| bool_term
	;

bool_op : AND {$$ = nontermnode(NAND);}
	   | OR {$$ = nontermnode(NOR);}
	   ;

bool_term : comp_term comp_op comp_term {$$ = $2; $$->p1 = $1; $$->p2 = $3;}
		| comp_term
	     ;

comp_op : EQ {$$ = nontermnode(NEQ);}
	   | NE {$$ = nontermnode(NNE);}
	   | '>' {$$ = nontermnode(NGR);}
	   | GE {$$ = nontermnode(NGE);}
	   | '<' {$$ = nontermnode(NLS);}
	   | LE {$$ = nontermnode(NLE);}
	   ;

comp_term : comp_term add_op term {$$ = $2; $$->p1 = $1; $$->p2 = $3;}
		| term
		;

add_op : '+' {$$ = nontermnode(NPLUS);}
	  | '-' {$$ = nontermnode(NMINUS);}
	  | APP {$$ = nontermnode(NAPP);}
	  ;

term : term mul_op factor {$$ = $2; $$->p1 = $1; $$->p2 = $3;}
	| factor
	;

mul_op : '*' {$$ = nontermnode(NMULT);}
	  | '/' {$$ = nontermnode(NDIV);}
	  ;

factor : unary_op factor {$$ = $1; $$->p1 = $2;}
	  | '(' expr ')' {$$ = $2;}
	  | ID {$$ = idnode();} {$$ = $2;}
	  | INTCONST {$$ = intconstnode();}
	  | STRCONST {$$ = strconstnode();}
	  | BOOLCONST {$$ = boolconstnode();}
	  | '[' opt_expr_list ']' {$$ = $2;}
	  | if_expr
	  | func_call
	  | input
	  | output
	  ;



unary_op : '-' {$$ = nontermnode(NNEG_MINUS);}
	    | NOT {$$ = nontermnode(NNOT);}
	    ;

opt_expr_list : expr_list {$$ = nontermnode(NOPT_EXPR_LIST); $$->p1 = $1;}
		    | {$$ = nontermnode(NOPT_EXPR_LIST);}
		    ;
expr_list : expr ',' expr_list {$$ = $1; $1->p3 = $3;}
		| expr
		;

if_expr : IF expr THEN expr ELSE expr END {$$ = nontermnode(NIF_EXPR); $$->p1 = $2; $2->p3 = $4; $4->p3 = $6;}
	   ;

input: '?' '(' type ')' {$$ = nontermnode(NINPUT); $$->p1 = $3;}
	;

output: '!' '(' expr ')' {$$ = nontermnode(NOUTPUT); $$->p1 = $3;}
	 ;

func_call : ID {$$ = idnode();} '(' opt_expr_list ')' {$$ = nontermnode(NFUNC_CALL); $$->p1 = $2; $$->p2 = $4;}
		;

body_section : BODY stat_list {$$ = nontermnode(NSTAT_LIST); $$->p1 = $2;}
		   ;

stat_list : stat ';' stat_list {$$ = $1; $1->p3 = $3;}
		| stat ';'
		;

stat : assign_stat
	| show_stat
	;

assign_stat : ID {$$ = idnode();} '=' expr {$$ = nontermnode(NASSIGN_STAT); $$->p1 = $2; $$->p2 = $4;}

show_stat : SHOW expr {$$ = nontermnode(NSHOW_STAT); $$->p1 = $2;}

%%

Pnode nontermnode(Nonterminal nonterm)
{
    Pnode p = newnode(T_NONTERMINAL);
    p->value.ival = nonterm;
    return(p);
}

Pnode idnode()
{
    Pnode p = newnode(T_ID);
    p->value.sval = lexval.sval;
    return(p);
}

Pnode keynode(Typenode keyword)
{
    return(newnode(keyword));
}

Pnode intconstnode()
{
    Pnode p = newnode(T_INTCONST);
    p->value.ival = lexval.ival;
    return(p);
}

Pnode strconstnode()
{
    Pnode p = newnode(T_STRCONST);
    p->value.sval = lexval.sval;
    return(p);
}

Pnode boolconstnode()
{
  Pnode p = newnode(T_BOOLCONST);
  p->value.ival = lexval.ival;
  return(p);
}

Pnode newnode(Typenode tnode)
{
  Pnode p = malloc(sizeof(Node));
  p->type = tnode;
  p->p1 = p->p2 = p->p3 = NULL;
  return(p);
}

void parser() 
{
  yyin = stdin;
  if(yyparse() == 0) {
  	treeprint(root, 0);
  }
  else  {
  	yyerror();
  }
}

void yyerror()
{
  fprintf(stderr, "Line %d: error on symbol \"%s\" \n", line, yytext);
  exit(-1);
}
