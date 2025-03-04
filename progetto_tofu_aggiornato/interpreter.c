#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"

#define MAX_ARGS 2
#define MAX_IP_OBJECTS 5000
#define MAX_OBJECTS 150
#define STRING_DIM 100
#define TRUE 1
#define FALSE 0

typedef struct {
	Operator oper;
	Value args[MAX_ARGS];
} Tcode;

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
	Domain type;
	int leng;
	Value inst;	
} Object;

typedef struct {
	int num;
	int objs;
	int ret;
} Aobject;

typedef struct {
	char *input;
} ArgsMain;

Tcode *prog;
int size;
int ap = -1;
int pc, vp, op, ip, pp;
Object *vars;
Object ostack[MAX_OBJECTS], istack[MAX_IP_OBJECTS], pstack[MAX_OBJECTS];
Aobject astack[MAX_OBJECTS];

void inizializeObject(Object stack[], int dim) {
	for(int j=0; j < dim; j++) {
		stack[j].leng = -1;
	}
}

void error(char *msg) {
	printf("%s", msg);
	exit(-1);
}

void checkip() {
	while(istack[ip].leng != -1) {
		ip++;
	}
}

void push_obj(Object obj) {
	if(op >= MAX_OBJECTS) 
		error("Object stack overflow \n");
		
	ostack[op++] = obj;
}

Object pop_obj() {
	return ostack[--op];
}

void push_int(int n) {
	Object obj;
	
	obj.type = D_INT;
	obj.leng = 0;
	obj.inst.ival = n;
	push_obj(obj);
}

void push_string(char *str) {
	Object obj;
	
	obj.type = D_STRING;
	obj.leng = 0;
	obj.inst.sval = malloc(strlen(str));
	strcpy(obj.inst.sval,str);
	
	push_obj(obj);
}

void push_bool(int b) {
	Object obj;
	
	obj.type = D_BOOL;
	obj.leng = 0;
	obj.inst.ival = b;
	push_obj(obj);
}

void push_list(int leng) {
	Object obj;
	
	obj.type = D_LIST;
	obj.leng = leng;
	if(leng == 0)
		obj.inst.ival = -1;
	else
		obj.inst.ival = ip;
	
	op = op - obj.leng;
		
		for(int j=0; j < obj.leng; j++) {
			istack[ip++] = ostack[op++];
		}
		op = op - obj.leng;
	
	push_obj(obj);
}

Object pop_list() {
	Object obj;
	
	obj = pop_obj();
	ip = obj.inst.ival;
	return obj;
}

void exec_loci(int n) {
	push_int(n);
}

void exec_locb(int b) {
	push_bool(b);
}

void exec_locs(char *s) {
	push_string(s);
}

void exec_list(int leng) {
	push_list(leng);
		
}

void exec_vars(int num_vars) {
	vars = malloc(num_vars*sizeof(Object));
	size = num_vars;
}

void exec_newi() {
	vars[vp].type = D_INT;
	vars[vp].leng = 0;
	vars[vp].inst.ival = -1;
	vp++;
}

void exec_newb() {
	vars[vp].type = D_BOOL;
	vars[vp].leng = 0;
	vars[vp].inst.ival = -1;
	vp++;
}

void exec_news() {
	vars[vp].type = D_STRING;
	vars[vp].leng = 0;
	vars[vp].inst.sval = "null";
	vp++;
}

void exec_newl() {
	vars[vp].type = D_LIST;
	vars[vp].leng = 0;
	vars[vp].inst.ival = -1;
	vp++;
}

void exec_load(int flag, int oid) {
	if(flag == 0) {
		push_obj(vars[oid]);
	}
	else {
		int current_param = pp - astack[ap].num;
		push_obj(pstack[current_param + oid]);
	}
}

void exec_stor(int oid) {
	vars[oid] = pop_obj();
}

void exec_skip(int offset) {
	pc += offset-1;
}

void exec_skpf(int offset) {
	if(pop_obj().inst.ival == FALSE)
		pc += offset-1;
}

int equal(Object obj1, Object obj2) {
	switch(obj1.type) {
		case D_BOOL:
		case D_INT: 
			return (obj1.inst.ival == obj2.inst.ival);
		case D_STRING:
			return (strcmp(obj1.inst.sval, obj2.inst.sval) == 0);
		case D_LIST:
			if(obj1.leng != obj2.leng)
				return FALSE;
			for(int i=0; i < obj1.leng; i++) {
				if(!equal(istack[obj1.inst.ival + i], istack[obj2.inst.ival + i])) {
					return FALSE;
				}
			}
			return TRUE;
	}
}

void exec_equa() {
	Object obj1, obj2;
	
	obj2 = pop_obj();
	obj1 = pop_obj();
	push_bool(equal(obj1, obj2));
	if(obj1.type == D_LIST)
		ip = obj2.inst.ival + obj2.leng;
}

void exec_nequ() {
	Object obj1, obj2;
	
	obj2 = pop_obj();
	obj1 = pop_obj();
	push_bool(!equal(obj1, obj2));
	if(obj1.type == D_LIST)
		ip = obj2.inst.ival + obj2.leng;
}

void exec_grtr() {
	Object obj1, obj2;
	
	obj2 = pop_obj();
	obj1 = pop_obj();
	if(obj1.type == D_INT) 
		push_bool(obj1.inst.ival > obj2.inst.ival);
	else
		push_bool(strcmp(obj1.inst.sval, obj2.inst.sval) > 0);
	
}

void exec_greq() {
	Object obj1, obj2;
	
	obj2 = pop_obj();
	obj1 = pop_obj();
	if(obj1.type == D_INT) 
		push_bool(obj1.inst.ival >= obj2.inst.ival);
	else
		push_bool(strcmp(obj1.inst.sval, obj2.inst.sval) >= 0);
	
}

void exec_leth() {
	Object obj1, obj2;
	
	obj2 = pop_obj();
	obj1 = pop_obj();
	if(obj1.type == D_INT) 
		push_bool(obj1.inst.ival < obj2.inst.ival);
	else
		push_bool(strcmp(obj1.inst.sval, obj2.inst.sval) < 0);
	
}

void exec_leeq() {
	Object obj1, obj2;
	
	obj2 = pop_obj();
	obj1 = pop_obj();
	if(obj1.type == D_INT) 
		push_bool(obj1.inst.ival <= obj2.inst.ival);
	else
		push_bool(strcmp(obj1.inst.sval, obj2.inst.sval) <= 0);
	
}

void exec_plus() {
	int n,m;
	
	n = pop_obj().inst.ival;
	m = pop_obj().inst.ival;
	push_int(m+n);
}

void exec_minu() {
	int n,m;
	
	n = pop_obj().inst.ival;
	m = pop_obj().inst.ival;
	push_int(m-n);
}

void exec_mult() {
	int n,m;
	
	n = pop_obj().inst.ival;
	m = pop_obj().inst.ival;
	push_int(m*n);
}

void exec_divi() {
	int n,m;
	
	n = pop_obj().inst.ival;
	m = pop_obj().inst.ival;
	push_int(m/n);
}

void exec_app() {
	Object obj1, obj2, obj;
	
	obj2 = pop_list();
	obj1 = pop_list();
	obj.type = D_LIST;
	obj.leng = obj1.leng + obj2.leng;
	
	if(obj1.leng == 0) {
		ip = obj2.inst.ival + obj2.leng;
		checkip();
		push_obj(obj2);
		return;
	}
	if(obj2.leng == 0) {
		ip = obj1.inst.ival + obj1.leng;
		checkip();
		push_obj(obj1);
		return;
	}
	
	checkip();
	int abs_ip = ip;
	obj.inst.ival = abs_ip;
	
	ip = obj1.inst.ival;
	for(int i=0; i < obj1.leng; i++) {
		istack[abs_ip++] = istack[ip+i];
	}
	
	ip = obj2.inst.ival;
	for(int i=0; i < obj2.leng; i++) {
		istack[abs_ip++] = istack[ip+i];
	}
	
	ip = abs_ip;
	push_obj(obj);
}

void exec_umin() {
	int n;
	
	n = pop_obj().inst.ival;
	push_int(-n);
}

void exec_nega() {
	int b;
	
	b = pop_obj().inst.ival;
	if(b == TRUE)
		b = FALSE;
	else
		b = TRUE;
	push_bool(b);
}

void exec_empt() {
	Object obj;
	int abs_ip = ip;
	
	obj = pop_list();
	push_bool(obj.leng==0);

	ip = abs_ip;
}

void exec_leng() {
	Object obj;
	int abs_ip = ip;
	
	obj = pop_list();
	push_int(obj.leng);
	
	ip = abs_ip;
}

void exec_head() {
	Object obj, h;
	
	obj = pop_list();
	if(obj.leng==0)
		error("Function head applied to empty list \n");
	
	h = istack[obj.inst.ival];
	push_obj(h);
	
	ip = obj.leng + obj.inst.ival;
	checkip();
}

void exec_tail() {
	Object obj, t, h;
	
	obj = pop_list();
	if(obj.leng==0)
		error("Function tail applied to empty list \n");
	
	t = obj;
	t.inst.ival = obj.inst.ival + 1;
	t.leng = obj.leng - 1;
	h = istack[obj.inst.ival];
	push_obj(t);

	ip = obj.leng + obj.inst.ival;
	checkip();
}

void exec_push(int params) {
	astack[++ap].num = params;
	astack[ap].objs = op - astack[ap].num;
	for(int i=0; i < params; i++)
		pstack[pp++] = ostack[astack[ap].objs + i];
	
	op = astack[ap].objs;
}

void exec_jump(int address) {
	astack[ap].ret = pc;
	pc = address;
}

void exec_retn() {	
	pc = astack[ap].ret;
}

void exec_apop() {
	pp = pp - astack[ap--].num;
}

void exec_geti() {
	int input;
	scanf("%d", &input);
	push_int(input);
}

void exec_gets() {
	char input[STRING_DIM];
	fgets(input,STRING_DIM,stdin);
	push_string(input);
}

void exec_getb() {
	int b;
	char input[STRING_DIM];
	scanf("%s", input);
	if(strcmp("true", input) == 0)
		b = 1;
	else if(strcmp("false", input) == 0)
		b = 0;
	else
		error("Attention: wrong boolean format\n");
	push_bool(b);
}

void push_atomic(int atomic, char s[]) {
	switch(atomic) {
		case A_INT: {
			int n = atoi(s);
			if(n == 0 && strcmp(s, "0")!=0) {
					fprintf(stderr, "Attention: \"%s\" is not an int element in the list inserted\n", s);
					exit(-1);
			}
			else
				push_int(n);
			break;
		}
		case A_STRING: {
			push_string(s);
			break;
		}
		case A_BOOL: {
			int b;
			if(strcmp("true", s) == 0)
				b = 1;
			else if(strcmp("false", s) == 0)
				b = 0;
			else {
				fprintf(stderr, "Attention: \"%s\" is a not boolean element in the list inserted\n", s);
				exit(-1);
			}	
			push_bool(b);
			break;
		}
	}
}

int k = 1;
int total_depth;

void getl(int depth, int atomic, char input_list[], char elem[]) {
	int leng = 0;
	for(int j=0; j < STRING_DIM; j++) {
		elem[j] = '\0';
	}
	if(depth == 1) {
		int i = 0;
		while(input_list[k] != ']') {
			if(input_list[k] != ',')
				elem[i++] = input_list[k];
			else {
				push_atomic(atomic, elem);
				i = 0;
				leng++;
				for(int j=0; j < STRING_DIM; j++)
					elem[j] = '\0';
			}
			k++;
		}
		if(elem[0] != '\0') {
			push_atomic(atomic, elem);
			leng++;
			for(int j=0; j < STRING_DIM; j++)
				elem[j] = '\0';
		}
		k++;
		push_list(leng);
	}
	else {
		while(TRUE) {
			if(input_list[k+depth-3] == ']' && input_list[k+depth-2] == ']') {
				break;
			}
			if(input_list[k] == ',') {
				k++;
			}
			k++;
			if(input_list[k] == ',') {
				k=k+total_depth-1;
			}
			leng++;
			getl(depth-1, atomic, input_list, elem);
		}
		push_list(leng);
	}
}

void exec_getl(int depth, int atomic) {
	char input_list[STRING_DIM];
	scanf("%s", input_list);
	char elem[STRING_DIM];
	total_depth = depth;
	getl(depth, atomic, input_list, elem);
	k = 1;
}

void exec_inst_puts_show() {
	Object instance = istack[ip++];
	
	if(instance.type == D_LIST) {
		int next_ip = ip;
		ip = instance.inst.ival;
		printf("[");
		for(int j=0; j < instance.leng; j++) {
			exec_inst_puts_show();
			if(j != instance.leng-1)
				printf(",");
		}
		printf("]");
		ip = next_ip;
	}
	else {
		if(instance.type == D_STRING)
			printf("%s", instance.inst.sval);
		else if(instance.type == D_INT)
			printf("%d", instance.inst.ival);
		else 
			printf("%s", instance.inst.ival == 1 ? "true" : "false");
	}
}

void exec_puts_show() {
	int abs_ip = ip;
	Object obj = pop_obj();
	
	if(obj.type == D_LIST) {
		op++;
		obj = pop_list();
		printf("[");
		for(int j=0; j < obj.leng; j++) {
			exec_inst_puts_show();
			if(j != obj.leng-1)
				printf(",");
		}
		printf("]");
		ip = abs_ip;
	}
	else {
		if(obj.type == D_STRING) {
			printf("%s", obj.inst.sval);
		}
		else if(obj.type == D_INT)
			printf("%d", obj.inst.ival);
		else
			printf("%s", obj.inst.ival == 1 ? "true" : "false");
	}
	
	if((&prog[pc-1])->oper == PUTS)
		op++;
}

void execute(Tcode *tstat) {
	switch(tstat->oper) {
		case VARS: 
			exec_vars(tstat->args[0].ival);
			break;
		case NEWI:
			exec_newi();
		 	break;
		case NEWS: 
			exec_news();
			break;
		case NEWB:
			exec_newb();
			break;
		case NEWL: 
			exec_newl();
			break;
		case LOCI:
			exec_loci(tstat->args[0].ival);
			break;
		case LOCB:
			exec_locb(tstat->args[0].ival);
			break;
		case LOCS:
			exec_locs(tstat->args[0].sval);
			break;
		case LIST:
			exec_list(tstat->args[0].ival);
			break;
		case LOAD:
			exec_load(tstat->args[0].ival, tstat->args[1].ival);
			break;
		case STOR:
			exec_stor(tstat->args[0].ival);
			break;
		case SKPF:
			exec_skpf(tstat->args[0].ival);
			break;
		case SKIP:
			exec_skip(tstat->args[0].ival);
			break;
		case EQUA:
			exec_equa();
			break;
		case NEQU:
			exec_nequ();
			break;
		case GRTR:
			exec_grtr();
			break;
		case GREQ:
			exec_greq();
			break;
		case LETH:
			exec_leth();
			break;
		case LEEQ:
			exec_leeq();
			break;
		case PLUS:
			exec_plus();
			break;
		case MINU:
			exec_minu();
			break;
		case MULT:
			exec_mult();
			break;
		case DIVI:
			exec_divi();
			break;
		case APPN:
			exec_app();
			break;
		case UMIN:
			exec_umin();
			break;
		case NEGA:
			exec_nega();
			break;
		case T_EMPT:
			exec_empt();
			break;
		case T_LENG:
			exec_leng();
			break;
		case T_HEAD:
			exec_head();
			break;
		case T_TAIL:
			exec_tail();
			break;
		case PUSH:
			exec_push(tstat->args[0].ival);
			break;
		case JUMP:
			exec_jump(tstat->args[0].ival);
			break;
		case APOP: 
			exec_apop();
			break;
		case T_FUNC: break;
		case RETN:
			exec_retn();
			break;
		case GETI:
			exec_geti();
			break;
		case GETS:
			exec_gets();
			break;
		case GETB:
			exec_getb();
			break;
		case GETL:
			exec_getl(tstat->args[0].ival, tstat->args[1].ival);
			break;
		case PUTS:
			exec_puts_show();
			printf("\n");
			break;
		case T_SHOW:
			exec_puts_show();
			break;
		default: error("Unknown T-code instruction \n");
			break;
	}
}

Operator fromStringToOp(char *str) {
	if(strcmp(str,"VARS")==0)
		return VARS;
	else if(strcmp(str,"HALT")==0)
		return HALT;
	else if(strcmp(str,"NEWI")==0)
		return NEWI;
	else if(strcmp(str,"NEWB")==0)
		return NEWB;
	else if(strcmp(str,"NEWS")==0)
		return NEWS;
	else if(strcmp(str,"NEWL")==0)
		return NEWL;
	else if(strcmp(str,"LOCI")==0)
		return LOCI;
	else if(strcmp(str,"LOCS")==0)
		return LOCS;
	else if(strcmp(str,"LOCB")==0)
		return LOCB;
	else if(strcmp(str,"LIST")==0)
		return LIST;
	else if(strcmp(str,"LOAD")==0)
		return LOAD;
	else if(strcmp(str,"STOR")==0)
		return STOR;
	else if(strcmp(str,"SKPF")==0)
		return SKPF;
	else if(strcmp(str,"SKIP")==0)
		return SKIP;
	else if(strcmp(str,"EQUA")==0)
		return EQUA;
	else if(strcmp(str,"NEQU")==0)
		return NEQU;
	else if(strcmp(str,"GRTR")==0)
		return GRTR;
	else if(strcmp(str,"GREQ")==0)
		return GREQ;
	else if(strcmp(str,"LETH")==0)
		return LETH;
	else if(strcmp(str,"LEEQ")==0)
		return LEEQ;
	else if(strcmp(str,"PLUS")==0)
		return PLUS;
	else if(strcmp(str,"MINU")==0)
		return MINU;
	else if(strcmp(str,"MULT")==0)
		return MULT;
	else if(strcmp(str,"DIVI")==0)
		return DIVI;
	else if(strcmp(str,"APPN")==0)
		return APPN;
	else if(strcmp(str,"UMIN")==0)
		return UMIN;
	else if(strcmp(str,"NEGA")==0)
		return NEGA;
	else if(strcmp(str,"EMPT")==0)
		return T_EMPT;
	else if(strcmp(str,"LENG")==0)
		return T_LENG;
	else if(strcmp(str,"HEAD")==0)
		return T_HEAD;
	else if(strcmp(str,"TAIL")==0)
		return T_TAIL;
	else if(strcmp(str,"PUSH")==0)
		return PUSH;
	else if(strcmp(str,"JUMP")==0)
		return JUMP;
	else if(strcmp(str,"FUNC")==0)
		return T_FUNC;
	else if(strcmp(str,"APOP")==0)
		return APOP;
	else if(strcmp(str,"RETN")==0)
		return RETN;
	else if(strcmp(str,"GETI")==0)
		return GETI;
	else if(strcmp(str,"GETS")==0)
		return GETS;
	else if(strcmp(str,"GETB")==0)
		return GETB;
	else if(strcmp(str,"GETL")==0)
		return GETL;
	else if(strcmp(str,"PUTS")==0)
		return PUTS;	
	else if(strcmp(str,"SHOW")==0)
		return T_SHOW;
	else
		error("Unknown T-code instruction\n");
}

Atomic fromStringToAtomic(char *arg) {
	if(strcmp(arg,"INT")==0) 
		return A_INT;
	else if(strcmp(arg,"STRING")==0)
		return A_STRING;
	else if(strcmp(arg,"BOOL")==0)
		return A_BOOL;
		
	return A_UNKNOWN;
}

void inizialize_char_array(char array[]) {
	for(int i=0; i < STRING_DIM; i++) {
		array[i] = '\0';
	}
}

void start_machine() {
	FILE *fp;  
	fp = fopen("IntermediateCode.txt", "r");
	
	char word[STRING_DIM];
	int i = 0, step = 0, code_size;
	char c = fgetc(fp);
	
	while(c != '\n') {
		while(c != ' ' && c != '\n') {  
			word[i++] = c; 
			c=fgetc(fp);
		}
		if(step != 0)
			code_size = atoi(word);
		else
			c=fgetc(fp);	
		step++;
		inizialize_char_array(word);
		i = 0;	
	}
	step = 0;
	i = 0;
	inizialize_char_array(word);
	prog = (Tcode*) malloc(code_size*sizeof(Tcode));
	
	while((c = fgetc(fp)) != EOF) {
		while(c != '\n') {
			while(c != ' ' && c != '\n') {
				if(c == '"') {
					c=fgetc(fp);
					do {
						word[i++] = c;
						c=fgetc(fp);
						if(word[i-2] == '\\' && word[i-1] == 'n') {
							i = i-2;
							word[i++] = '\n';
							word[i] = '\0';
						}
					} while(c != '"');
					c=fgetc(fp);
				}
				else {
					word[i++] = c;
					c=fgetc(fp);
				}
			}
			if(step == 0)
				prog->oper = fromStringToOp(word);
			else if(step == 1) {
				if(prog->oper == LOCS) {
					prog->args[0].sval = malloc(strlen(word));
					strcpy(prog->args[0].sval,word);
				}
				else
					prog->args[0].ival = atoi(word);
			}
			else {
				if(prog->oper == GETL) {
					prog->args[1].ival = fromStringToAtomic(word);
				}
				else
					prog->args[1].ival = atoi(word);
			}
				
			step++;	
			i = 0;
			inizialize_char_array(word);
			if(c != '\n')
				c=fgetc(fp);
		}
		step = 0;
		i = 0;
		inizialize_char_array(word);
		prog++;
	}
	
	prog = prog - code_size;
	fclose(fp);
	
	inizializeObject(istack, MAX_IP_OBJECTS);
	
}

int main() {
	Tcode *tstat;
	
	start_machine();
	printf("\n");
	while((tstat = &prog[pc++])->oper != HALT) {
		execute(tstat);
	}
	printf("\n");
	return 0;
}

