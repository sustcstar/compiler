#ifndef PARSETREE_H
#define PARSETREE_H
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

#define NONTERMINAL 0
#define TERMINAL_VALUE 1
#define TERMINAL_NOVALUE 2

typedef struct parseTree{
    char *token_name; //terminal_names, and all nonterminal_names
    int type; //nonterminal, terminal with attribute, terminal with no attribute
    char *attribute; //int/float/char/ID values
    int lineno;
    int kids_num;
    struct parseTree **kids;
}parseTree;

//构造一个新节点
parseTree *newNode(char *token_name, int type, char *attribute, int lineno, int kids_num, ...);

void preOrderPrint(parseTree *root, int spacenum);

void freeParseTree(parseTree *root);
#endif