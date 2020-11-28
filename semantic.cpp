#include"semantic.hpp"
#include"reportError.hpp"
#include <stdarg.h>
#include <map>

#define SE_DEBUG 1

//假设1：不会在函数内部定义结构体, 只有全局定义结构体
//假设2：只有全局scope
//假设3：结构体成员变量不包括结构体
//假设4：数组和结构体不会初始化
//假设5：全局变量不会在定义的时候初始化
//假设6：没有void返回值的函数
//假设7：变量名和函数名不可重复
//假设8：暂时默认函数名相同，则两个函数相等

//实现1：实现基本类型（无赋值）的插入符号表以及检查是否重复（名称）
//实现2：实现基本类型多维数组（无赋值）的插入符号表以及检查是否重复（名称）
//实现3：实现结构体（无赋值）的插入符号表以及检查是否重复（名称）\
        结构体内的成员变量不赋值，且不会使用int a,b,c这种连续定义（一行定义一个） 已经实现连续定义
//实现4：实现基本类型、多维数组、以及结构体的单行多次定义，包括结构体内部的参数的单行多次定义（包括重复检测）
//实现5：实现函数定义，并检查函数名重复性，以及参数名重复性，参数可以是结构体
//实现6：实现在一个compst内的定义变量(暂时没有scope)
//实现7：在表达式仅为基本类型时（不包括ID），可以用于初始化变量。
//预备8：实现exp框架，之后再修复(实现这行可以去掉)

//改进1：可以把插入表的地方放在一个高层，统一起来，下层的函数都是返回变量pair,\
        下层都用vector返回pair,上层则用map看搜集，看是否有重复，看情况报错 --已经完成

//改进2：每过一行变量定义，就把该行的变量们都插入符号表，这样下一行就可以继续调用这一行的变量。
//想法1：先这么放着

//问题1：结构体内部的同名参数不会报错
//想法1：我认为结构体内部的定义和函数内部的定义是一个东西，所以只要在checkDefList \
        这个函数里统一报错即可 ---已经根据这个想法解决问题

//问题2：后定义的重复变量不会报错，先定义的重复变量会报错 
//想法1：通过在type中加入lineno，即可记录每一个type出现时的行数，这样只要改变少许代码 \
        就能报后边的重复变量了
//想法2：或者把一个fiedlist作为参数传下去，也可以达到目标
//想法3：等解決了函數内部定义变量再考虑这个问题，因为这两个会互相影响

//问题3：checkDefList不会会返回包含有同名变量的list，但是可能会返回symbol_table中\
        已经存在的变量名，且报错行数有错误
//暂时的解决：使用oooooops在代码中作为一个醒目的标志，先不管这个，等引入scope后再管

//问题4：varDec在被定义的时候，即便右边exp检查不对劲，仍会返回该变量，该变量仍算作被定义
//解决：就这样吧

FILE *out;
int current_scope_level;
std::map<std::string, Type *> symbol_table; //用来存变量的，检测变量名是否重合
std::map<std::string, Type *> structure_table; //用来存用户定义类型的，检测是否重复定义了类型

//finished
void debug_log(const char *str, ...){
#if  SE_DEBUG==1
    va_list args;
    va_start(args, str);
    vfprintf(out, str, args);
    va_end(args);
#endif
}

//finished
void debug_print_symbol_map(){
#if  SE_DEBUG==1 
    int count = 0;

    for (auto iter=symbol_table.begin(); iter!=symbol_table.end(); iter++){
        count++;
        debug_log("symbol_table: key%d: %s\n", count, iter->first.c_str());
    }
#endif
}

//finished
void debug_print_structure_map(){
#if  SE_DEBUG==1 
    int count = 0;

    for (auto iter=structure_table.begin(); iter!=structure_table.end(); iter++){
        count++;
        debug_log("structure_table: key%d: %s\n", count, iter->first.c_str());
    }
#endif
}

//finished
void putAMapIntoSymbolTable(std::map<std::string, Type *> themap, parseTree *node){
    for (auto themap_iter=themap.begin(); themap_iter!=themap.end(); themap_iter++){
        auto symbol_iter = symbol_table.find(themap_iter->first);
        if(symbol_iter != symbol_table.end()){  //symbol_table中已有这个key
            std::cout<<"Ooooooops!!!, checkList return multiple defined variables!"<<std::endl;
            reportError(out, T3_VAR_REDEF, node->lineno);
        }
        else{   //symbol_table中没有这个key，插入
            symbol_table.insert(*themap_iter);
        }
    }
}

//finished
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

//finished
Type *checkPrimitive(parseTree *node){
    debug_log("In checkPrimitive\n");
    debug_log("node->token_name = %s\n", node->token_name.c_str());
    std::string value(node->token_name);
    Type *primitive;

    if(value.compare("INT") == 0){
        primitive = new Type("", current_scope_level, Type::INT);
    }
    else if(value.compare("FLOAT") == 0){
        primitive = new Type("", current_scope_level, Type::FLOAT);
    }
    else if(value.compare("CHAR") == 0){
        primitive = new Type("", current_scope_level, Type::CHAR);
    }
    return primitive;
}

//如果不匹配，报个错就完事了。type为该表达式应该匹配的类型
void checkExp(parseTree *node, Type *type){
    debug_log("exp->kids_num = %d\n", node->kids_num);
    if(node->kids_num == 1){
        // ID/INT/FLOAT/CHAR
        debug_log("exp->kids[0]->token_name = %s\n", node->kids[0]->token_name.c_str());
        if(node->kids[0]->token_name.compare("ID") == 0){
            //根据ID获得Type, 再进行比较
            std::string id(node->kids[0]->attribute.str_attribute);
            auto pair = symbol_table.find(id);
            if(pair != symbol_table.end()){
                //1. ID存在, 获得Type并比较
                if(type == pair->second){ //1-1. 两个类型相等，返回
                    return;
                }
                else{ //1-2.两个类型不相等，报错
                    reportError(out, T5_UNMATCH_TYPE_ASSIGN, node->lineno);
                }
            }
            else{
                //2. ID不存在，报错
                reportError(out, T1_VAR_USED_NO_DEF, node->lineno);
            }
        }
        else{ //INT/FLOAT/CHAR, 构造一个类型，再进行Type之间的比较
            debug_log("Before checkPrimitive\n");
            Type *primitive = checkPrimitive(node->kids[0]);
            debug_log("After checkPrimitive\n");
            if(*type == *primitive){
                debug_log("type == primitive\n");
                return;
            }
            else{
                debug_log("type != primitive\n");
                reportError(out, T5_UNMATCH_TYPE_ASSIGN, node->lineno);
            }
        }
    }
    else if(node->kids_num == 2){
        // MINUS Exp/NOT Exp
    }
    else if(node->kids_num == 3){
        // Exp ASSIGN Exp/Exp AND Exp/Exp OR Exp/Exp LT Exp
        // Exp LE Exp/Exp GT Exp/Exp GE Exp/Exp NE Exp
        // Exp EQ Exp/Exp PLUS Exp/Exp MINUS Exp/Exp MUL Exp
        // Exp DIV Exp/LP Exp RP/ID LP RP/Exp DOT ID
    }
    else if(node->kids_num == 4){
        // ID LP Args RP/Exp LB Exp RB
    }
    else{
        debug_log("Oooooops! Unexpected behavior!\n");
    }
}

//finished 我希望checkDec能够返回一个<string, type *> pair
std::pair<std::string, Type *> checkDec(parseTree *node, Type *type){
    std::pair<std::string, Type *> field;

    //我希望checkVarDec能够返回一个<string, type *> pair
    field = checkVarDec(node->kids[0], type);
    if(node->kids_num > 1){ // 右边有赋值操作,目前只需检查右边表达式是否和type匹配
        //如果不匹配，报个错就完事了
        checkExp(node->kids[2], type);
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

//finished, 这个函数的目的就是返回一个pair
std::pair<std::string, Type *> checkParamDec(parseTree *node){
    Type *type;

    type = checkSpecifier(node->kids[0]);
    return checkVarDec(node->kids[1], type);
}

//finished
std::map<std::string, Type *> checkVarList(parseTree *node){
    std::map<std::string, Type *> paramList;
    // //我希望checkParamDec能够返回一个<string, type *> pair
    auto param = checkParamDec(node->kids[0]);
    if(node->kids_num > 1){ //checkVarList
        auto paramSubList = checkVarList(node->kids[2]);
        paramList = paramSubList;
    }
    if(key_in_map(paramList, param.first)){ //形参的名字重复啦！
        reportError(out, T3_VAR_REDEF, node->lineno);
        paramList.clear();
    }
    else{
        paramList.insert(param); //形参名字不重复，插入参数列表
    }
    return paramList;
}

//finished
std::pair<std::string, Type *> checkFunDec(parseTree *node, Type *type){
    Type *function;
    //1. 检查变量名是否重复
    // debug_log("func_id = 0x%px.\n", node->kids[1]->attribute.str_attribute);
    std::string id(node->kids[0]->attribute.str_attribute);
    debug_log("func_id = %s.\n", id.c_str());
    if(variableDefined(id)){
        reportError(out, T4_FUNC_REDEF, node->lineno);
        function = NULL;
        return make_pair(id, function);
    }
    //2. 检查是否有形参
    if(node->kids_num == 3){ //无形参
        std::map<std::string, Type *> paramList;
        function = new Function(id, type, paramList);
    }
    else if(node->kids_num == 4){ //有形参    //3. 检查形参互相之间是否有重复
        auto paramList = checkVarList(node->kids[2]);
        if(paramList.size() == 0){ //如果下层出现问题，则返回空空的参数列表
            function = NULL;
            return make_pair(id, function);
        }
        else{
            function = new Function(id, type, paramList);
        }
    } 
    //4. 向上层返回Type *, 如果函数有错误，则返回NULL
    return make_pair(id, function);
}

void checkStmtList(parseTree *node, Function *funDec){

}

void checkCompSt(parseTree *node, Function *funDec){
    debug_log("kids_num = %d\n", node->kids_num);
    debug_log("node->name = %s\n", node->token_name.c_str());
    if(node->kids_num == 3){    //直接StmtList
        checkStmtList(node->kids[1], funDec);
    }
    else{   //先DefList再StmtList 
        auto variableList = checkDefList(node->kids[1]); //假设已经在下一层检查过
        putAMapIntoSymbolTable(variableList, node); //把它们插入symbol_table
        debug_print_symbol_map();
        checkStmtList(node->kids[2], funDec);
    }
}

//finished
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
        debug_log("In checkExtDef, before checkFunDec.\n");
        auto function = checkFunDec(node->kids[1], type);
        debug_log("In checkExtDef, after checkFunDec.\n");
        if(function.second != NULL){ //在下层已经检查过，知道没问题
            symbol_table.insert(function);
            Function *funDec = (Function *)function.second;
            debug_log("In checkExtDef, before checkCompSt.\n");
            checkCompSt(node->kids[2], funDec);
            debug_log("In checkExtDef, after checkCompSt.\n");
        }
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