#include <stdio.h>
#include <stdlib.h>

typedef enum 
{
    NPROGRAM,
    NVAR_SECTION,
    NVAR_DECL,
    NID_LIST,
    NTYPE,
    NLIST_TYPE,
    NFUNC_SECTION,
    NFUNC_DECL,
    NOPT_FORMAL_LIST,
    NFORMAL_DECL,
    NAND,
    NOR,
    NEQ,
    NNE,
    NGR,
    NGE,
    NLS,
    NLE,
    NPLUS,
    NMINUS,
    NMULT,
    NDIV,
    NAPP,
    NNEG_MINUS,
    NNOT,
    NOPT_EXPR_LIST,
    NIF_EXPR,
    NINPUT,
    NOUTPUT,
    NFUNC_CALL,
    NBODY_SECTION,
    NSTAT_LIST,
    NSTAT,
    NASSIGN_STAT,
    NSHOW_STAT
} Nonterminal;

typedef enum
{
    T_INT,
    T_STRING,
    T_BOOL,
    T_INTCONST,
    T_BOOLCONST,
    T_STRCONST,
    T_ID,
    T_NONTERMINAL
} Typenode;

typedef enum {
	VARS,
	HALT,
	NEWI,
	NEWS,
	NEWB,
	NEWL,
	LOCI,
	LOCB,
	LOCS,
	LIST,
	LOAD,
	STOR,
	SKPF,
	SKIP,
	EQUA,
	NEQU,
	GRTR,
	GREQ,
	LETH,
	LEEQ,
	PLUS,
	MINU,
	MULT,
	DIVI,
	APPN,
	UMIN,
	NEGA,
	T_HEAD,
	T_TAIL,
	T_EMPT,
	T_LENG,
	PUSH,
	JUMP,
	APOP,
	T_FUNC,
	RETN,
	GETI,
	GETS,
	GETB,
	GETL,
	PUTS,
	T_SHOW
} Operator;

typedef union
{
    int ival;
    char *sval;
} Value;

typedef struct snode
{
    Typenode type;
    Value value;
    /*child, child's-brother(second child), same-line-brother */
    struct snode *p1 , *p2, *p3;
} Node;

typedef Node *Pnode;

char *newstring(char*);

int yylex();

Pnode nontermnode(Nonterminal), 
      idnode(), 
      keynode(Typenode), 
      intconstnode(),
      strconstnode(),
      boolconstnode(),
      newnode(Typenode);
      
void treeprint(Pnode, int), yyerror(), parser();



