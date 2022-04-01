#ifndef DEFS_H
#define DEFS_H


#define MAX_ANDS 20
#define MAX_ORS 20

#define PAGE_SIZE 131072
#define PIPE_BUFFER_SIZE 128

enum Target {Left, Right, Literal};
enum CompOperator {LessThan, GreaterThan, Equals};
enum Type {Int, Double, String};

#define FILE_TYPE_HEAP "heap"
#define FILE_TYPE_SORTED "sorted"

unsigned int Random_Generate();


#endif

