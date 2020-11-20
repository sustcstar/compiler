#include"semantic.hpp"
#include"reportError.hpp"
#include <stdarg.h>
#include <map>
#define SE_DEBUG 1

//假设1：不会在函数内部定义结构体, 只有全局定义结构体
//假设2：只有全局scope

//实现1：实现基本类型（无赋值）的插入符号表以及检查是否重复（名称）
//实现2：实现基本类型多维数组（无赋值）的插入符号表以及检查是否重复（名称）
//预备3：实现结构体（无赋值）的插入符号表以及检查是否重复（名称）

FILE *out;
int current_scope_level;
// std::vector<Type *> symbol_table; 
std::map<std::string, Type *> symbol_table;

void debug_log(const char *str, ...){
#if  SE_DEBUG==1
    va_list args;
    va_start(args, str);
    vfprintf(out, str, args);
    va_end(args);
#endif
}

//finished
bool structRedefined(std::string name){
    bool flag;

    // 查询符号表
    auto iter = symbol_table.find(name);
    if(iter != symbol_table.end()){ //2. 这个id已经存在
        flag = true;
    }
    else{                //3. 这个id不存在
        flag = false;
    }
    return flag;
}

//finished
void VarDec_onlyID(parseTree *node, Type *type){
    //1. 查看这个id是否已经存在于symbol_table
    std::string id(node->kids[0]->attribute.str_attribute);
    auto iter = symbol_table.find(id);
    if(iter != symbol_table.end()){ //2. 这个id已经存在，则报错
        reportError(out, T3_VAR_REDEF, node->lineno);
    }
    else{                //3. 这个id不存在，则插入它
        symbol_table[id] = type;
    }
}

//1. 生成整个Type只能在上层, 所以必须把id传给上层
//2. 即便不把id传给上层，在上层通过specifer也能知道type
//3. 但总之，总结性工作（插表）只能在上层做
//finished
Type *VarDec_Array(parseTree *node, Type *type){
    Type *base = NULL;
    Array *array = NULL;
    int size = 0;
    //可以是ID，也可以是ID[INT]
    if(node->kids_num == 1){ 
        // 递归终止，需要传给上层的参数有
        // 由于这里没法插入id，只能让上层插入，所以要传给上层
        // 1. id
        // 建议：把id放进Type(传下来的参数type)里，作为一个name, 
        type->name = node->kids[0]->attribute.str_attribute;
        return type;
    }
    else{ 
        // 递归继续，需要传给上层的参数有
        // 1. 我这一层的size，以及下层的Type(也就是我这一层的base), 可以封装成一个Array一起传上去
        base = VarDec_Array(node->kids[0], type);
        size = node->kids[2]->attribute.value_attribute;
        array = new Array(base->name, current_scope_level, base, size);
        return array;
    }
}

//finished
void checkVarDec(parseTree *node, Type *type){
    Type *array = NULL;
    //可以是ID，也可以是ID[INT]
    if(node->kids_num == 1){ // 就一个ID,
        VarDec_onlyID(node, type);
    }
    else{ // 可以有数组 注意，有可能是多维数组
        //这里是顶层，建议在这里进行插入表操作
        array = VarDec_Array(node, type);
        auto iter = symbol_table.find(array->name);
        if(iter != symbol_table.end()){ //2. 这个id已经存在，则报错
            reportError(out, T3_VAR_REDEF, node->lineno);
        }
        else{                //3. 这个id不存在，则插入它
            symbol_table[array->name] = array;
        }
    }
}

void checkDec(parseTree *node){
    if(node->kids_num == 1){
        // checkVarDec(node->kids[0]);
    }
    else{ // 右边有赋值操作

    }
}

void checkDecList(parseTree *node){
    checkDec(node->kids[0]);
    if(node->kids_num > 1){ //checkDecList
        
    }
}

void checkDef(parseTree *node){
    Type *type;

    type = checkSpecifier(node->kids[0]);
    checkDecList(node->kids[1]);
}

std::vector<Type> checkDefList(parseTree *node){ //为structure的建立搜集参数列表信息
    std::vector<Type> fieldList;
    
    checkDef(node->kids[0]);
    if(node->kids_num > 1){ //checkDefList
        
    }
    return fieldList;
}

//finished
Type *checkStructSpecifier(parseTree *node){
    Type *structure;

    if(node->kids_num > 2){ //类型不存在，1.定义类型  2.检查是不是已经有这个结构体了 
        //1.定义类型
        std::vector<Type> fieldList;
        fieldList = checkDefList(node->kids[3]);
        std::string id(node->kids[1]->attribute.str_attribute);
        structure = new Structure(id, current_scope_level, fieldList);
        //2.检查该结构体是否存在
        if(structRedefined(structure->name)){
            reportError(out, T15_STRUCTURE_REDEF, node->lineno);
        }
    }
    else{ //1. 类型已存在，根据ID返回类型 
        std::string id(node->kids[1]->attribute.str_attribute);
        if(structRedefined(id)){
            structure = symbol_table[id];
        }
        else{ //2. 类型不存在，报错
            reportError(out, T1_VAR_USED_NO_DEF, node->lineno);
        }
    }
    return structure;
}

//finished
Type *checkTYPE(parseTree *node){
    std::string value(node->attribute.str_attribute);
    Type *primitive;

    if(value.compare("int") == 0){
        primitive = new Type("", current_scope_level, Type::INT);
    }
    else if(value.compare("float") == 0){
        primitive = new Type("", current_scope_level, Type::FLOAT);
    }
    else if(value.compare("char") == 0){
        primitive = new Type("", current_scope_level, Type::CHAR);
    }
    return primitive;
}

//finished
Type *checkSpecifier(parseTree *node){
    Type *type;

    if(node->kids[0]->token_name.compare("TYPE") == 0){
        type = checkTYPE(node->kids[0]);
    }
    else if(node->kids[0]->token_name.compare("StructSpecifier") == 0){
        type = checkStructSpecifier(node->kids[0]);
    }
    return type;
}

void checkExtDecList(parseTree *node, Type *type){
    checkVarDec(node->kids[0], type);
    if(node->kids_num > 1){

    }
}

void checkExtDef(parseTree *node){
    Type *type;
    debug_log("In checkExtDef, before checkSpecifier.\n");
    type = checkSpecifier(node->kids[0]); //其实就是返回类型
    debug_log("In checkExtDef, after checkSpecifier.\n");
    if(node->kids[1]->token_name.compare("ExtDecList") == 0){
        //是一堆变量定义
        checkExtDecList(node->kids[1], type);
    }
    else if(node->kids[1]->token_name.compare("FunDec") == 0){
        //是一个函数定义
        
    }
}

//finished
void checkExtDefList(parseTree *node){
    if(node->kids_num > 0){
        checkExtDef(node->kids[0]);
    }
    if(node->kids_num > 1){
        checkExtDefList(node->kids[1]);
    }
}

//finished
void semanticCheck(parseTree *root){
    out = stdout;
    current_scope_level = 0;
    if(root->kids_num > 0){
        checkExtDefList(root->kids[0]);
    }
}