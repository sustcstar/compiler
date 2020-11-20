#ifndef PARSETREE_H
#define PARSETREE_H
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<iostream>

#define NONTERMINAL 0
#define TERMINAL_VALUE 1
#define TERMINAL_NOVALUE 2

typedef struct parseTree{
    std::string token_name; //terminal_names, and all nonterminal_names
    int type; //nonterminal, terminal with attribute, terminal with no attribute
    union{
        char *str_attribute; //int/float/char/ID values
        int value_attribute; //int/float/char/ID values
    }attribute;
    int lineno;
    int kids_num;
    struct parseTree **kids;
}parseTree;

//构造一个新节点
parseTree *newNode(std::string token_name, int type, char *attribute, int lineno, int kids_num, ...);

void preOrderPrint(parseTree *root, int spacenum);

void freeParseTree(parseTree *root);
#endif