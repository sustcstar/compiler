#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP
#include<iostream>
#include<map>
#include<vector>
#include"parseTree.hpp"

class Type{
    public:
    std::string name;
    int scope_level;
    enum Type_category {INT, FLOAT, CHAR, ARRAY, STRUCTURE} type_category;

    Type(std::string name, int scope_level, enum Type_category type_category){
        this->name = name;
        this->scope_level = scope_level;
        this->type_category = type_category;
    }
};

class Array: public Type{
    public:
    Type *base;
    int size;

    Array(std::string name, int scope_level, Type *base, int size):Type(name, scope_level, ARRAY){
        this->base = base;
        this->size = size;
    } 
};

class Structure: public Type{
    public:
    std::map<std::string, Type *> fieldList;

    Structure(std::string name, int scope_level, std::map<std::string, Type *> fieldList):Type(name, scope_level, STRUCTURE){
        this->fieldList = fieldList;
    } 
};

void semanticCheck(parseTree *root);
Type *checkSpecifier(parseTree *node);

#endif