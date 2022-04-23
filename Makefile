
CC = g++ -std=c++11 -Wno-deprecated -w

tag = -i
gTest_tag = -std=c++11 -lgtest -lgtest_main

ifdef linux
tag = -n
endif

a5.out:   y.tab.o lex.yy.o RelOp.o Record.o Schema.o Comparison.o ComparisonEngine.o Function.o Pipe.o BigQ.o File.o DBFile.o DBFileHeap.o DBFileSorted.o DBFileTree.o NodeForQuery.o Statistics.o main.o
	$(CC) -o a5.out y.tab.o lex.yy.o RelOp.o Record.o Schema.o Comparison.o ComparisonEngine.o Function.o Pipe.o BigQ.o File.o DBFile.o DBFileHeap.o DBFileSorted.o DBFileTree.o NodeForQuery.o Statistics.o main.o -lpthread

main: main.o Record.o Comparison.o Statistics.o ComparisonEngine.o Schema.o File.o DBFile.o DBFileGeneric.o DBFileTree.o BigQ.o Pipe.o y.tab.o yyfunc.tab.o lex.yy.o lex.yyfunc.o DBFileSorted.o DBFileHeap.o RelOp.o Function.o
	$(CC) -o main.o Record.o Comparison.o Statistics.o ComparisonEngine.o Schema.o DBFileGeneric.o DBFileTree.o BigQ.o Pipe.o File.o DBFileSorted.o DBFileHeap.o DBFile.o y.tab.o yyfunc.tab.o lex.yy.o RelOp.o lex.yyfunc.o Function.o -lpthread

gTest: Record.o Comparison.o Statistics.o ComparisonEngine.o Schema.o File.o DBFile.o DBFileGeneric.o DBFileTree.o BigQ.o Pipe.o y.tab.o yyfunc.tab.o lex.yy.o lex.yyfunc.o gTest.o DBFileSorted.o DBFileHeap.o RelOp.o Function.o
	$(CC) -o gTest Record.o gTest.o Comparison.o Statistics.o ComparisonEngine.o Schema.o DBFileGeneric.o DBFileTree.o BigQ.o Pipe.o File.o DBFileSorted.o DBFileHeap.o DBFile.o y.tab.o yyfunc.tab.o lex.yy.o RelOp.o lex.yyfunc.o Function.o $(gTest_tag) -l pthread -lgtest

test.out: Record.o Comparison.o Statistics.o ComparisonEngine.o Schema.o File.o DBFile.o DBFileGeneric.o DBFileHeap.o DBFileSorted.o DBFileTree.o Pipe.o BigQ.o RelOp.o Function.o y.tab.o yyfunc.tab.o lex.yy.o lex.yyfunc.o test.o
	$(CC) -o test.out Record.o Comparison.o Statistics.o ComparisonEngine.o Schema.o File.o DBFile.o DBFileGeneric.o DBFileHeap.o DBFileSorted.o DBFileTree.o Pipe.o BigQ.o RelOp.o Function.o y.tab.o yyfunc.tab.o lex.yy.o lex.yyfunc.o test.o -lpthread

# test.out: Record.o Comparison.o Statistics.o ComparisonEngine.o Schema.o File.o DBFile.o DBFileGeneric.o DBFileHeap.o DBFileSorted.o DBFileTree.o Pipe.o BigQ.o RelOp.o Function.o y.tab.o yyfunc.tab.o lex.yy.o lex.yyfunc.o test.o
# 	$(CC) -o test.out Record.o Comparison.o Statistics.o ComparisonEngine.o Schema.o File.o DBFile.o DBFileGeneric.o DBFileHeap.o DBFileSorted.o DBFileTree.o Pipe.o BigQ.o RelOp.o Function.o y.tab.o yyfunc.tab.o lex.yy.o lex.yyfunc.o test.o -lpthread

# test_a3.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o DBFileGeneric.o DBFileHeap.o DBFileSorted.o DBFileTree.o Pipe.o BigQ.o RelOp.o Function.o y.tab.o yyfunc.tab.o lex.yy.o lex.yyfunc.o test_a3.o
# 	$(CC) -o test_a3.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o DBFileGeneric.o DBFileHeap.o DBFileSorted.o DBFileTree.o Pipe.o BigQ.o RelOp.o Function.o y.tab.o yyfunc.tab.o lex.yy.o lex.yyfunc.o test_a3.o -lpthread
	
a2-2test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o Pipe.o y.tab.o lex.yy.o a2-2test.o
	$(CC) -o a2-2test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o Pipe.o y.tab.o lex.yy.o a2-2test.o -lpthread
	
a2test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o DBFileGeneric.o DBFileHeap.o DBFileSorted.o DBFileTree.o Pipe.o y.tab.o lex.yy.o a2test.o
	$(CC) -o a2test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o DBFileGeneric.o DBFileHeap.o DBFileSorted.o DBFileTree.o Pipe.o y.tab.o lex.yy.o a2test.o -lpthread
	
a1test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o Pipe.o y.tab.o lex.yy.o a1-test.o
	$(CC) -o a1test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o Pipe.o y.tab.o lex.yy.o a1-test.o

# gTest: Record.o Comparison.o ComparisonEngine.o Schema.o File.o Pipe.o y.tab.o lex.yy.o gTest.o  Function.o
# 	$(CC) -o gTest.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o Pipe.o y.tab.o lex.yy.o gTest.o Function.o $(gTest_tag)

main.o : main.cc
	$(CC) -g -c main.cc

gTest.o:
	$(CC) -g -c gTest.cc

Statistics.o: Statistics.cc
	$(CC) -g -c Statistics.cc

	
test.o: test.cc
	$(CC) -g -c test.cc

a2-2test.o: a2-2test.cc
	$(CC) -g -c a2-2test.cc

a2test.o: a2test.cc
	$(CC) -g -c a2test.cc

a1-test.o: a1-test.cc
	$(CC) -g -c a1-test.cc

NodeForQuery.o: NodeForQuery.cc
	$(CC) -g -c NodeForQuery.cc

Comparison.o: Comparison.cc
	$(CC) -g -c Comparison.cc
	
ComparisonEngine.o: ComparisonEngine.cc
	$(CC) -g -c ComparisonEngine.cc
	
DBFile.o: DBFile.cc
	$(CC) -g -c DBFile.cc

Pipe.o: Pipe.cc
	$(CC) -g -c Pipe.cc

BigQ.o: BigQ.cc
	$(CC) -g -c BigQ.cc

# DBFile.o: DBFile.cc
# 	$(CC) -g -c DBFile.cc

DBFileGeneric.o: DBFileGeneric.cc
	$(CC) -g -c DBFileGeneric.cc

DBFileHeap.o: DBFileHeap.cc
	$(CC) -g -c DBFileHeap.cc

DBFileSorted.o: DBFileSorted.cc
	$(CC) -g -c DBFileSorted.cc

DBFileTree.o: DBFileTree.cc
	$(CC) -g -c DBFileTree.cc

RelOp.o: RelOp.cc
	$(CC) -g -c RelOp.cc

Function.o: Function.cc
	$(CC) -g -c Function.cc

File.o: File.cc
	$(CC) -g -c File.cc

Record.o: Record.cc
	$(CC) -g -c Record.cc

Schema.o: Schema.cc
	$(CC) -g -c Schema.cc
	
y.tab.o: Parser.y
	yacc -d Parser.y
	#sed $(tag) y.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/" 
	g++ -c y.tab.c
		
yyfunc.tab.o: ParserFunc.y
	yacc -p "yyfunc" -b "yyfunc" -d ParserFunc.y
	#sed $(tag) yyfunc.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/" 
	g++ -c yyfunc.tab.c
	
lex.yy.o: Lexer.l
	lex Lexer.l
	gcc  -c lex.yy.c

lex.yyfunc.o: LexerFunc.l
	lex -Pyyfunc LexerFunc.l
	gcc  -c lex.yyfunc.c


clean: 
	rm -f *.o
	rm -f *.out
	rm -f y.tab.*
	rm -f yyfunc.tab.*
	rm -f lex.yy.*
	rm -f lex.yyfunc*
