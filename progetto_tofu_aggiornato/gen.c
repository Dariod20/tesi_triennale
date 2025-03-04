#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantics.c"

#define ARGS_DIM 2
#define FUNC_DIM 100
#define FIRST_DECL_FUNC 5

typedef struct statement{
	int address;
	Operator op;
	Value args [ARGS_DIM];
	struct statement *next;
} Stat;

typedef struct {
	Stat *head;
	int size;
	Stat *tail;	
} Code;

int func_index;
int var_oid;
int entry[FUNC_DIM];
int verify_var = FALSE;
int check_builtin = FALSE;
	
void relocate(Code code, int offset) {
	Stat *pt = code.head;
	
	for(int i=0; i < code.size; i++) {
		pt->address += offset;
		pt = pt->next;
	}
}

Code appcode(Code code1, Code code2) {
	Code rescode;
	
	relocate(code2, code1.size);
	rescode.head = code1.head;
	rescode.tail = code2.tail;
	code1.tail->next = code2.head;
	rescode.size = code1.size + code2.size;
	return rescode;
}

Code endcode() {
	static Code code = {NULL, 0, NULL};
	return code;
}

Code concode(Code code1, Code code2, ...) {
	Code rescode = code1;
	Code *pcode = &code2;
	while(pcode->head != NULL) {
		rescode = appcode(rescode, *pcode);
		pcode++;
	}
	return rescode;
}

Stat *newstat(Operator op) {
	Stat *pstat;
	pstat = (Stat*) malloc(sizeof(Stat));
	pstat->address = 0;
	pstat->op = op;
	pstat->next = NULL;
	return pstat;
}

Code makecode(Operator op) {
	Code code;
	code.head = code.tail = newstat(op);
	code.size = UNO;
	return code;
}

Code makecode1(Operator op, int arg) {
	Code code;
	code = makecode(op);
	code.head->args[0].ival = arg;
	return code;
}

Code makecode2(Operator op, int arg1, int arg2) {
	Code code;
	code = makecode(op);
	code.head->args[0].ival = arg1;
	code.head->args[1].ival = arg2;
	return code;
}

Code make_vars(int num_var) {
	return makecode1(VARS, num_var);
}

Code make_halt() {
	return makecode(HALT);
}

Code make_newi() {
	return makecode(NEWI);
}

Code make_news() {
	return makecode(NEWS);
}

Code make_newb() {
	return makecode(NEWB);
}

Code make_newl() {
	return makecode(NEWL);
}

Code make_loci(int num) {
	return makecode1(LOCI, num);
}

Code make_locb(int b) {
	return makecode1(LOCB, b);
}

Code make_locs(char *s) {
	Code code;
	code = makecode(LOCS);
	code.head->args[0].sval = s;
	return code;
}

Code make_list(int size) {
	return makecode1(LIST, size);
}

Code make_load(int flag, int oid) {
	return makecode2(LOAD, flag, oid);
}

Code make_stor(int oid) {
	return makecode1(STOR, oid);
}

Code make_and(int offset, Code code, int skip, int f_loci) {
	return concode(makecode1(SKPF, offset), code, makecode1(SKIP, skip), make_locb(f_loci), endcode());
}

Code make_or(int skpf, int t_loci, int exit) {
	return concode(makecode1(SKPF, skpf), make_locb(t_loci), makecode1(SKIP, exit), endcode());
}

Code make_equa() {
	return makecode(EQUA);
}

Code make_nequ() {
	return makecode(NEQU);
}

Code make_grtr() {
	return makecode(GRTR);
}

Code make_greq() {
	return makecode(GREQ);
}

Code make_leth() {
	return makecode(LETH);
}

Code make_leeq() {
	return makecode(LEEQ);
}

Code make_plus() {
	return makecode(PLUS);
}

Code make_minu() {
	return makecode(MINU);
}

Code make_mult() {
	return makecode(MULT);
}

Code make_divi() {
	return makecode(DIVI);
}

Code make_appn() {
	return makecode(APPN);
}

Code make_umin() {
	return makecode(UMIN);
}

Code make_nega() {
	return makecode(NEGA);
}

Code make_empt() {
	return makecode(T_EMPT);
}

Code make_leng() {
	return makecode(T_LENG);
}

Code make_head() {
	return makecode(T_HEAD);
}

Code make_tail() {
	return makecode(T_TAIL);
}

Code make_func_call(int numparams, int entry) {
	return concode(makecode1(PUSH, numparams), makecode1(JUMP, entry), makecode(APOP), endcode());
}

Code make_if_expr(int offset, Code code, int exit) {
	return concode(makecode1(SKPF, offset), code, makecode1(SKIP, exit), endcode());
}

Code make_func_decl(int fid, Code code) {
	return concode(makecode1(T_FUNC, fid), code, makecode(RETN), endcode());
}

Code make_geti() {
	return makecode(GETI);
}

Code make_gets() {
	return makecode(GETS);
}

Code make_getb() {
	return makecode(GETB);
}

Code make_getl(int depth, int atomic) {
	return makecode2(GETL, depth, atomic);
}

Code make_puts() {
	return makecode(PUTS);
}

Code make_show() {
	return makecode(T_SHOW);
}

Code genVarCode() {
	Code code = endcode();
	for(int i=0; i < TOT; i++) {
		if(symbolTable[i].name != NULL && symbolTable[i].classe == VAR && symbolTable[i].oid == var_oid) {
			switch(symbolTable[i].tipo.domain) {
				case D_INT:
					var_oid++;
					code = genVarCode();
					if(code.size != 0) {
						code = concode(make_newi(), code, endcode());
					}
					else {
						code = make_newi();	
					}
					break;
				case D_STRING:
					var_oid++;
					code = genVarCode();
					if(code.size != 0) {
						code = concode(make_news(), code, endcode());
					}
					else {
						code = (make_news());	
					}
					break;
				case D_BOOL:
					var_oid++;
					code = genVarCode();
					if(code.size != 0) {
						code = concode(make_newb(), code, endcode());
					}
					else {
						code = make_newb();	
					}
					break;
				case D_LIST:
					var_oid++;
					code = genVarCode();
					if(code.size != 0) {
						code = concode(make_newl(), code, endcode());
					}
					else {
						code = make_newl();	
					}
					break;
			}
		}
	}
	return code;
}

Code genBuiltinCode(char *name) {
	Code code;
	
	if(strcmp(name, EMPTY)==0) {
		check_builtin = TRUE;
		code = make_empt();
	}
	else if(strcmp(name, LENGTH)==0) {
		check_builtin = TRUE;
		code = make_leng();
	}
	else if(strcmp(name, HEAD)==0) {
		check_builtin = TRUE;
		code = make_head();
	}
	else if(strcmp(name, TAIL)==0) {
		check_builtin = TRUE;
		code = make_tail();
	}
	else
		code = endcode();
	return code;
}

Code genConstCode(Pnode p) {
	Code code;
	
	if(p->type == T_INTCONST) 
		code = make_loci(p->value.ival);
	else if(p->type == T_STRCONST)
		code = make_locs(p->value.sval);
	else if (p->type == T_BOOLCONST)
		code = make_locb(p->value.ival);
		
	return code;
}

Code genIdCode(Pnode p) {
	Code code;
	
	if(p->type == T_INT) 
		code = make_geti();
	else if(p->type == T_STRING)
		code = make_gets();
	else if (p->type == T_BOOL)
		code = make_getb();
	else if (p->value.ival == NLIST_TYPE) {
		Type t = defineType(p);
		code = make_getl(t.depth, t.atomic);
	}
		
	return code;
}

Code genSimpleExprCode(Pnode expr) {
	Code code;
	
	if(expr->type == T_ID) {
		if(verify_var == FALSE) {
			Formals formals = symbolTable[func_index].formals;
			for(int i=0; i < formals.num; i++) {
				if(strcmp(expr->value.sval, formals.params[i].name)==0) {
					code = make_load(1, formals.params[i].oid);
				}
			}
		}
		else {
			int index = binarySearch(TOT, hash(expr->value.sval, TOT));
			code = make_load(0, symbolTable[index].oid);
		}
	}
	else {
		code = genConstCode(expr);
	}
	return code;
}

Code genExprCode(Pnode expr) {
	Code code;
	if(expr->type != T_NONTERMINAL) {
		code = genSimpleExprCode(expr);
	}
	else if(expr->value.ival == NAND) {
		Code code_expr2 = genExprCode(expr->p2);
		code = concode(genExprCode(expr->p1), make_and(code_expr2.size + 2, code_expr2, 2, 0),  endcode());
	}
	else if(expr->value.ival == NOR) {
		Code code_expr2 = genExprCode(expr->p2);
		code = concode(genExprCode(expr->p1), make_or(3, 1, code_expr2.size + 1), code_expr2, endcode());
	}
	else if(expr->value.ival == NEQ) {
		code = concode(genExprCode(expr->p1), genExprCode(expr->p2), make_equa(), endcode());
	} 
	else if(expr->value.ival == NNE) {
		code = concode(genExprCode(expr->p1), genExprCode(expr->p2), make_nequ(), endcode());
	}
	else if(expr->value.ival == NGR) {
		code = concode(genExprCode(expr->p1), genExprCode(expr->p2), make_grtr(), endcode());
	}
	else if(expr->value.ival == NGE) {
		code = concode(genExprCode(expr->p1), genExprCode(expr->p2), make_greq(), endcode());
	}
	else if(expr->value.ival == NLS) {
		code = concode(genExprCode(expr->p1), genExprCode(expr->p2), make_leth(), endcode());
	}
	else if(expr->value.ival == NLE) {
		code = concode(genExprCode(expr->p1), genExprCode(expr->p2), make_leeq(), endcode());
	}
	else if(expr->value.ival == NPLUS) {	
		code = concode(genExprCode(expr->p1), genExprCode(expr->p2), make_plus(), endcode());
	}
	else if(expr->value.ival == NMINUS) {
		code = concode(genExprCode(expr->p1), genExprCode(expr->p2), make_minu(), endcode());
	}
	else if(expr->value.ival == NMULT) {
		code = concode(genExprCode(expr->p1), genExprCode(expr->p2), make_mult(), endcode());
	}
	else if(expr->value.ival == NDIV) {
		code = concode(genExprCode(expr->p1), genExprCode(expr->p2), make_divi(), endcode());
	}
	else if(expr->value.ival == NAPP) {
		code = concode(genExprCode(expr->p1), genExprCode(expr->p2), make_appn(), endcode());
	}
	else if(expr->value.ival == NNEG_MINUS) {
		code = concode(genExprCode(expr->p1), make_umin(), endcode());
	}
	else if(expr->value.ival == NNOT) {
		code = concode(genExprCode(expr->p1), make_nega(), endcode());
	}
	else if(expr->value.ival == NIF_EXPR) {
		Code code_expr2 = genExprCode(expr->p1->p3);
		Code code_expr3 = genExprCode(expr->p1->p3->p3);
		code = concode(genExprCode(expr->p1), make_if_expr(code_expr2.size + 2, code_expr2, code_expr3.size + 1), code_expr3, endcode());
	}
	else if(expr->value.ival == NFUNC_CALL) {
		char *id = expr->p1->value.sval;
		
		Pnode func_call = expr->p2->p1;
		int num_param = 0;
		int index = binarySearch(TOT, hash(id, TOT));
		int jump = entry[symbolTable[index].oid];
		
		if(func_call != NULL) {
			Code param_code = genExprCode(func_call);
			num_param++;
			
			code = genBuiltinCode(id);
			if(check_builtin == TRUE) {
				code = concode(param_code, code, endcode());
			}
			else {
				func_call = func_call->p3;
				for(func_call; func_call != NULL; func_call = func_call->p3) {
					param_code = concode(param_code, genExprCode(func_call), endcode());
					num_param++;
				}
				code = concode(param_code, make_func_call(num_param, jump), endcode());
			}	
		}
		else {
			code = make_func_call(num_param, jump);
		}
		check_builtin = FALSE;
	}
	else if(expr->value.ival == NINPUT) {
		code = genIdCode(expr->p1->p1);
	}
	else if(expr->value.ival == NOUTPUT) {
		code = concode(genExprCode(expr->p1), make_puts(), endcode());	
	}	
	else if(expr->value.ival == NSHOW_STAT) {
		code = concode(genExprCode(expr->p1), make_show(), endcode());	
	}
	else if(expr->value.ival == NOPT_EXPR_LIST) {
		int size = 0;
		if(expr->p1 != NULL) {
			Code list_code = genExprCode(expr->p1);
			size++;
			Pnode opt_list = expr->p1->p3;
		
			for(opt_list; opt_list != NULL; opt_list = opt_list->p3) {
				list_code = concode(list_code, genExprCode(opt_list), endcode());
				size++;
			}
			code = concode(list_code, make_list(size), endcode());	
		}
		else {
			code = make_list(size);
		}
	}
	else if(expr->value.ival == NASSIGN_STAT) {
		char *id = expr->p1->value.sval;
		int index = binarySearch(TOT, hash(id, TOT));
		code = concode(genExprCode(expr->p2), make_stor(symbolTable[index].oid), endcode());
	}
	else {
		error("Unknown yacc instruction \n");
	}
	return code;
}

void *TcodeString(FILE *fp, Operator op, Value arg1, Value arg2) {
	switch(op) {
		case VARS:
			fprintf(fp, "VARS %d\n", arg1.ival);
			break;
		case HALT:
			fprintf(fp, "HALT\n");
			break;
		case NEWI:
			fprintf(fp, "NEWI\n");
			break;
		case NEWS:
			fprintf(fp, "NEWS\n");
			break;
		case NEWB:
			fprintf(fp, "NEWB\n");
			break;
		case NEWL:
			fprintf(fp, "NEWL\n");
			break;
		case LOCI:
			fprintf(fp, "LOCI %d\n", arg1.ival);
			break;
		case LOCB:
			fprintf(fp, "LOCB %d\n", arg1.ival);
			break;
		case LOCS:
			fprintf(fp, "LOCS %s\n", arg1.sval);
			break;
		case LIST:
			fprintf(fp, "LIST %d\n", arg1.ival);
			break;
		case LOAD:
			fprintf(fp, "LOAD %d %d\n", arg1.ival, arg2.ival);
			break;
		case STOR:
			fprintf(fp, "STOR %d\n", arg1.ival);
			break;
		case SKPF:
			fprintf(fp, "SKPF %d\n", arg1.ival);
			break;
		case SKIP:
			fprintf(fp, "SKIP %d\n", arg1.ival);
			break;
		case EQUA:
			fprintf(fp, "EQUA\n");
			break;
		case NEQU:
			fprintf(fp, "NEQU\n");
			break;
		case GRTR:
			fprintf(fp, "GRTR\n");
			break;
		case GREQ:
			fprintf(fp, "GREQ\n");
			break;
		case LETH:
			fprintf(fp, "LETH\n");
			break;
		case LEEQ:
			fprintf(fp, "LEEQ\n");
			break;
		case PLUS:
			fprintf(fp, "PLUS\n");
			break;
		case MINU:
			fprintf(fp, "MINU\n");
			break;
		case MULT:
			fprintf(fp, "MULT\n");
			break;
		case DIVI:
			fprintf(fp, "DIVI\n");
			break;
		case APPN:
			fprintf(fp, "APPN\n");
			break;
		case UMIN:
			fprintf(fp, "UMIN\n");
			break;
		case NEGA:
			fprintf(fp, "NEGA\n");
			break;
		case T_EMPT:
			fprintf(fp, "EMPT\n");
			break;
		case T_LENG:
			fprintf(fp, "LENG\n");
			break;
		case T_HEAD:
			fprintf(fp, "HEAD\n");
			break;
		case T_TAIL:
			fprintf(fp, "TAIL\n");
			break;
		case PUSH:
			fprintf(fp, "PUSH %d\n", arg1.ival);
			break;
		case JUMP:
			fprintf(fp, "JUMP %d\n", arg1.ival);
			break;
		case APOP:
			fprintf(fp, "APOP\n");
			break;
		case T_FUNC:
			fprintf(fp, "FUNC %d\n", arg1.ival);
			break;
		case RETN:
			fprintf(fp, "RETN\n");
			break;
		case GETI:
			fprintf(fp, "GETI\n");
			break;
		case GETS:
			fprintf(fp, "GETS\n");
			break;
		case GETB:
			fprintf(fp, "GETB\n");
			break;
		case GETL:
			fprintf(fp, "GETL %d %s\n", arg1.ival, describeAtomic(arg2.ival));
			break;
		case PUTS:
			fprintf(fp, "PUTS\n");
			break;
		case T_SHOW:
			fprintf(fp, "SHOW\n");
			break;
		default: error("Unknown T-code instruction \n");
			break;
	}
}

Code varBodyTcode() {
	Code var_code = genVarCode();
	Code code;
	Pnode pnode = root->p1->p3->p3->p1;
	verify_var = TRUE;
	if(var_code.size != 0) {
		code = concode(make_vars(var_code.size), var_code, genExprCode(pnode), endcode());
	}
	else {
		code = concode(make_vars(var_code.size), genExprCode(pnode), endcode());
	}
	for(pnode = pnode->p3; pnode != NULL; pnode = pnode->p3) {
		code = concode(code, genExprCode(pnode), endcode());
	}
	return concode(code, make_halt(), endcode());
}

Code funcTcodeSenzaJump(Pnode p, Code varBody) {
	Code func_code;
	int j = FIRST_DECL_FUNC + 1;
	
	if(p != NULL) {
			char *name = p->p1->value.sval;
			func_index = binarySearch(TOT, hash(name, TOT));
			entry[FIRST_DECL_FUNC] = varBody.tail->address + 1;
			Pnode expr = p->p1->p3->p3->p3;
			func_code = make_func_decl(symbolTable[func_index].oid, genExprCode(expr));
			for(p = p->p3; p != NULL; p = p->p3) {
				name = p->p1->value.sval;
				func_index = binarySearch(TOT, hash(name, TOT));
				entry[j++] = entry[FIRST_DECL_FUNC] + func_code.tail->address + 1; 
				expr = p->p1->p3->p3->p3;
				func_code = concode(func_code, make_func_decl(symbolTable[func_index].oid, genExprCode(expr)), endcode());
			}
			return func_code;	
		}
		
		return endcode();
}

void genTcode(FILE *fp) {
	Code code = varBodyTcode();
	Code func_code;
	Pnode p = root->p1->p3->p1;
	Pnode pSenzaJump = p;
	verify_var = FALSE;
	
	func_code = funcTcodeSenzaJump(pSenzaJump, code);
	
	if(p != NULL) {
			char *name = p->p1->value.sval;
			func_index = binarySearch(TOT, hash(name, TOT));
			Pnode expr = p->p1->p3->p3->p3;
			func_code = make_func_decl(symbolTable[func_index].oid, genExprCode(expr));
			for(p = p->p3; p != NULL; p = p->p3) {
				name = p->p1->value.sval;
				func_index = binarySearch(TOT, hash(name, TOT)); 
				expr = p->p1->p3->p3->p3;
				func_code = concode(func_code, make_func_decl(symbolTable[func_index].oid, genExprCode(expr)), endcode());
			}	
		}
		else {
			func_code = endcode();
		}

	var_oid = 0;
	if(func_code.size != 0) {
		code = concode(varBodyTcode(), func_code, endcode());
	}
	else {
		code = varBodyTcode();
	}
	fprintf(fp, "TCODE %d\n", code.size);
	for(Stat *stat = code.head; stat != NULL; stat = stat->next) {
		TcodeString(fp, stat->op, stat->args[0], stat->args[1]);
	}
}

int main() {
	semantics();
	
	FILE *fp;  
	fp = fopen("IntermediateCode.txt", "w"); 	
	genTcode(fp);
	fclose(fp);
	
	return 0;
}

