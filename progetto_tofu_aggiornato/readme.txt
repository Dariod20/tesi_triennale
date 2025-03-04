La prima volta che si vuole eseguire un progrmma tofu bisogna compilare il file makefile.txt da linea di comando nel seguente modo:
	sh makefile.txt

Dopodichè ogni volta che si modifica il programma tofu o se ne scrive uno nuovo, bisogna SEMPRE eseguire i due seguenti comandi:
	./T_code <program.tofu
	./tofu

Se invece il programma non viene modificato ed è sempre lo stesso, allora basta digitare il comando:
	./tofu

Comando per convertire il carattere eof da Windows a Unix:
tr -d '\15\32' < winfile.txt > unixfile.txt

File richiesti:
-def.h
-tree.c
-lexer.lex (analisi lessicale: lex)
-parser.y  (analisi sintattica: yacc) 
-semantics.c (analisi semantica)
-gen.c (generazione codice intermedio)
-interpreter.c (interprete/macchina virtuale)


LEGENDA:
output:  input
	comandi su linux


parser.h parser.c parser.output parser.dot:  parser.y def.h
	bison -dvg -o parser.c parser.y

parser.o parser.pdf:  parser.c def.h parser.dot
	cc -g -c parser.c
	dot -Tpdf -o parser.pdf parser.dot

lexer.c:  lexer.lex def.h
	flex -o lexer.c lexer.lex

lexer.o:  lexer.c parser.h def.h
	cc -g -c lexer.c 

tree.o:  tree.c def.h
	cc -g -c tree.c

gen.o: def.h parser.h semantics.c gen.c
	cc -g -c gen.c

T_code:  lexer.o parser.o tree.o gen.o
	cc -g -o T_code lexer.o parser.o tree.o gen.o

interpreter.o:  def.h interpreter.c
	cc -g -c interpreter.c

tofu: interpreter.o
	cc -g -o tofu interpreter.o

AbstractTree.txt SymbolTable.txt IntermediateCode.txt: T_code program.tofu
	./T_code <program.tofu

output vero e proprio di tofu:  tofu
	./tofu