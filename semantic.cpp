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
//假设9：函数定义时必须带着compst
//假设10：函数可以没有返回值（哪怕定义的时候要求了返回值），但是一旦使用return，必须与定义匹配
// Assumption 1 char variables only occur in assignment operations or function param-
// eters/arguments
// Assumption 2 only int variables can do boolean operations
// Assumption 3 only int and float variables can do arithmetic operations

//实现1：实现基本类型（无赋值）的插入符号表以及检查是否重复（名称）
//实现2：实现基本类型多维数组（无赋值）的插入符号表以及检查是否重复（名称）
//实现3：实现结构体（无赋值）的插入符号表以及检查是否重复（名称）\
        结构体内的成员变量不赋值，且不会使用int a,b,c这种连续定义（一行定义一个） 已经实现连续定义
//实现4：实现基本类型、多维数组、以及结构体的单行多次定义，包括结构体内部的参数的单行多次定义（包括重复检测）
//实现5：实现函数定义，并检查函数名重复性，以及参数名重复性，参数可以是结构体
//实现6：实现在一个compst内的定义变量(暂时没有scope)
//实现7：在表达式仅为基本类型时（不包括ID），可以用于初始化变量。
//预备8：完成compst框架，包括while/if/else等关键字
//预备9: 照着测试样例填补exp框架
//预备10：一个函数的参数列表在compst中应该算作已经定义

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

//问题5：一行连续两个赋值（=）怎么报错？

//问题6：如果不匹配，报个错, 可以返回NULL也可以返回最左边的exp，看情况挑选

//问题7：在初始化赋值中，可以使用checkexp把exp的类型返回上来，也可以在checkexp中做所有事情
//目前解决：在checkexp中解决所有事情

//问题8：没有过一行定义一行，应该改为每扫描一行，那一行就定义好了。

//问题9：在末尾的注释会卡住程序
//想法（已采纳）：测试用例中没有注释所以不管。

//问题10：if...else...if这个需要确认

//注意1：所有有迟疑的地方都用TODO标注了

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

//finished, 有就返回Type，没有就返回NULL
Type *stringToType(std::string name){
    // 查询符号表
    auto iter = symbol_table.find(name);
    if(iter != symbol_table.end()){ //2. 这个id已经存在
        return iter->second;
    }
    else{                //3. 这个id不存在
        return NULL;
    }
}

//finished
void putAMapIntoSymbolTable(std::map<std::string, Type *> themap, parseTree *node){
    for (auto themap_iter=themap.begin(); themap_iter!=themap.end(); themap_iter++){
        auto symbol_iter = symbol_table.find(themap_iter->first);
        if(symbol_iter != symbol_table.end()){  //symbol_table中已有这个key
            // std::cout<<"Ooooooops!!!, checkList return multiple defined variables!"<<std::endl;
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

//finished
bool isExpLvalue(parseTree *node){
    if(node->kids_num == 1){
        // ID 是左值，没问题
        if(node->kids[0]->token_name.compare("ID") == 0){
            return true;
        }
        else{
            return false;
        }
    }
    else if(node->kids_num == 3){
        // Exp DOT ID 要求Exp也是左值
        if(node->kids[0]->token_name.compare("Exp") == 0 && \
            node->kids[1]->token_name.compare("DOT") == 0 && \
            node->kids[2]->token_name.compare("ID") == 0){
            return isExpLvalue(node->kids[0]);
        }
        else{
            return false;
        }
    }
    else if(node->kids_num == 4){
        // Exp LB Exp RB 要求左边的Exp也是左值
        if(node->kids[0]->token_name.compare("Exp") == 0 && \
            node->kids[1]->token_name.compare("LB") == 0 && \
            node->kids[2]->token_name.compare("Exp") == 0 && \
            node->kids[3]->token_name.compare("RB") == 0){
            return isExpLvalue(node->kids[0]);
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}

//finished 目前只管int和float
Type *expandType(Type *type1, Type *type2, int lineno, int return_type_category){
    if((type1->type_category != Type::INT && type1->type_category != Type::FLOAT) \
    || (type2->type_category != Type::INT && type2->type_category != Type::FLOAT)){
        reportError(out, T7_UNMATCH_OPERANDS, lineno);
        return NULL;
    }
    else{
        if(type1->type_category == return_type_category){
            return type1;
        }
        if(type2->type_category == return_type_category){
            return type1;
        }
    }
}

//finished, 返回一个结构体的member，若member不存在，返回NULL
Type *getMember(Type *structure, std::string memberid){
    std::map<std::string, Type *> fieldList = dynamic_cast<Structure *>(structure)->fieldList;
    auto iter = fieldList.find(memberid);
    if(iter != fieldList.end()){ //2. 这个memberid已经存在
        return iter->second;
    }
    else{                //3. 这个memberid不存在
        return NULL;
    }
}

//type为该表达式应该匹配的类型
//成功，则返回exp所代表的类型
// 1. 检验左右类型是否匹配
// 2. 检验各类ID是否定义
// 3. 两个不同值域的类型进行运算，结果会自动转换为值域较大的类型。\
    char 1个字节， int,float4个字节，double 8个。
// 4. 意思是赋值号左侧的是只能是变量，不能是表达式。
// 5. 如果不匹配，报个错, 可以返回NULL也可以返回最左边的exp，看情况挑选
Type *checkExp(parseTree *node){
    debug_log("line %d: exp->kids_num = %d\n", node->lineno, node->kids_num);
    if(node->kids_num == 1){
        // ID/INT/FLOAT/CHAR
        debug_log("exp->kids[0]->token_name = %s\n", node->kids[0]->token_name.c_str());
        if(node->kids[0]->token_name.compare("ID") == 0){
            //根据ID获得Type, 再进行比较
            std::string id(node->kids[0]->attribute.str_attribute);
            auto pair = symbol_table.find(id);
            if(pair != symbol_table.end()){
                //1. ID存在, 获得variable并且返回type
                return pair->second;
            }
            else{
                //2. ID不存在，报错, 返回NULL
                reportError(out, T1_VAR_USED_NO_DEF, node->lineno);
                return NULL;
            }
        }
        else{ //INT/FLOAT/CHAR, 构造一个类型，再返回
            debug_log("line %d: Before checkPrimitive\n", node->lineno);
            Type *primitive = checkPrimitive(node->kids[0]);
            debug_log("After checkPrimitive\n");
            return primitive;
        }
    }
    else if(node->kids_num == 2){
        // MINUS Exp/NOT Exp
        // TODO: 这个先不管，等下照着test样例来填充
    }
    else if(node->kids_num == 3){
        // Assumption 1 char variables only occur in assignment operations or function param-
        // eters/arguments
        // Assumption 2 only int variables can do boolean operations
        // Assumption 3 only int and float variables can do arithmetic operations
        debug_log("line %d: The expression is %s %s %s\n", node->lineno, node->kids[0]->token_name.c_str(), \
        node->kids[1]->token_name.c_str(), node->kids[2]->token_name.c_str());
        if(node->kids[0]->token_name.compare("Exp") == 0 && \
            node->kids[2]->token_name.compare("Exp") == 0){
                //1. 提取左右表达式的类型
                if(node->kids[1]->token_name.compare("ASSIGN") == 0){
                    // Exp ASSIGN Exp
                    debug_log("Before Exp ASSIGN Exp\n");
                    if(isExpLvalue(node->kids[0])){ //ASSIGN左边是左值
                        Type *type1 = checkExp(node->kids[0]);
                        Type *type2 = checkExp(node->kids[2]);
                        if(type1 != NULL && type2 != NULL){
                            if(*type1 != *type2){
                                reportError(out, T5_UNMATCH_TYPE_ASSIGN, node->lineno);
                            }
                        }
                    }
                    else{ //ASSIGN左边不是左值
                        reportError(out, T6_RVAL_ON_ASSIGN_LEFT, node->lineno);
                        return NULL; //TODO
                    }
                    debug_log("After Exp ASSIGN Exp\n");
                }
                else{
                    // Exp AND Exp/Exp OR Exp/Exp LT Exp
                    // Exp LE Exp/Exp GT Exp/Exp GE Exp/Exp NE Exp
                    // Exp EQ Exp/Exp PLUS Exp/Exp MINUS Exp/Exp MUL Exp
                    // Exp DIV Exp/
                    // 把文档自己提供的假设当成真的
                    std::string oper(node->kids[1]->token_name);
                    if(oper.compare("AND") == 0 || oper.compare("OR") == 0){
                        //布尔运算 左右两边都得是int
                        Type *type1 = checkExp(node->kids[0]);
                        Type *type2 = checkExp(node->kids[2]);
                        if(type1 != NULL && type2 != NULL){
                            if(type1->type_category != Type::INT \
                            || type2->type_category != Type::INT){
                                reportError(out, T7_UNMATCH_OPERANDS, node->lineno);
                                return NULL;
                            }
                        }
                        else{
                            return NULL;
                        }
                    }
                    else if(oper.compare("LT") == 0 || oper.compare("LE") == 0 \
                    || oper.compare("GT") == 0 || oper.compare("GE") == 0 \
                    || oper.compare("NE") == 0 || oper.compare("EQ") == 0){
                        //关系运算 左右两边都得是int和float, 扩张为int
                        Type *type1 = checkExp(node->kids[0]);
                        Type *type2 = checkExp(node->kids[2]);
                        if(type1 != NULL && type2 != NULL){
                            //返回值可以是int,float,NULL
                            return expandType(type1, type2, node->lineno, Type::INT);
                        }
                        else{
                            return NULL;
                        }
                    }
                    else{
                        //算术运算 左右两边都得是int和float，扩张为float
                        Type *type1 = checkExp(node->kids[0]);
                        Type *type2 = checkExp(node->kids[2]);
                        if(type1 != NULL && type2 != NULL){
                            //返回值可以是int,float,NULL
                            return expandType(type1, type2, node->lineno, Type::FLOAT);
                        }
                        else{
                            return NULL;
                        }
                    }
                }
        }
        else{
            // LP Exp RP/ID LP RP/Exp DOT ID
            // TODO: 这个先不管，等下照着test样例来填充  
            if(node->kids[0]->token_name.compare("LP") == 0){
                return checkExp(node->kids[1]);
            } 
            else if(node->kids[0]->token_name.compare("ID") == 0){
                //查看ID是否已经声明
                std::string key(node->kids[0]->attribute.str_attribute);
                if(key_in_map(symbol_table, key)){ //若已经声明，返回函数返回值作为类型
                    return stringToType(key);
                }
                else{ //若没有，报错，返回NULL
                    reportError(out, T2_FUNC_USED_NO_DEF, node->lineno);
                    return NULL;
                }
            }
            else{
                //1. Exp必须是已经定义且是结构体，否则就报“T13_ACCESS_MEMBER_NON_STRUCTURE”错误
                Type *structure = checkExp(node->kids[0]);
                if(structure && structure->type_category == Type::STRUCTURE){
                    //2. member必须是该结构体的成员变量
                    std::string memberid(node->kids[2]->attribute.str_attribute);
                    Type *member = NULL;
                    if(member = getMember(structure, memberid)){
                        //是member, 则返回member的类型
                        return member;
                    }
                    else{
                        //不是member，则报错，返回NULL
                        reportError(out, T14_ACCESS_NONDEF_STRUCTURE_MEMBER, node->lineno);
                        return NULL;
                    }
                }
                else{
                    reportError(out, T13_ACCESS_MEMBER_NON_STRUCTURE, node->lineno);
                    return NULL;
                }
            }
        }
    }
    else if(node->kids_num == 4){
        // ID LP Args RP/Exp LB Exp RB
        // TODO: 这个先不管，等下照着test样例来填充
        if(node->kids[0]->token_name.compare("ID") == 0){
            //查看ID是否已经声明
            std::string key(node->kids[0]->attribute.str_attribute);
            if(key_in_map(symbol_table, key)){ //若已经声明，返回函数返回值作为类型
                //还需要检查Args里的各类东西1.是否定义 2.类型是否match
                //TODO: 关于这个，看看有没有测试用例吧
                return stringToType(key);
            }
            else{ //若没有，报错，返回NULL
                reportError(out, T2_FUNC_USED_NO_DEF, node->lineno);
                return NULL;
            }
        } 
        else{
            
        }
    }
    else{
        debug_log("Oooooops! Unexpected behavior!\n");
    }
    //其余情况都是意外情况，返回NULL
    return NULL;
}

//finished 我希望checkDec能够返回一个<string, type *> pair
std::pair<std::string, Type *> checkDec(parseTree *node, Type *type){
    std::pair<std::string, Type *> field;

    //我希望checkVarDec能够返回一个<string, type *> pair
    field = checkVarDec(node->kids[0], type);
    if(node->kids_num > 1){ // 右边有赋值操作,目前只需检查右边表达式是否和type匹配
        //如果不匹配，报个错就完事了
        Type *expType = checkExp(node->kids[2]);
        if(expType != NULL){
            if( *type != *expType){ //如果不匹配
                reportError(out, T5_UNMATCH_TYPE_ASSIGN, node->lineno);
            }
        }
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
    putAMapIntoSymbolTable(fields, node);
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
            // reportError(out, T3_VAR_REDEF, node->lineno);
            //已经在下一层报错过，先不管TODO
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

//  和exp一样，是最复杂的部分，结合test一起来做吧
void checkStmt(parseTree *node, Function *funDec){
    if(node->kids_num == 1){
        // CompSt
        checkCompSt(node->kids[0], funDec);
    }
    else if(node->kids_num == 2){
        // Exp SEMI
        checkExp(node->kids[0]);
    }
    else if(node->kids_num == 3){
        // RETURN Exp SEMI
    }
    else if(node->kids_num == 5){
        if(node->kids[0]->token_name.compare("IF") == 0){
            // IF LP Exp RP Stmt
            checkExp(node->kids[2]);
            checkStmt(node->kids[4], funDec);
        }
        else{
            // WHILE LP Exp RP Stmt
            checkExp(node->kids[2]);
            checkStmt(node->kids[4], funDec);
        }
    }
    else{
        // IF LP Exp RP Stmt ELSE Stmt
        checkExp(node->kids[2]);
        checkStmt(node->kids[4], funDec);
        checkStmt(node->kids[6], funDec);
    }
}

//finished  funDec中包含了你这个函数的参数列表和返回值
void checkStmtList(parseTree *node, Function *funDec){
    if(node->kids_num == 1){
        //最后一句，需要有返回值？ 不需要！
        checkStmt(node->kids[0], funDec);
    }
    else if(node->kids_num == 2){
        checkStmt(node->kids[0], funDec);
        checkStmtList(node->kids[1], funDec);
    }
    else{
        // StmtList = %empty do nothing?
    }
}

//finished
void checkCompSt(parseTree *node, Function *funDec){
    debug_log("line %d: kids_num = %d\n", node->lineno, node->kids_num);
    debug_log("line %d: node->name = %s\n", node->lineno, node->token_name.c_str());
    if(node->kids_num == 3){    //直接StmtList
        checkStmtList(node->kids[1], funDec);
    }
    else{   //先DefList再StmtList 
        auto variableList = checkDefList(node->kids[1]); //假设已经在下一层检查过
        // putAMapIntoSymbolTable(variableList, node); //把它们插入symbol_table
        //已经在下一层放进去过，这个等到扩张scope的时候再说
        debug_print_symbol_map();
        checkStmtList(node->kids[2], funDec);
    }
}

//
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
        //TODO：在这里判断return Type是否match
        debug_log("In checkExtDef, before checkFunDec.\n");
        auto function = checkFunDec(node->kids[1], type);
        debug_log("In checkExtDef, after checkFunDec.\n");
        if(function.second != NULL){ //在下层已经检查过，知道没问题 TODO: 我认为即使，函数定义有错，compst也得检查一下
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
