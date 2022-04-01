CC = g++ -O2 -Wno-deprecated -std=c++11 

LD_FLAGS = -l pthread -lgtest

tag = -i
test_out_tag = -ll

ifdef linux
tag = -n
test_out_tag = -lfl
endif

gtest: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o BigQ.o Pipe.o y.tab.o lex.yy.o gTest.o Sorted.o Heap.o
	$(CC) -o gtest Record.o gTest.o Comparison.o ComparisonEngine.o Schema.o BigQ.o Pipe.o File.o Sorted.o Heap.o DBFile.o y.tab.o lex.yy.o $(test_out_tag) $(LD_FLAGS)

test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o BigQ.o Pipe.o y.tab.o  yyfunc.tab.o lex.yy.o lex.yyfunc.o test.o Sorted.o Heap.o RelOp.o Function.o
	$(CC) -o test.out Record.o Comparison.o ComparisonEngine.o Schema.o BigQ.o Pipe.o File.o DBFile.o y.tab.o  yyfunc.tab.o lex.yy.o lex.yyfunc.o test.o Sorted.o Heap.o RelOp.o Function.o $(test_out_tag) -l pthread

test_a22.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o BigQ.o Pipe.o y.tab.o lex.yy.o test_a22.o Sorted.o Heap.o
	$(CC) -o test_a22.out Record.o Comparison.o ComparisonEngine.o Schema.o BigQ.o Pipe.o File.o DBFile.o y.tab.o lex.yy.o test_a22.o Sorted.o Heap.o $(test_out_tag) -l pthread

test_a21_old.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o BigQ.o Pipe.o y.tab.o  yyfunc.tab.o lex.yy.o lex.yyfunc.o test_a21_old.o Sorted.o Heap.o Function.o
	$(CC) -o test_a21_old.out Record.o Comparison.o ComparisonEngine.o Schema.o BigQ.o Pipe.o File.o DBFile.o y.tab.o Sorted.o Heap.o  yyfunc.tab.o lex.yy.o lex.yyfunc.o test_a21_old.o Function.o $(test_out_tag) -l pthread
	
test_a1.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o BigQ.o Pipe.o y.tab.o lex.yy.o test_a1.o Sorted.o Heap.o
	$(CC) -o test_a1.out Record.o Comparison.o ComparisonEngine.o Schema.o BigQ.o Pipe.o File.o DBFile.o Sorted.o Heap.o y.tab.o lex.yy.o test_a1.o $(test_out_tag) -l pthread
	
main: Record.o Comparison.o ComparisonEngine.o Schema.o File.o y.tab.o lex.yy.o main.o
	$(CC) -o main Record.o Comparison.o ComparisonEngine.o Schema.o File.o y.tab.o lex.yy.o main.o $(test_out_tag)
	
#DBFile_test.o:
#	$(CC) -g -c DBFile_test.cc

gTest.o:
	$(CC) -g -c gTest.cc

test.o: test.cc
	$(CC) -g -c test.cc

test_a22.o: test_a22.cc
	$(CC) -g -c test_a22.cc

test_a21_old.o: test_a21_old.cc
	$(CC) -g -c test_a21_old.cc

test_a1.o: test_a1.cc
	$(CC) -g -c test_a1.cc

main.o: main.cc
	$(CC) -g -c main.cc
	
Comparison.o: Comparison.cc
	$(CC) -g -c Comparison.cc
	
ComparisonEngine.o: ComparisonEngine.cc
	$(CC) -g -c ComparisonEngine.cc
	
DBFile.o: DBFile.cc
	$(CC) -g -c DBFile.cc

File.o: File.cc
	$(CC) -g -c File.cc

Function.o: Function.cc
	$(CC) -g -c Function.cc

Record.o: Record.cc
	$(CC) -g -c Record.cc

RelOp.o: RelOp.cc
	$(CC) -g -c RelOp.cc

Schema.o: Schema.cc
	$(CC) -g -c Schema.cc

Sorted.o: Sorted.cc
	$(CC) -g -c Sorted.cc

Heap.o: Heap.cc
	$(CC) -g -c Heap.cc

BigQ.o: BigQ.cc
	$(CC) -g -c BigQ.cc

Pipe.o: Pipe.cc
	$(CC) -g -c Pipe.cc
	
y.tab.o: Parser.y
	yacc -d Parser.y
	sed $(tag) -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/" y.tab.c 
	g++ -c y.tab.c

yyfunc.tab.o: ParserFunc.y
	yacc -p "yyfunc" -b "yyfunc" -d ParserFunc.y
	sed $(tag) yyfunc.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/" 
	g++ -c yyfunc.tab.c

lex.yy.o: Lexer.l
	lex  Lexer.l
	gcc  -c lex.yy.c

lex.yyfunc.o: LexerFunc.l
	lex -Pyyfunc LexerFunc.l
	gcc  -c lex.yyfunc.c

clean: 
	rm -f *.o
	rm -f *.out
	rm -f y.tab.c
	rm -f yyfunc.tab.*
	rm -f lex.yy.c
	rm -f lex.yyfunc*
	rm -f y.tab.h

uninstall:	clean
	rm -f main gtest *.bin