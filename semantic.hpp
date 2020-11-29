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
    enum Type_category {INT, FLOAT, CHAR, ARRAY, STRUCTURE, FUNCTION} type_category;

    Type(std::string name, int scope_level, enum Type_category type_category){
        this->name = name;
        this->scope_level = scope_level;
        this->type_category = type_category;
    }

    // typeid 的操作对象既可以是表达式，也可以是数据类型，下面是它的两种使用方法：
    // typeid( dataType )
    // typeid( expression )
    virtual bool operator == (const Type &other) const { //定义这种类型的"=="关系运算符以及"!="关系运算符
        if(type_category != INT && type_category != FLOAT && type_category != CHAR){
            std::cout<<"Oppppps, A BIG UNEXPECTED BEHAVIOR!"<<std::endl;
        }
        // std::cout <<"this: "<< typeid(*this).name() << std::endl;
        // std::cout <<"other: "<< typeid(other).name() << std::endl;
        return (typeid(*this)==typeid(other) && \
                this->type_category == other.type_category);
    }
    //  virtual的意思是虚函数
    // 可以在基类中将被重写的成员函数设置为虚函数，其含义是：当通过基类的指针或者引用调用该成员函数时，将根据指针指向的
    // 对象类型确定调用的函数，而非指针的类型。
    // 如果没有virtual，则派生类型的同名方法无法被重写。
    virtual bool operator!=(const Type &other) const { 
        return !(*this == other);
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

    bool operator == (const Type &other) const { 
        // std::cout <<"this: "<< typeid(*this).name() << std::endl;
        // std::cout <<"other: "<< typeid(other).name() << std::endl;
        return (typeid(*this)==typeid(other) && \
                this->base == dynamic_cast<const Array &>(other).base && \
                this->size == dynamic_cast<const Array &>(other).size);
    }

    bool operator!=(const Type &other) const { 
        return !(*this == other);
    }
};

class Structure: public Type{
    public:
    std::map<std::string, Type *> fieldList;

    Structure(std::string name, int scope_level, std::map<std::string, Type *> fieldList):Type(name, scope_level, STRUCTURE){
        this->fieldList = fieldList;
    } 

    bool operator == (const Type &other) const { 
        // std::cout <<"this: "<< typeid(*this).name() << std::endl;
        // std::cout <<"other: "<< typeid(other).name() << std::endl;      
        return (typeid(*this)==typeid(other) && \
                this->name == dynamic_cast<const Structure &>(other).name);
    }

    bool operator!=(const Type &other) const { 
        return !(*this == other);
    }
};

class Function: public Type{
    public:
    Type *return_type;
    std::vector<std::pair<std::string, Type *>> args;

    Function(std::string name, Type *return_type, std::vector<std::pair<std::string, Type *>> args):Type(name, 0, FUNCTION){
        this->return_type = return_type;
        this->args = args;
    }

    //暂时默认函数名相同，则两个函数相等
    bool operator == (const Type &other) const { 
        // std::cout <<"this: "<< typeid(*this).name() << std::endl;
        // std::cout <<"other: "<< typeid(other).name() << std::endl;
        return (typeid(*this)==typeid(other) && \
                this->name == dynamic_cast<const Function &>(other).name);
    }

    bool operator!=(const Type &other) const { 
        return !(*this == other);
    }
};

void checkCompSt(parseTree *node, Type *returnType);
void semanticCheck(parseTree *root);
Type *checkSpecifier(parseTree *node);
Type *checkExp(parseTree *node);
std::map<std::string, Type *> accessSymbolTable();
bool changeSymbolTable(std::string key, Type *type, int lineno);
Type *keyToType(std::string key);

// //两个Type相等
// bool typeEqual(Type *A, Type *B){ //定义这种类型的"=="关系运算符以及"!="关系运算符
//     switch(A->type_category){
//         //基本类型
//         case Type::INT:
//         case Type::FLOAT:
//         case Type::CHAR:
//             return (A->type_category == B->type_category);
//             break;
//         //数组
//         case Type::ARRAY:
//             return (A->type_category == B->type_category && \
//                     ((Array *)A)->size == ((Array *)B)->size && \
//                     typeEqual(((Array *)A)->base, ((Array *)B)->base));
//             break;
//         //结构体
//         case Type::STRUCTURE:

//             break;
//         //函数
//         case Type::FUNCTION:
//             break;
//         //错误
//         default:
//             debug_log("OMG, AN UNEXPECTED BEHAVIOR!\n");
//             break;
//     }
//     return true;
// }

// //finished, 两个Type不相等
// bool typeNotEqual(Type *A, Type *B){ 
//     return !typeEqual(A, B);
// }

#endif