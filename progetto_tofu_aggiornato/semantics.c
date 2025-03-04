#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "parser.h"

#define TOT 541
#define TOT_FUNC 89
#define OFFSET 20
#define SHIFT 4
#define UNO 1
#define MENO_UNO -1
#define TRUE 1
#define FALSE 0
#define NOTHING "null"
#define EMPTY "empty"
#define LENGTH "length"
#define HEAD "head"
#define TAIL "tail"

extern Pnode root;

typedef enum {
	D_INT,
	D_STRING,
	D_BOOL,
	D_LIST
} Domain;

typedef enum {
	A_UNKNOWN,
	A_INT,
	A_STRING,
	A_BOOL
} Atomic;

typedef struct {
	Domain domain;
	int depth;
	Atomic atomic;
} Type;

typedef enum {
	VAR,
	FUNC,
	PARAM
} Class;

typedef struct {
	int num;
	struct stable *params;
} Formals;

typedef struct stable {
	char *name;
	Class classe;
	int oid;
	Type tipo;
	struct stable *local;
	Formals formals;
	struct stable *next;
	
} SymbolTable;

SymbolTable symbolTable[TOT];
int varOid, paramOid = 0;
int funcOid = 1;
int funcIndex[TOT_FUNC];
int checkVar = FALSE;
char *msg_error = "Si Inizializza la stringa msg_error";

int hash (char* id, int dim) {
	int h=0;
	for(int i=0; id[i] != '\0'; i++)
		h = ((h << SHIFT) + id[i]) % dim;
	return h;
}

int binarySearch(int n, int num) {
	int i, start = 0, end = n-1;
	do {
		i = (start + end)/2;
		if(i==num)
			return i;
		else if(i < num)
			start = i+1;
		else
			end = i-1;
	} while(start <= end);
	return -1;
}

int maxArray(int array[]) {
	int max = array[0];
	for(int i=1; i < TOT_FUNC && array[i] != MENO_UNO; i++) {
		if(array[i] > max) {
			max = array[i];
		}
	}
	return max;
}

void error(char *msg) {
	printf("%s", msg);
	exit(-1);
}

Domain defineDomain(Pnode p) {
	if(p->type == T_INT || p->type == T_INTCONST) 
		return D_INT;
	else if(p->type == T_STRING || p->type == T_STRCONST)
		return D_STRING;
	else if (p->type == T_BOOL || p->type == T_BOOLCONST)
		return D_BOOL;
	else if(p->value.ival == NLIST_TYPE) 
		return D_LIST;
}

Atomic defineAtomic(Pnode p) {
	if(p->type == T_INT || p->type == T_INTCONST) 
		return A_INT;
	else if(p->type == T_STRING || p->type == T_STRCONST)
		return A_STRING;
	else if (p->type == T_BOOL || p->type == T_BOOLCONST)
		return A_BOOL;
	else
		return A_UNKNOWN;
}

Atomic fromDomainToAtomic(Domain d) {
	if(d == D_INT) 
		return A_INT;
	else if(d == D_STRING)
		return A_STRING;
	else if (d == D_BOOL)
		return A_BOOL;
}

Domain fromAtomicToDomain(Atomic a) {
	if(a == A_INT) 
		return D_INT;
	else if(a == A_STRING)
		return D_STRING;
	else if (a == A_BOOL)
		return D_BOOL;
}

Type defineType(Pnode p) {
	Type type;
	type.domain = defineDomain(p);
	type.depth = 0;
	if(type.domain == D_LIST) {
		while (p->value.ival == NLIST_TYPE) {
			type.depth++;
			p = p->p1->p1;
		}
		type.atomic = defineAtomic(p);
	} else {
		type.atomic = A_UNKNOWN;
	}
	return type;
}

void insert(SymbolTable *sTable, char *name, Class classe, int oid, Type tipo, struct stable *local, int num, 
			struct stable *params, struct stable *next) {
	sTable->name = name;
	sTable->classe = classe;
	sTable->oid = oid;
	sTable->tipo = tipo;
	sTable->local = local;
	sTable->formals.num = num;
	sTable->formals.params = params;
	sTable->next = next;
}

SymbolTable *buildLocal(int length) {
	struct {
		SymbolTable *st;
	} Local;

	Local.st = malloc(length * sizeof (SymbolTable));
	for(int i=0; i < length; i++) {
		Local.st[i].name = NULL;
	}
	return Local.st;
}

int numParam(Pnode param) {
	int num = 0;
	while(param != NULL) {
		num++;
		param = param->p3;
	}
	return num;
}

void whereInsert(SymbolTable sTable[], int index, char *name, Class c, Type t) {
	if(sTable[index].name == NULL) {
		if(c == VAR) {
			insert(&sTable[index], name, c, varOid, t, NULL, MENO_UNO, NULL, buildLocal(UNO));
			varOid++;
		} else {
			insert(&sTable[index], name, c, paramOid, t, NULL, MENO_UNO, NULL, buildLocal(UNO));
		}
		sTable[index].next->name = NOTHING;
	} else {
		if(c == VAR) {
			insert(sTable[index].next, name, c, varOid, t, NULL, MENO_UNO, NULL, buildLocal(UNO));
			varOid++;
		} else {
			insert(sTable[index].next, name, c, paramOid, t, NULL, MENO_UNO, NULL, buildLocal(UNO));
		}
		sTable[index].next->next->name = NOTHING;
	}
}

void whereInsertFunc(SymbolTable sTable[], int index, char *name, Class c, Type t, int numParam) {
	if(sTable[index].name == NULL) {
		insert(&sTable[index], name, c, funcOid, t, buildLocal(TOT_FUNC), numParam, buildLocal(numParam), buildLocal(UNO));
		sTable[index].next->name = NOTHING;
		funcOid++;
	} else {
		insert(sTable[index].next, name, c, funcOid, t, buildLocal(TOT_FUNC), numParam, buildLocal(numParam), buildLocal(UNO));
		sTable[index].next->next->name = NOTHING;
		funcOid++;
	}
}

char* describeClass(Class c) {
	char *nome_classe;
	if(c == VAR) 
		nome_classe = "VARIABLE";
	else if(c == FUNC)
		nome_classe = "FUNCTION";
	else 
		nome_classe = "PARAMETER";
			
	return nome_classe;
}

char* describeDomain(Domain d) {
	char *nome_dominio;
	if(d == D_INT) 
		nome_dominio = "INT";
	else if(d == D_STRING)
		nome_dominio = "STRING";
	else if(d == D_BOOL)
		nome_dominio = "BOOL";
	else 
		nome_dominio = "LIST";	
		
	return nome_dominio;
}

char* describeAtomic(Atomic a) {
	char *nome_atomico;
	if(a == A_INT) 
		nome_atomico = "INT";
	else if(a == A_STRING)
		nome_atomico = "STRING";
	else if(a == A_BOOL)
		nome_atomico = "BOOL";
	else 
		nome_atomico = "UNKNOWN";
		
	return nome_atomico;
}

int lookup(char *id, SymbolTable st[], int dim) {
	int ok = FALSE;
	int h = binarySearch(dim, hash(id, dim));
	if(st[h].name != NULL && strcmp(st[h].name, id) == 0) {
		ok = TRUE;	
	}
	return ok;
}

void domainEqual(Domain d1, Domain d2) {
	if(d1 != d2) {
		printf("%s", msg_error);
		fprintf(stderr, "Error: incompatible types; must be all %s or %s \n", describeDomain(d1), describeDomain(d2));
  		exit(-1);
	} 
}

void typeEqual(Type t1, Type t2) {
	if(t1.domain != t2.domain) {
		printf("%s", msg_error);
		fprintf(stderr, "Error: incompatible types; must be all %s or %s \n", describeDomain(t1.domain), describeDomain(t2.domain));
  		exit(-1);
	} 
	else if(t1.domain == D_LIST) {
		if(t1.atomic != A_UNKNOWN && t2.atomic != A_UNKNOWN && t1.depth != t2.depth) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: incompatible list types; must both have depth %d or %d \n", t1.depth, t2.depth);
			exit(-1);
		} 
		else if(t2.atomic == A_UNKNOWN && t1.atomic != A_UNKNOWN && t2.depth > t1.depth) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: incompatible list types; expected depth <= %d instead of %d\n", t1.depth, t2.depth);
			exit(-1);
		} 
		else if(t1.atomic == A_UNKNOWN && t2.atomic != A_UNKNOWN && t1.depth > t2.depth) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: incompatible list types; expected something <= %d instead of %d as depth \n", t2.depth, t1.depth);
			exit(-1);
		} 
		else if(t1.atomic != A_UNKNOWN && t2.atomic != A_UNKNOWN && t1.atomic != t2.atomic) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: incompatible atomic list types; must be all %s or %s \n", describeAtomic(t1.atomic), describeAtomic(t2.atomic));
  			exit(-1);
		}
	}
}

void builtinFuncStruct(char *name, Type tipo) {
	Type param = {D_LIST, MENO_UNO, A_UNKNOWN};
	int i = binarySearch(TOT, hash(name, TOT));
	whereInsertFunc(symbolTable, i, name, FUNC, tipo, UNO);
	int local_i = binarySearch(TOT_FUNC, hash(name, TOT_FUNC));
	whereInsert(symbolTable[i].local, local_i, "param1", PARAM, param);
	symbolTable[i].formals.params[0] = symbolTable[i].local[local_i];
}

void insertBuiltinFunc() {
	Type e_tipo = {D_BOOL, 0, A_UNKNOWN};
	builtinFuncStruct(EMPTY, e_tipo);
	
	Type l_tipo = {D_INT, 0, A_UNKNOWN};
	builtinFuncStruct(LENGTH, l_tipo);
	
	Type h_tipo = {D_LIST, UNO, A_UNKNOWN};
	builtinFuncStruct(HEAD, h_tipo);
	
	Type t_tipo = {D_LIST, UNO, A_UNKNOWN};
	builtinFuncStruct(TAIL, t_tipo);
}

int isBuiltinFunc(char *name) {
	int check = FALSE;
	
	if(strcmp(name,EMPTY)==0) {
		check = TRUE;
	}
	else if(strcmp(name,LENGTH)==0) {
		check = TRUE;
	}
	else if(strcmp(name,HEAD)==0) {
		check = TRUE;
	}
	else if(strcmp(name,TAIL)==0) {
		check = TRUE;
	}
	return check;
}

void checkBuiltinFunc(char *name, Type p_tipo) {
	Type expected_p_tipo;
	int i;
	
	if(strcmp(name,EMPTY)==0) {
		i = binarySearch(TOT, hash(name, TOT));
		expected_p_tipo = symbolTable[i].formals.params[0].tipo;
		domainEqual(expected_p_tipo.domain, p_tipo.domain);
	}
	else if(strcmp(name,LENGTH)==0) {
		i = binarySearch(TOT, hash(name, TOT));
		expected_p_tipo = symbolTable[i].formals.params[0].tipo;
		domainEqual(expected_p_tipo.domain, p_tipo.domain);
	}
	else if(strcmp(name,HEAD)==0) {
		i = binarySearch(TOT, hash(name, TOT));
		expected_p_tipo = symbolTable[i].formals.params[0].tipo;
		domainEqual(expected_p_tipo.domain, p_tipo.domain);
		if(p_tipo.depth == UNO) {
			symbolTable[i].tipo.domain = fromAtomicToDomain(p_tipo.atomic);
			symbolTable[i].tipo.depth = 0;
			symbolTable[i].tipo.atomic = A_UNKNOWN;
		}
		else {
			symbolTable[i].tipo = p_tipo;
			symbolTable[i].tipo.depth--;
		}
	}
	else if(strcmp(name,TAIL)==0) {
		i = binarySearch(TOT, hash(name, TOT));
		expected_p_tipo = symbolTable[i].formals.params[0].tipo;
		domainEqual(expected_p_tipo.domain, p_tipo.domain);
		symbolTable[i].tipo = p_tipo;
	}
}

Type idVarType(char *id) {
	Type t;	
	if(lookup(id, symbolTable, TOT) == TRUE) {
		int h = binarySearch(TOT, hash(id, TOT));
		t = symbolTable[h].tipo;
	}
	else {
		printf("%s", msg_error);
		fprintf(stderr, "Error: identifier \"%s\" is an undeclared variable\n", id);
  		exit(-1);
	}
	return t;
}

Type idParamType(char *id) {
	Type t;	
	int func_pos = funcIndex[funcOid-6];
	if(lookup(id, symbolTable[func_pos].local, TOT_FUNC) == TRUE) {
		int h = binarySearch(TOT_FUNC, hash(id, TOT_FUNC));
		t = symbolTable[func_pos].local[h].tipo;			
	} 
	else {
		printf("%s", msg_error);
		fprintf(stderr, "Error: identifier \"%s\" is an undeclared parameter \n", id);
  		exit(-1);
	}
	return t;
}

Type simpleExprType(Pnode expr) {
	Type t;
	if(expr->type == T_ID) {
		if(checkVar == FALSE)
			t = idParamType(expr->value.sval);
		else
			t = idVarType(expr->value.sval);
	}
	else
		t = defineType(expr);
	return t;
}

Type exprType(Pnode expr) {
	Type t1;
	Type t2;
	if(expr->type != T_NONTERMINAL) {
		t1 = simpleExprType(expr);
	}
	else if(expr->value.ival == NAND || expr->value.ival == NOR) {
		t1 = exprType(expr->p1);	
		t2 = exprType(expr->p2);
		if(t1.domain != D_BOOL) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: expected a BOOL type and not %s type \n", describeDomain(t1.domain));
  			exit(-1);
		}
		if(t2.domain != D_BOOL) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: expected a BOOL type and not %s type \n", describeDomain(t2.domain));
  			exit(-1);
		}
	}
	else if(expr->value.ival == NEQ || expr->value.ival == NNE) {
		t1 = exprType(expr->p1);	
		t2 = exprType(expr->p2);
		typeEqual(t1, t2);
		t1.domain = D_BOOL;
	} 
	else if(expr->value.ival == NGR || expr->value.ival == NGE || expr->value.ival == NLS || expr->value.ival == NLE) {
		t1 = exprType(expr->p1);	
		t2 = exprType(expr->p2);
		if(t1.domain != D_INT && t1.domain != D_STRING) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: expected a INT or STRING type and not %s type \n", describeDomain(t1.domain));
  			exit(-1);
		}
		if(t2.domain != D_INT && t2.domain != D_STRING) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: expected a INT or STRING type and not %s type \n", describeDomain(t2.domain));
  			exit(-1);
		}
		typeEqual(t1, t2);
		t1.domain = D_BOOL;
	}
	else if(expr->value.ival == NPLUS || expr->value.ival == NMINUS || expr->value.ival == NMULT || expr->value.ival == NDIV) {
		t1 = exprType(expr->p1);	
		t2 = exprType(expr->p2);
		if(t1.domain != D_INT) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: expected a INT type and not %s type \n", describeDomain(t1.domain));
  			exit(-1);
		}
		if(t2.domain != D_INT) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: expected a INT type and not %s type \n", describeDomain(t2.domain));
  			exit(-1);
		}
	}
	else if(expr->value.ival == NAPP) {
		t1 = exprType(expr->p1);
		t2 = exprType(expr->p2);
		if(t1.domain != D_LIST) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: expected a LIST type and not %s type \n", describeDomain(t1.domain));
  			exit(-1);
		}
		if(t2.domain != D_LIST) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: expected a LIST type and not %s type \n", describeDomain(t2.domain));
  			exit(-1);
		}
		typeEqual(t1, t2);
	}
	else if(expr->value.ival == NNEG_MINUS) {
		t1 = exprType(expr->p1);	
		if(t1.domain != D_INT) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: expected a INT type and not %s type \n", describeDomain(t1.domain));
  			exit(-1);
		}
	}
	else if(expr->value.ival == NNOT) {
		t1 = exprType(expr->p1);	
		if(t1.domain != D_BOOL) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: expected a BOOL type and not %s type \n", describeDomain(t1.domain));
  			exit(-1);
		}
	}
	else if(expr->value.ival == NIF_EXPR) {
		Pnode if_node = expr->p1;
		t1 = exprType(if_node);
		if(t1.domain != D_BOOL) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: expected a BOOL type and not %s type \n", describeDomain(t1.domain));
  			exit(-1);
		}
		if_node = if_node->p3;
		t1 = exprType(if_node);
		if_node = if_node->p3;
		t2 = exprType(if_node);
		typeEqual(t1, t2);
	}
	else if(expr->value.ival == NFUNC_CALL) {
		Pnode func_call = expr->p1;
		char *id = func_call->value.sval;
		int i = binarySearch(TOT, hash(id, TOT));
		if(lookup(id, symbolTable, TOT) == FALSE || symbolTable[i].classe != FUNC) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: identifier \"%s\" is an undeclared function \n", id);
  			exit(-1);
		}
		int indexParams = 0;
		int builtin = isBuiltinFunc(id);
		for(func_call = expr->p2->p1; func_call != NULL; func_call = func_call->p3) {
			if(builtin == TRUE) {
				checkBuiltinFunc(id, exprType(func_call));
				indexParams++;
			}
			else {
				t1 = exprType(func_call);
				t2 = symbolTable[i].formals.params[indexParams++].tipo;
				typeEqual(t1, t2);
			}
		}
		if(symbolTable[i].formals.num != indexParams) {
			fprintf(stderr, "Error: number of parameters expected for the function \"%s\" is %d and not %d \n", id, symbolTable[i].formals.num, indexParams);
  			exit(-1);
		}
		t1 = symbolTable[i].tipo;
	}
	else if(expr->value.ival == NINPUT) {
		t1 = defineType(expr->p1->p1);	
	}
	else if(expr->value.ival == NOUTPUT || expr->value.ival == NSHOW_STAT) {
		t1 = exprType(expr->p1);	
	}
	else if(expr->value.ival == NOPT_EXPR_LIST) {
		Pnode opt_list = expr->p1;
		Type opt_list_type [TOT_FUNC];
		int j = 0;
		t1.domain = D_LIST;
		int arg_list_depth [TOT_FUNC];
		
		if(opt_list == NULL) {
			t1.atomic = A_UNKNOWN;
			t1.depth = 1;
			return t1;
		} else {
			for(opt_list; opt_list != NULL; opt_list = opt_list->p3) {
				opt_list_type[j++] = exprType(opt_list);
			}
			opt_list_type[j].depth = -1;
			
			for(j=0; j < TOT_FUNC && opt_list_type[j].depth != MENO_UNO; j++) {
				for(int i=j+1; i < TOT_FUNC && opt_list_type[i].depth != MENO_UNO; i++) {
					typeEqual(opt_list_type[j], opt_list_type[i]);
				}
				arg_list_depth[j] = opt_list_type[j].depth;
			}
		arg_list_depth[j] = MENO_UNO;		
		}
		for(j=0; j < TOT_FUNC && opt_list_type[j].depth != MENO_UNO; j++) {
			if(opt_list_type[j].domain == D_LIST) {
				t1.atomic = opt_list_type[j].atomic;
				if(t1.atomic != A_UNKNOWN)
					break;
			}
			else {
				t1.atomic = fromDomainToAtomic(opt_list_type[j].domain);
				break;
			}
		}
		t1.depth = maxArray(arg_list_depth) + 1;
	}
	else if(expr->value.ival == NASSIGN_STAT) {
		char *id = expr->p1->value.sval;
		if(lookup(id, symbolTable, TOT) == FALSE) {
			printf("%s", msg_error);
			fprintf(stderr, "Error: identifier \"%s\" is an undeclared variable \n", id);
  			exit(-1);
		}
		int index = binarySearch(TOT, hash(id, TOT));
		t1 = symbolTable[index].tipo;
		t2 = exprType(expr->p2);
		typeEqual(t1,t2);
	}
	else {
		error("Unknown yacc instruction \n");
	}
	
	return t1;
}

void insertVariable(Pnode pnode) {
	for(Pnode p = pnode->p1; p != NULL; p = p->p3) {
		Pnode var = p->p1->p1;
		Pnode t = p->p1->p3;
		for(var; var != NULL; var = var->p3) {
			char *name = var->value.sval;
			if(lookup(name, symbolTable, TOT) == FALSE) {
				Type tipo = defineType(t->p1);
				int index = binarySearch(TOT, hash(name, TOT));
				whereInsert(symbolTable, index, name, VAR, tipo);
			}
			else {
				fprintf(stderr, "Error: identifier \"%s\" is a duplicate variable \n", name);
  				exit(-1);
			}
		}
	}
}

int insertFunction(Pnode f, Pnode param) {
	int index;
	char *name = f->value.sval;
	msg_error = malloc(sizeof(msg_error)+OFFSET);
	sprintf(msg_error, "In the function %s: \n", name);
	Pnode type = f->p3->p3->p1;
	if(lookup(name, symbolTable, TOT) == FALSE) {
		Type tipo = defineType(type);
		index = binarySearch(TOT, hash(name, TOT));
		int numParameters = numParam(param);
		whereInsertFunc(symbolTable, index, name, FUNC, tipo, numParameters);
	}
	else {
		fprintf(stderr, "Error: identifier \"%s\" is a duplicate function or it has been declared as a variable \n", name);
  		exit(-1);
	}
	return index;
}

void insertParameter(Pnode param, int j) {
	char *name = param->p1->value.sval;
	int func_pos = funcIndex[funcOid-6];
	if(lookup(name, symbolTable[func_pos].local, TOT_FUNC) == FALSE) {
		Type tipo = defineType(param->p2->p1);
		int local_index = binarySearch(TOT_FUNC, hash(name, TOT_FUNC));
		whereInsert(symbolTable[func_pos].local, local_index, name, PARAM, tipo);
			
		if(strcmp(symbolTable[func_pos].local[local_index].next->name, NOTHING) != 0) {
			symbolTable[func_pos].formals.params[j] = *symbolTable[func_pos].local[local_index].next;
		} else {
			symbolTable[func_pos].formals.params[j] = symbolTable[func_pos].local[local_index];
		}	
		paramOid++;
	}
	else {
		printf("%s", msg_error);
		fprintf(stderr, "Error: identifier \"%s\" is a duplicate parameter \n", name);
  		exit(-1);
	}
}

int createAndCheckSymbolTable() {
	Pnode pnode;
	char *id;
	int func_pos = 0;
	insertBuiltinFunc();
	
	pnode = root->p1;
	insertVariable(pnode);
	
	pnode = pnode->p3;
	for(Pnode p = pnode->p1; p != NULL; p = p->p3) {
		Pnode f = p->p1;
		Pnode param = f->p3->p1;
		int j = 0;
		funcIndex[func_pos++] = insertFunction(f, param);
		
		for(param; param != NULL; param = param->p3) {
			insertParameter(param, j++);
		}
		paramOid = 0;
	}
	
	func_pos = 0;
	funcOid = 5;
	for(Pnode p = pnode->p1; p != NULL; p = p->p3) {
		funcOid++;
		Pnode expr = p->p1->p3->p3->p3;
		Type eType = exprType(expr);
		Type fType = symbolTable[funcIndex[func_pos++]].tipo;
		typeEqual(fType, eType);
	}
	
	pnode = pnode->p3;
	checkVar = TRUE;
	int count = 1;
	for(Pnode body = pnode->p1; body != NULL; body = body->p3) {
		msg_error = malloc(sizeof(msg_error)+OFFSET);
		sprintf(msg_error, "In the statement number %d: \n", count++);
		Type eType = exprType(body);
	}
	return 0;
}

void printElement(FILE *fp, SymbolTable st, int i) {
	if(isBuiltinFunc(st.name)) {
		fprintf(fp, "Nome: %s, Classe: %s, Oid:%d, Numero_Parametri: %d \n\n", st.name, describeClass(st.classe), st.oid, st.formals.num);
		return;
	}
	fprintf(fp, "Nome: %s, Classe: %s, Tipo: (Dominio: %s, Profondita: %d, Tipo_Atomico: %s), Oid:%d", st.name, describeClass(st.classe), 
		describeDomain(st.tipo.domain), st.tipo.depth, describeAtomic(st.tipo.atomic), st.oid);
	if(st.classe == FUNC) {
		fprintf(fp, ", Numero_Parametri: %d \n\nParametri della funzone: \n", st.formals.num);
		/*if(st.formals.num == 0) {
			fprintf(fp, %s, "nessuno\n");
		} else {
			for(int j=0; j < st.formals.num; j++) {
				printElement(fp, st.formals.params[j], j);
			}
		}*/
		//vanno bene entrambi per stampare i parametri!!
		if(st.formals.num == 0) {
			fprintf(fp, "%s", "nessuno\n");
		} else {
			for(int i=0; i < TOT_FUNC; i++) {
				if(st.local[i].name != NULL) {
					fprintf(fp, "\nContenuto della Local Symbol Table nel posto %d \n", i);
					printElement(fp, st.local[i], i);
				}
			}
		}
		fprintf(fp, "%s", "\n");
	} else {
		fprintf(fp, "%s", "\n\n");
	}
	if(strcmp(st.next->name, NOTHING) != 0 /*&& st.classe != PARAM*/) {
		fprintf(fp, "Descrittore collegato al posto %d della Symbol Table\n", i);
		printElement(fp, *st.next, i);
	}
	
}

void semantics() {
	parser();
	
	createAndCheckSymbolTable();
	
	FILE *fp;  
	fp = fopen("SymbolTable.txt", "w"); 	
	
	for(int i=0; i < TOT; i++) {
		if(symbolTable[i].name != NULL) {
			fprintf(fp, "Contenuto Symbol Table nel posto %d \n", i);
			printElement(fp, symbolTable[i], i);
		}
	}
	fclose(fp);
}

