#include"semantic.hpp"
#include"reportError.hpp"
#include <stdarg.h>
#include <map>
#define SE_DEBUG 1

//假设1：不会在函数内部定义结构体, 只有全局定义结构体
//假设2：只有全局scope
//假设3：结构体成员变量不包括结构体

//实现1：实现基本类型（无赋值）的插入符号表以及检查是否重复（名称）
//实现2：实现基本类型多维数组（无赋值）的插入符号表以及检查是否重复（名称）
//实现3：实现结构体（无赋值）的插入符号表以及检查是否重复（名称）\
        结构体内的成员变量不赋值，且不会使用int a,b,c这种连续定义（一行定义一个）
//实现4：实现基本类型、多维数组、以及结构体的单行多次定义，包括结构体内部的参数的单行多次定义（包括重复检测）

//改进1：可以把插入表的地方放在一个高层，统一起来，下层的函数都是返回变量pair,\
        下层都用vector返回pair,上层则用map看搜集，看是否有重复，看情况报错 --已经完成

//问题1：结构体内部的同名参数不会报错
//想法1：我认为结构体内部的定义和函数内部的定义是一个东西，所以只要在checkDefList \
        这个函数里统一报错即可 ---已经根据这个想法解决问题

//问题2：后定义的重复变量不会报错，先定义的重复变量会报错 
//想法1：通过在type中加入lineno，即可记录每一个type出现时的行数，这样只要改变少许代码 \
        就能报后边的重复变量了
//想法2：或者把一个fiedlist作为参数传下去，也可以达到目标
//想法3：等解決了函數内部定义变量再考虑这个问题，因为这两个会互相影响

FILE *out;
int current_scope_level;
std::map<std::string, Type *> symbol_table; //用来存变量的，检测变量名是否重合
std::map<std::string, Type *> structure_table; //用来存用户定义类型的，检测是否重复定义了类型

void debug_log(const char *str, ...){
#if  SE_DEBUG==1
    va_list args;
    va_start(args, str);
    vfprintf(out, str, args);
    va_end(args);
#endif
}

void debug_print_symbol_map(){
#if  SE_DEBUG==1 
    int count = 0;

    for (auto iter=symbol_table.begin(); iter!=symbol_table.end(); iter++){
        count++;
        debug_log("symbol_table: key%d: %s\n", count, iter->first.c_str());
    }
#endif
}

void debug_print_structure_map(){
#if  SE_DEBUG==1 
    int count = 0;

    for (auto iter=structure_table.begin(); iter!=structure_table.end(); iter++){
        count++;
        debug_log("structure_table: key%d: %s\n", count, iter->first.c_str());
    }
#endif
}

bool key_in_map(std::map<std::string, Type *> themap, std::string key){
    bool flag;

    auto iter = themap.find(key);
    if(iter != themap.end()){ //2. 这个id已经存在
        flag = true;
    }
    else{                //3. 这个id不存在
        flag = false;
    }
    return flag;
}

//finished
bool variableDefined(std::string name){
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


//finished 是类型定义重复，不是id重复
bool structDefined(std::string name){
    bool flag;
    // 查询用户定义类型表
    auto iter = structure_table.find(name);
    if(iter != structure_table.end()){ //2. 这个id已经存在
        flag = true;
    }
    else{                //3. 这个id不存在
        flag = false;
    }
    return flag;
}

//finished
std::string VarDec_onlyID(parseTree *node, Type *type){
    //1. 查看这个id是否已经存在于symbol_table
    std::string id(node->kids[0]->attribute.str_attribute);
    // auto iter = symbol_table.find(id);
    // if(iter != symbol_table.end()){ //2. 这个id已经存在，则报错
    //     reportError(out, T3_VAR_REDEF, node->lineno);
    // }
    // else{                //3. 这个id不存在，则插入它
    //     symbol_table[id] = type;
    // }
    return id;
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
std::pair<std::string, Type *> checkVarDec(parseTree *node, Type *type){
    Type *array = NULL;

    //可以是ID，也可以是ID[INT]
    if(node->kids_num == 1){ // 就一个ID,
        //把id返回上来，接下来就可以构成<id, type> pair
        auto id = VarDec_onlyID(node, type);
        return make_pair(id, type);
    }
    else{ // 可以有数组 注意，有可能是多维数组
        //返回了一个Type *,且array->name为id
        array = VarDec_Array(node, type);
        return make_pair(array->name, array);
        // auto iter = symbol_table.find(array->name);
        // if(iter != symbol_table.end()){ //2. 这个id已经存在，则报错
        //     reportError(out, T3_VAR_REDEF, node->lineno);
        // }
        // else{                //3. 这个id不存在，则插入它
        //     symbol_table[array->name] = array;
        // }
    }
}

// 我希望checkDec能够返回一个<string, type *> pair
std::pair<std::string, Type *> checkDec(parseTree *node, Type *type){
    std::pair<std::string, Type *> field;

    if(node->kids_num == 1){ // 右边没有赋值操作
        //我希望checkVarDec能够返回一个<string, type *> pair
        field = checkVarDec(node->kids[0], type);
    }
    else{ // 右边有赋值操作, 目前只需检查右边表达式是否和type匹配

    }
    return field;
}

//finished, 我希望checkDecList返回的是一个std::map<std::string, Type *>
std::map<std::string, Type *> checkDecList(parseTree *node, Type *type){
    std::map<std::string, Type *> fieldList;
    //我希望checkDec能够返回一个<string, type *> pair
    auto field = checkDec(node->kids[0], type);
    if(node->kids_num > 1){ //checkDecList
        auto fields = checkDecList(node->kids[2], type);
        fieldList = fields;
    }
    fieldList.insert(field);
    return fieldList;
}

//finished
std::map<std::string, Type *> checkDef(parseTree *node){
    Type *type;

    type = checkSpecifier(node->kids[0]); //获得这个变量的类型
    //我希望checkDecList返回的是一个std::map<std::string, Type *>
    auto fields = checkDecList(node->kids[1], type);
    return fields;
}

//finished 一个DefList是很多行定义，一个Def就是一行定义，一个DecList指的是一行里连续定义的variables
std::map<std::string, Type *> checkDefList(parseTree *node){ //为structure的建立搜集参数列表信息
    std::map<std::string, Type *> fieldList;

    //我希望checkDef能够返回一个<string, type *> map
    auto fields = checkDef(node->kids[0]);
    if(node->kids_num > 1){ //checkDefList
        //checkDefList返回的是一个std::map<std::string, Type *>
        auto subfieldList = checkDefList(node->kids[1]);
        fieldList = subfieldList;
    }
    //检查两个list里定义的变量是否有重复
    for (auto iter=fields.begin(); iter!=fields.end(); iter++){
        if(key_in_map(fieldList, iter->first)){ //变量重复，报错
            reportError(out, T3_VAR_REDEF, node->lineno);
        }
        else{ //变量不重复，插入fieldList
            fieldList.insert(*iter);
        }
    }
    return fieldList;
}

//finished 
Type *checkStructSpecifier(parseTree *node){ //用户定义类型报错统一在这里进行
    Type *structure = NULL;

    debug_log("In checkStructSpecifier.\n");
    if(node->kids_num > 2){ //1.检查是不是已经有这个定义了 2.定义类型 3. 插入这个定义
        std::string id(node->kids[1]->attribute.str_attribute);
        if(structDefined(id)){ //1.检查是不是已经有这个定义了，若有则报错
            reportError(out, T15_STRUCTURE_REDEF, node->lineno);
        }
        else{ //2. 若没有，则定义类型，并且 3.插入定义
            auto fieldList = checkDefList(node->kids[3]);
            structure = new Structure(id, current_scope_level, fieldList);
            structure_table[id] = structure;
        }
        debug_print_structure_map();
    }
    else{ //1. 假设类型已存在，根据ID返回类型 
        std::string id(node->kids[1]->attribute.str_attribute);
        if(structDefined(id)){
            structure = structure_table[id];
        }
        else{ //2. 类型不存在，报错
            reportError(out, T16_STRUCTURE_NODEF, node->lineno);
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

//finished, 这里应该返回一个vector, 将下层的变量传上去
std::vector<std::pair<std::string, Type *>> checkExtDecList(parseTree *node, Type *type){
    std::vector<std::pair<std::string, Type *>> variableList;
    //返回的是一个变量pair
    auto variable = checkVarDec(node->kids[0], type); //这里不是同一插入层-1
    if(node->kids_num > 1){
        auto variables = checkExtDecList(node->kids[2], type);
        variableList = variables;
    }
    variableList.push_back(variable);
    return variableList;
}

void checkExtDef(parseTree *node){ //这里作为统一插入层比较好，代表着global_scope每一行的定义
    Type *type;

    debug_log("In checkExtDef, before checkSpecifier.\n");
    type = checkSpecifier(node->kids[0]); //其实就是返回类型
    debug_log("In checkExtDef, after checkSpecifier.\n");
    if(node->kids[1]->token_name.compare("ExtDecList") == 0){
        //是一行变量定义，它们都发生在同一行
        auto variableList = checkExtDecList(node->kids[1], type);
        for(auto variable : variableList){
            if(variableDefined(variable.first)){
                //1. 该变量名已经被使用，报错
                reportError(out, T3_VAR_REDEF, node->lineno);
            }
            else{
                //2. 该变量名未被使用，插入符号表
                symbol_table.insert(variable);
            }
        }
        debug_print_symbol_map();
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