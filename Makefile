CC = g++ -O2 -Wno-deprecated -std=c++11 

LD_FLAGS = -l pthread -lgtest

tag = -i
test_out_tag = -ll

ifdef linux
tag = -n
test_out_tag = -lfl
endif

gtest: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o BigQ.o Pipe.o y.tab.o lex.yy.o BigQ_Test.o
	$(CC) -o gtest Record.o BigQ_Test.o Comparison.o ComparisonEngine.o Schema.o BigQ.o Pipe.o File.o DBFile.o y.tab.o lex.yy.o $(test_out_tag) $(LD_FLAGS)

test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o BigQ.o Pipe.o y.tab.o lex.yy.o test.o
	$(CC) -o test.out Record.o Comparison.o ComparisonEngine.o Schema.o BigQ.o Pipe.o File.o DBFile.o y.tab.o lex.yy.o test.o $(test_out_tag) -l pthread
	
test_a1.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o BigQ.o Pipe.o y.tab.o lex.yy.o test_a1.o
	$(CC) -o test_a1.out Record.o Comparison.o ComparisonEngine.o Schema.o BigQ.o Pipe.o File.o DBFile.o y.tab.o lex.yy.o test_a1.o $(test_out_tag) -l pthread
	
main: Record.o Comparison.o ComparisonEngine.o Schema.o File.o y.tab.o lex.yy.o main.o
	$(CC) -o main Record.o Comparison.o ComparisonEngine.o Schema.o File.o y.tab.o lex.yy.o main.o $(test_out_tag)
	
#DBFile_test.o:
#	$(CC) -g -c DBFile_test.cc

BigQ_Test.o:
	$(CC) -g -c BigQ_Test.cc

test.o: test.cc
	$(CC) -g -c test.cc

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

Record.o: Record.cc
	$(CC) -g -c Record.cc

Schema.o: Schema.cc
	$(CC) -g -c Schema.cc

BigQ.o: BigQ.cc
	$(CC) -g -c BigQ.cc

Pipe.o: Pipe.cc
	$(CC) -g -c Pipe.cc
	
y.tab.o: Parser.y
	yacc -d Parser.y
	sed $(tag) -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/" y.tab.c 
	g++ -c y.tab.c

lex.yy.o: Lexer.l
	lex  Lexer.l
	gcc  -c lex.yy.c

clean: 
	rm -f *.o
	rm -f *.out
	rm -f y.tab.c
	rm -f lex.yy.c
	rm -f y.tab.h

uninstall:	clean
	rm -f main gtest *.bin