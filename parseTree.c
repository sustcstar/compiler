#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include"parseTree.h"

// （1）va_list

// 定义了一个指针arg_ptr, 用于指示可选的参数.

// （2）va_start(arg_ptr, argN)
// 使参数列表指针arg_ptr指向函数参数列表中的第一个可选参数，argN是位于第一个可选参数之前的固定参数, 或者说最后一个固定参数.如有一va
// 函数的声明是void va_test(char a, char b, char c, …), 则它的固定参数依次是a,b,c, 最后一个固定参数argN为c, 因此就是va_start
// (arg_ptr, c).

// （3）va_arg(arg_ptr, type)
// 返回参数列表中指针arg_ptr所指的参数, 返回类型为type. 并使指针arg_ptr指向参数列表中下一个参数.返回的是可选参数, 不包括固定参数.

// （4）va_end(arg_ptr)
// 清空参数列表, 并置参数指针arg_ptr无效.
// （注：va在这里是variable-argument(可变参数)的意思. 这些宏定义在stdarg.h中,所以用到可变参数的程序应该包含这个头文件）

parseTree *newNode(char *token_name, int type, char *attribute, int lineno, int kids_num, ...){
    /*  1. 分配一个新node空间
        2. 分配可以直接分配的值
        3. 根据kidsnum分配空间
        4. 循环把kids放进kids数组里
    */
    parseTree *node = (parseTree *)malloc(sizeof(parseTree));

    node->token_name = token_name;
    node->type = type;
    node->attribute = attribute;
    node->lineno = lineno;
    node->kids_num = kids_num;
    
    if(kids_num > 0){
        node->kids = (parseTree **)malloc(sizeof(parseTree *) * kids_num);
    }
    else{
        node->kids = NULL;
    }

    va_list arg_ptr;
    va_start(arg_ptr, kids_num);
    for(int i = 0;i < kids_num;i++){
        node->kids[i] = va_arg(arg_ptr, parseTree *);
    }
    va_end(arg_ptr);

    return node;
}

// typedef struct parseTree{
//     char *token_name; //terminal_names, and all nonterminal_names
//     int type; //nonterminal, terminal with attribute, terminal with no attribute
//     char *attribute; //int/float/char/ID values
//     int lineno;
//     int kids_num;
//     struct parseTree **kids;
// }parseTree;

// #define NONTERMINAL 0
// #define TERMINAL_VALUE 1
// #define TERMINAL_NOVALUE 2

void printSpace(int spacenum){
    for(int i = 0;i < spacenum;i++){
        printf(" ");
    }
}

void preOrderPrint(parseTree *root, int spacenum){
    printSpace(spacenum);
    if(root->type == NONTERMINAL){
        printf("%s (%d)\n", root->token_name, root->lineno);
    }
    else if(root->type == TERMINAL_VALUE){
        printf("%s: %s\n", root->token_name, root->attribute);
    }
    else{ //TERMINAL_NOVALUE
        printf("%s\n", root->token_name);
    }
    for(int i = 0;i < root->kids_num;i++){
        preOrderPrint(root->kids[i], spacenum + 2);
    }
}

void freeParseTree(parseTree *root){

}