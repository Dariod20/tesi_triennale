#include "def.h"

char* tabtypes[] =
{
	"INT",
	"STRING",
	"BOOL",
	"INTCONST",
	"BOOLCONST",
	"STRCONST", 
	"ID",
	"NONTERMINAL"
};

char* tabnonterm[] =
{
	"PROGRAM",
	"VAR_SECTION",
    "VAR_DECL",
    "ID_LIST",
    "TYPE",
    "LIST_TYPE",
    "FUNC_SECTION",
    "FUNC_DECL",
    "OPT_FORMAL_LIST",
    "FORMAL_DECL",
    "AND",
    "OR",
    "EQ",
    "NE",
    ">", /*GR*/
    "GE",
    "<", /*LS*/
    "LE",
    "+", /*PLUS*/
    "-", /*MINUS*/
    "*", /*MULT*/
    "/", /*DIV*/
    "APP",
    "-", /*NEG_MINUS*/
    "NOT",
    "OPT_EXPR_LIST",
    "IF_EXPR",
    "INPUT",
    "OUTPUT",
    "FUNC_CALL",
    "BODY_SECTION",
    "STAT_LIST",
    "STAT",
    "ASSIGN_STAT",
    "SHOW_STAT"
};

void treeConstruction(FILE *fp, Pnode root, int indent)
{
	int i;
	Pnode p;
	Pnode sc; 	
  
	for(i=0; i<indent; i++)
		fprintf(fp, "%s", "    ");
	fprintf(fp, "%s", (root->type == T_NONTERMINAL ? tabnonterm[root->value.ival] : tabtypes[root->type]));
	if(root->type == T_ID || root->type == T_STRCONST)
		fprintf(fp, " (%s)", root->value.sval);
	else if(root->type == T_INTCONST)
		fprintf(fp, " (%d)", root->value.ival);
	else if(root->type == T_BOOLCONST)
		fprintf(fp, " (%s)", (root->value.ival == 1 ? "true" : "false"));
	fprintf(fp, "%s", "\n");
    
    for(p=root->p1; p != NULL; p = p->p3) {
    	sc= root->p2;
    	treeConstruction(fp, p, indent+1);
    	if(sc != NULL)
    		treeConstruction(fp, sc, indent+1);
	}
}

void treeprint(Pnode root, int indent) 
{
	FILE *fp;
	fp = fopen("AbstractTree.txt", "w");
	
	treeConstruction(fp, root, indent);
	
	fclose(fp);
}

