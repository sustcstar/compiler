#include"ir.hpp"
#include"semantic.hpp"
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include<stdarg.h>
#include <sstream>

#define IR_DEBUG 0
#define LINE_MAX_LENGTH 100

//当前目标：只考虑int main()以及所有假设
//注意1：这次没有全局变量，所以可能不需要考虑ext的一些东西

FILE *irout;
// std::vector<TAC> tacs;
int place;
int label;

std::string formatToString(const char *str, ...){
    char cline[LINE_MAX_LENGTH];
    va_list args;
    va_start(args, str);
    sprintf(cline, str, args);
    va_end(args);
    std::string stringline(cline);
    return stringline;
}

void initializeIR(){
    place = 0;
    label = 0;
}

int new_place(){
    return place++;
}

int new_label(){
    return label++;
}

int to_int(parseTree *node){
    return node->attribute.value_attribute;
}

//TODO：暂时只考虑那四个玩意儿
std::string ir_cond_exp(parseTree *node, int lb_true, int lb_false){
    if(node->kids_num == 3){
        if(node->kids[1]->token_name.compare("LT") == 0){

        }
        else if(node->kids[1]->token_name.compare("LE") == 0){

        }
        else if(node->kids[1]->token_name.compare("GT") == 0){

        }
        else if(node->kids[1]->token_name.compare("GE") == 0){

        }
        else if(node->kids[1]->token_name.compare("NE") == 0){

        }
        else if(node->kids[1]->token_name.compare("EQ") == 0){
            int t1 = new_place();
            int t2 = new_place();
            std::string code1 = irExp(node->kids[0], t1);
            std::string code2 = irExp(node->kids[2], t2);
            std::string code3 = formatToString("IF t%d == t%d GOTO label%d\n", t1, t2, lb_true) \
                                + formatToString("GOTO label%d\n", lb_false);
            return code1 + code2 + code3;

        }
        else if(node->kids[1]->token_name.compare("AND") == 0){
            int lb1 = new_label();
            std::string code1 = ir_cond_exp(node->kids[0], lb1, lb_false) + formatToString("LABEL label%d :\n", lb1);
            std::string code2 = ir_cond_exp(node->kids[2], lb_true, lb_false);
            return code1 + code2;
        }
        else if(node->kids[1]->token_name.compare("OR") == 0){
            int lb1 = new_label();
            std::string code1 = ir_cond_exp(node->kids[0], lb_true, lb1) + formatToString("LABEL label%d :\n", lb1);
            std::string code2 = ir_cond_exp(node->kids[2], lb_true, lb_false);
            return code1 + code2;
        }
    }
    else{
        //not exp
        return ir_cond_exp(node->kids[1], lb_false, lb_true);
    }
}

//TODO:目前只考虑整数参数
std::string irArgs(parseTree *node, std::vector<int> arg_list){

}

//TODO:左值目前只考虑ID
std::string irExp(parseTree *node, int place){
    if(node->kids_num == 1){
        if(node->kids[0]->token_name.compare("INT") == 0){
            int value = to_int(node->kids[0]);
            return formatToString("t%d := #%d\n", place, value);
        }
        else if(node->kids[0]->token_name.compare("ID") == 0){
            std::string variable(node->kids[0]->attribute.str_attribute);
            return formatToString("t%d := %s\n", place, variable.c_str());
        }
    }
    else if(node->kids_num == 3){
        if(node->kids[1]->token_name.compare("ASSIGN") == 0){
            std::string variable(node->kids[0]->kids[0]->attribute.str_attribute);
            int tp = new_place();
            std::string code1 = irExp(node->kids[2], tp);
            std::string code2 = formatToString("%s := t%d\n", variable.c_str(), tp);
            std::string code3 = formatToString("t%d := %s\n", place, variable.c_str());
            return code1 + code2 + code3;
        }
        else if(node->kids[1]->token_name.compare("PLUS") == 0){
            int t1 = new_place();
            int t2 = new_place();
            std::string code1 = irExp(node->kids[0], t1);
            std::string code2 = irExp(node->kids[2], t2);
            std::string code3 = formatToString("t%d := t%d + t%d\n", place, t1, t2);
            return code1 + code2 + code3;
        }
        // 条件表达式
        else if(node->kids[1]->token_name.compare("LT") == 0 || node->kids[1]->token_name.compare("LE") == 0 \
        || node->kids[1]->token_name.compare("GT") == 0 || node->kids[1]->token_name.compare("GE") == 0 \
        || node->kids[1]->token_name.compare("NE") == 0 || node->kids[1]->token_name.compare("EQ") == 0 \
        || node->kids[1]->token_name.compare("AND") == 0 || node->kids[1]->token_name.compare("OR") == 0){
            int lb1 = new_label();
            int lb2 = new_label();
            std::string code0 = formatToString("t%d := #0\n", place);
            std::string code1 = ir_cond_exp(node, lb1, lb2);
            std::string code2 = formatToString("LABEL label%d :\n", lb1) \
                                + formatToString("t%d := #1\n", place) \
                                + formatToString("LABEL label%d :\n", lb2);
            return code0 + code1 + code2;
        }
        else if(node->kids[0]->token_name.compare("ID") == 0){
            // ID LP RP
            std::string function_name(node->kids[0]->attribute.str_attribute);
            if(function_name.compare("read") == 0){
                return formatToString("READ t%d\n", place);
            }
            else{
                return formatToString("t%d := CALL %s\n", place, function_name.c_str());
            }
        }
    }
    else if(node->kids_num == 2){
        if(node->kids[0]->token_name.compare("MINUS") == 0){
            int tp = new_place();
            std::string code1 = irExp(node->kids[1], tp);
            std::string code2 = formatToString("t%d := #0 - t%d\n", place, tp);
            return code1 + code2;
        }
        //条件exp，NOT Exp
        else if(node->kids[0]->token_name.compare("NOT") == 0){
            int lb1 = new_label();
            int lb2 = new_label();
            std::string code0 = formatToString("t%d := #0\n", place);
            std::string code1 = ir_cond_exp(node, lb1, lb2);
            std::string code2 = formatToString("LABEL label%d :\n", lb1) \
                                + formatToString("t%d := #1\n", place) \
                                + formatToString("LABEL label%d :\n", lb2);
            return code0 + code1 + code2;
        }
    }
    else{ //4 TODO：目前不考虑数组
        // ID LP Args RP
        std::string function_name(node->kids[0]->attribute.str_attribute);
        if(function_name.compare("write") == 0){
            int tp = new_place();
            return irExp(node->kids[2], tp) + formatToString("WRITE t%d\n", tp);
        }
        else{
            // std::vector<>
        }
    }
}

//TODO: ELSE不一定翻译正确，毕竟没检查过
std::string irStmt(parseTree *node){
    if(node->kids_num == 1){
        // CompSt

    }
    else if(node->kids_num == 2){
        // Exp SEMI
    }
    else if(node->kids_num == 3){
        // RETURN Exp SEMI
        int tp = new_place();
        std::string code = irExp(node->kids[1], tp);
        return code + formatToString("RETURN t%d\n", tp);
    }
    else if(node->kids_num == 5){
        if(node->kids[0]->token_name.compare("IF") == 0){
            // IF LP Exp RP Stmt
            int lb1 = new_label();
            int lb2 = new_label();
            std::string code1 = ir_cond_exp(node->kids[2], lb1, lb2) \
                                + formatToString("LABEL label%d :\n", lb1);
            std::string code2 = irStmt(node->kids[4]) + formatToString("LABEL label%d :\n", lb2);
            return code1 + code2;
        }
        else{
            // WHILE LP Exp RP Stmt
            int lb1 = new_label();
            int lb2 = new_label();
            int lb3 = new_label();
            std::string code1 = formatToString("LABEL label%d :\n", lb1) + ir_cond_exp(node->kids[2], lb2, lb3);
            std::string code2 = formatToString("LABEL label%d :\n", lb2) + irStmt(node->kids[4]) \
                                + formatToString("GOTO label%d\n", lb1);
            return code1 + code2 + formatToString("LABEL label%d :\n", lb3);
        }
    }
    else{
        // IF LP Exp RP Stmt ELSE Stmt
        int lb1 = new_label();
        int lb2 = new_label();
        int lb3 = new_label();
        std::string code1 = ir_cond_exp(node->kids[2], lb1, lb2) \
                            + formatToString("LABEL label%d :\n", lb1);
        std::string code2 = irStmt(node->kids[4]) + formatToString("GOTO label%d\n", lb3) \
                            + formatToString("LABEL label%d :\n", lb2);
        std::string code3 = irStmt(node->kids[6]) + formatToString("LABEL label%d :\n", lb3);
        return code1 + code2 + code3;
    }
}

// //finished
// void ir_debug_log(const char *str, ...){
// #if  IR_DEBUG==1
//     va_list args;
//     va_start(args, str);
//     vfprintf(out, str, args);
//     va_end(args);
// #endif
// }

// void irProgram(parseTree *root){
//     irout = stdout;
//     tacs.clear();

//     if(root->kids_num > 0){
//         irExtDefList(root->kids[0]);
//     }
//     //TODO：打印TACs
// }

// //finished
// void irExtDefList(parseTree *node){
//     if(node->kids_num > 0){
//         irExtDef(node->kids[0]);
//     }
//     if(node->kids_num > 1){
//         irExtDefList(node->kids[1]);
//     }
// }

// //暂时不用管的事情
// //1. 返回类型，这里不考虑
// //2. 不考虑全局变量
// //3. TODO：（将来要修改）目前只考虑int main()
// //finished
// void irExtDef(parseTree *node){ 
//     if(node->kids[1]->token_name.compare("FunDec") == 0){
//         auto function = irFunDec(node->kids[1]);
//         irCompSt(node->kids[2]);
//     }
// }

// //finished
// void irCompSt(parseTree *node){
//     if(node->kids_num == 3){    //直接StmtList
//         irStmtList(node->kids[1]);
//     }
//     else{  
//         irDefList(node->kids[1]); 
//         irStmtList(node->kids[2], returnType);
//     }
// }

// //finished
// void irDefList(parseTree *node){ 
//     auto fields = irDef(node->kids[0]);
//     if(node->kids_num > 1){ 
//         auto subfieldList = irDefList(node->kids[1]);
//     }
// }

// //finished
// void irDef(parseTree *node){
//     Type *type;
//     type = checkSpecifier(node->kids[0]); //获得这个变量的类型
//     auto fields = irDecList(node->kids[1], type);
// }

// //finished
// void irDecList(parseTree *node, Type *type){
//     auto field = irDec(node->kids[0], type);
//     if(node->kids_num > 1){ //irDecList
//         auto fieldSubList = irDecList(node->kids[2], type);
//     }
// }

// void irDec(parseTree *node, Type *type){
//     field = irVarDec(node->kids[0], type);
//     if(node->kids_num > 1){ 
//         irExp(node->kids[2]);
//     }
// }

// //TODO：目前只考虑整数
// void irVarDec(parseTree *node, Type *type){
//     Type *array = NULL;
//     if(node->kids_num == 1){ // 就一个ID,没有数组
//         //把id返回上来，接下来就可以构成<id, type> pair
//         auto id = VarDec_onlyID(node, type);
//         return make_pair(id, type);
//     }
// }

//----分界线



// int current_scope_level;
// std::map<std::string, Type *> structure_table; //用来存用户定义类型的，检测是否重复定义了类型
// std::stack<std::map<std::string, Type *>> symbol_stack;

// std::vector<>

// //finished
// void debug_print_symbol_map(){
// #if  IR_DEBUG==1 
//     int count = 0;
//     auto symbol_table = accessSymbolTable(); //ired
//     for (auto iter=symbol_table.begin(); iter!=symbol_table.end(); iter++){
//         count++;
//         ir_debug_log("symbol_table: key%d: %s\n", count, iter->first.c_str());
//     }
// #endif
// }

// //finished
// void debug_print_structure_map(){
// #if  IR_DEBUG==1 
//     int count = 0;

//     for (auto iter=structure_table.begin(); iter!=structure_table.end(); iter++){
//         count++;
//         ir_debug_log("structure_table: key%d: %s\n", count, iter->first.c_str());
//     }
// #endif
// }

// //finished, 有就返回Type，没有就返回NULL
// Type *stringToType(std::string name){
//     // 查询符号表
//     auto symbol_table = accessSymbolTable(); //ired
//     auto iter = symbol_table.find(name);
//     if(iter != symbol_table.end()){ //2. 这个id已经存在
//         return iter->second;
//     }
//     else{                //3. 这个id不存在
//         return NULL;
//     }
// }

// //finished
// void putAMapIntoSymbolTable(std::map<std::string, Type *> themap, parseTree *node){
//     auto symbol_table = accessSymbolTable(); //ired
//     for (auto themap_iter=themap.begin(); themap_iter!=themap.end(); themap_iter++){
//         auto symbol_iter = symbol_table.find(themap_iter->first);
//         // if(symbol_iter != symbol_table.end()){  //symbol_table中已有这个key
//         //     // std::cout<<"Ooooooops!!!, irList return multiple defined variables!"<<std::endl;
//         // }
//         // else{   //symbol_table中没有这个key，插入
//         changeSymbolTable((*themap_iter).first, (*themap_iter).second, node->lineno);  //ired
//         symbol_table = accessSymbolTable();
//         // }
//     }
// }

// //finished-2
// bool key_in_map(std::map<std::string, Type *> themap, std::string key){
//     bool flag;

//     auto iter = themap.find(key);
//     if(iter != themap.end()){ //2. 这个id已经存在
//         flag = true;
//     }
//     else{                //3. 这个id不存在
//         flag = false;
//     }
//     return flag;
// }

// // 大工程：revoke Assumption 6, thus variables in dierent scopes can share the same identier,
// // and variables defined in the outer scope will be shadowed by the inner variables with
// // the same identifier.
// // 解决：使用一个stack + symboltable
// // 1. 每次进入一个compst, current_scope_level+1, 复制一个新的symboltable，push入stack (修改地，所有的compst)
// // 2. 如果1中前面有funcDec定义，则把参数列表也放进stack.top (修改地，所有的compst)
// // 3. 每次放进去前，看看是否已经有同名变量，若有，则覆盖它，若没有，直接插入(compst里的def，以及外部的def)
// // 4. 从compst出来时,current_scope_level-1, stack pop 一个table (修改地，所有的compst)
// // note:目前所有涉及到symbol_table的地方很可能都要修改

// //finished-2
// bool changeSymbolTable(std::string key, Type *type, int lineno){
//     std::map<std::string, Type *> table = symbol_stack.top();
//     bool flag;

//     symbol_stack.pop();
//     auto iter = table.find(key);
//     if(iter != table.end()){
//         //1. 如果该key存在
//         if(iter->second->scope_level < current_scope_level){
//             //1-1. 如果scope_level < 当前，则覆盖
//             table[key] = type;
//             flag = true;
//         }
//         else{
//             //1-2. 否则，不插入并报错
//             // ir_debug_log("Oppps, a BIG UNEXPECTED_BEHAVIOR");
//             reportError(out, T3_VAR_REDEF, lineno);
//             flag = false;
//         }
//     }
//     else{
//         //2. 如果該key不存在，直接插入
//         table[key] = type;
//         flag = true;
//     }
//     symbol_stack.push(table);
//     return flag;
// }

// //finished
// Type *keyToType(std::string key){
//     std::map<std::string, Type *> table = symbol_stack.top();
//     auto iter = table.find(key);
//     if(iter != table.end()){
//         return NULL;
//     }
//     else{
//         return (iter->second);
//     }
// }

// //finished-2
// std::map<std::string, Type *> accessSymbolTable(){
//     return symbol_stack.top();
// }

// //finished-2
// void outofCompst(){
//     //1. 减少scope_level
//     current_scope_level--;
//     //2. stack pop 一个table
//     symbol_stack.pop();
// }

// //finished
// void initGlobalScope(){
//     current_scope_level = 0;
//     std::map<std::string, Type *> symbol_table;
//     symbol_stack.push(symbol_table);
// }

// //finished-2
// void enterCompst(Function *funDec){
//     //1. 提高scope_level
//     current_scope_level++;
//     //2. 复制顶端符号表
//     auto symbol_table = symbol_stack.top();
//     //3. 把funDec里的参数表放进符号表
//     if(funDec != NULL){
//         auto fun_args = funDec->args;
//         for(auto iter = fun_args.begin(); iter != fun_args.end(); iter++){
//             if(key_in_map(symbol_table, iter->first)){
//                 //3-1. 符号表中已经有这个参数，覆盖它，这个时候所有的scope_level都比它小，所以可以直接覆盖
//                 symbol_table[iter->first] = iter->second;
//             }
//             else{
//                 //3-2. 符号表中没有这个参数，插入它
//                 symbol_table.insert(*iter);
//             }
//         }
//     }
//     //4. 把新的符号表放进stack里
//     symbol_stack.push(symbol_table);
// }

// //finished
// bool variableDefined(std::string name){
//     bool flag;
//     // 查询符号表
//     auto symbol_table = accessSymbolTable(); //ired
//     auto iter = symbol_table.find(name);
//     if(iter != symbol_table.end()){ //2. 这个id已经存在
//         flag = true;
//     }
//     else{                //3. 这个id不存在
//         flag = false;
//     }
//     return flag;
// }

// //finished 是类型定义重复，不是id重复
// bool structDefined(std::string name){
//     bool flag;
//     // 查询用户定义类型表
//     auto iter = structure_table.find(name);
//     if(iter != structure_table.end()){ //2. 这个id已经存在
//         flag = true;
//     }
//     else{                //3. 这个id不存在
//         flag = false;
//     }
//     return flag;
// }

// //finished
// std::string VarDec_onlyID(parseTree *node, Type *type){
//     //1. 查看这个id是否已经存在于symbol_table
//     std::string id(node->kids[0]->attribute.str_attribute);
//     // auto iter = symbol_table.find(id);
//     // if(iter != symbol_table.end()){ //2. 这个id已经存在，则报错
//     // }
//     // else{                //3. 这个id不存在，则插入它
//     //     symbol_table[id] = type;
//     // }
//     return id;
// }

// //1. 生成整个Type只能在上层, 所以必须把id传给上层
// //2. 即便不把id传给上层，在上层通过specifer也能知道type
// //3. 但总之，总结性工作（插表）只能在上层做
// //finished
// Type *VarDec_Array(parseTree *node, Type *type){
//     Type *base = NULL;
//     Array *array = NULL;
//     int size = 0;
//     //可以是ID，也可以是ID[INT]
//     if(node->kids_num == 1){ 
//         // 递归终止，需要传给上层的参数有
//         // 由于这里没法插入id，只能让上层插入，所以要传给上层
//         // 1. id
//         // 建议：把id放进Type(传下来的参数type)里，作为一个name, 
//         type->name = node->kids[0]->attribute.str_attribute;
//         return type;
//     }
//     else{ 
//         // 递归继续，需要传给上层的参数有
//         // 1. 我这一层的size，以及下层的Type(也就是我这一层的base), 可以封装成一个Array一起传上去
//         base = VarDec_Array(node->kids[0], type);
//         size = node->kids[2]->attribute.value_attribute;
//         array = new Array(base->name, current_scope_level, base, size);
//         return array;
//     }
// }

// //finished
// Type *irPrimitive(parseTree *node){
//     ir_debug_log("In irPrimitive\n");
//     ir_debug_log("node->token_name = %s\n", node->token_name.c_str());
//     std::string value(node->token_name);
//     Type *primitive;

//     if(value.compare("INT") == 0){
//         primitive = new Type("", current_scope_level, Type::INT);
//     }
//     else if(value.compare("FLOAT") == 0){
//         primitive = new Type("", current_scope_level, Type::FLOAT);
//     }
//     else if(value.compare("CHAR") == 0){
//         primitive = new Type("", current_scope_level, Type::CHAR);
//     }
//     return primitive;
// }

// //finished
// bool isExpLvalue(parseTree *node){
//     if(node->kids_num == 1){
//         // ID 是左值，没问题
//         if(node->kids[0]->token_name.compare("ID") == 0){
//             return true;
//         }
//         else{
//             return false;
//         }
//     }
//     else if(node->kids_num == 3){
//         // Exp DOT ID 要求Exp也是左值
//         if(node->kids[0]->token_name.compare("Exp") == 0 && \
//             node->kids[1]->token_name.compare("DOT") == 0 && \
//             node->kids[2]->token_name.compare("ID") == 0){
//             return isExpLvalue(node->kids[0]);
//         }
//         else{
//             return false;
//         }
//     }
//     else if(node->kids_num == 4){
//         // Exp LB Exp RB 要求左边的Exp也是左值
//         if(node->kids[0]->token_name.compare("Exp") == 0 && \
//             node->kids[1]->token_name.compare("LB") == 0 && \
//             node->kids[2]->token_name.compare("Exp") == 0 && \
//             node->kids[3]->token_name.compare("RB") == 0){
//             return isExpLvalue(node->kids[0]);
//         }
//         else{
//             return false;
//         }
//     }
//     else{
//         return false;
//     }
// }

// //finished
// bool canExpand(Type *type1, Type *type2){
//     if((type1->type_category != Type::INT && type1->type_category != Type::FLOAT) \
//     || (type2->type_category != Type::INT && type2->type_category != Type::FLOAT)){
//         return false;
//     }
//     else{
//         if(type1->type_category == Type::FLOAT){
//             return true;
//         }
//     }
// }

// //finished 目前只管int和float
// Type *expandType(Type *type1, Type *type2, int lineno, int expand_type){
//     if((type1->type_category != Type::INT && type1->type_category != Type::FLOAT) \
//     || (type2->type_category != Type::INT && type2->type_category != Type::FLOAT)){
//         reportError(out, T7_UNMATCH_OPERANDS, lineno);
//         return NULL;
//     }
//     else{
//         if(type1->type_category == expand_type){
//             return type1;
//         }
//         else if(type2->type_category == expand_type){
//             return type2;
//         }
//         else{
//             return type1;
//         }
//     }
// }

// //finished, 返回一个结构体的member，若member不存在，返回NULL
// Type *getMember(Type *structure, std::string memberid){
//     std::map<std::string, Type *> fieldList = dynamic_cast<Structure *>(structure)->fieldList;
//     auto iter = fieldList.find(memberid);
//     if(iter != fieldList.end()){ //2. 这个memberid已经存在
//         return iter->second;
//     }
//     else{                //3. 这个memberid不存在
//         return NULL;
//     }
// }

// //finished Args: Exp COMMA Args | Exp 每个arg是一个exp
// std::vector<Type *> irArgs(parseTree *node){
//     std::vector<Type *> args;
//     Type *expType = irExp(node->kids[0]);
//     if(node->kids_num > 1){
//         args = irArgs(node->kids[2]);
//     }
//     args.insert(args.begin(), expType);
//     return args;
// }

// //finished
// bool isArgsMatch(Type *tmp, std::vector<Type *> tmpargs){
//     Function *function = dynamic_cast<Function *>(tmp);
//     auto function_args = function->args;
//     int count = function_args.size();
//     // ir_debug_log("function_args.size = %d, tmpargs.size = %d\n", function_args.size(), tmpargs.size());
//     if(function_args.size() == tmpargs.size()){
//         for(int i = 0; i < count; i++){
//             // std::cout<<"function_args.at(i).second: "<<function_args.at(i).second->type_category \
//             // << " tmpargs.at(i): "<<tmpargs.at(i)->type_category<<std::endl;
//             if(tmpargs.at(i) == NULL){
//                 //使用的实参有一部分没定义
//                 return false;
//             }
//             if(*(function_args.at(i).second) != *(tmpargs.at(i))){
//                 if(canExpand((function_args.at(i).second), (tmpargs.at(i)))){
//                     return true;
//                 }
//                 else{
//                     return false;
//                 }
//             }
//         }
//         return true;
//     }
//     else{
//         return false;
//     }
// }



// //finished 
// Type *irStructSpecifier(parseTree *node){ //用户定义类型报错统一在这里进行
//     Type *structure = NULL;

//     if(node->kids_num > 2){ //1.检查是不是已经有这个定义了 2.定义类型 3. 插入这个定义
//         std::string id(node->kids[1]->attribute.str_attribute);
//         if(structDefined(id)){ //1.检查是不是已经有这个定义了，若有则报错
//             reportError(out, T15_STRUCTURE_REDEF, node->lineno);
//         }
//         else{ //2. 若没有，则定义类型，并且 3.插入定义
//             ir_debug_log("In irStructSpecifier, before irDefList\n");
//             enterCompst(NULL);
//             auto fieldList = irDefList(node->kids[3]);
//             outofCompst();
//             ir_debug_log("In irStructSpecifier, After irDefList\n");
//             structure = new Structure(id, current_scope_level, fieldList);
//             structure_table[id] = structure;
//         }
//         debug_print_structure_map();
//     }
//     else{ //1. 假设类型已存在，根据ID返回类型 
//         std::string id(node->kids[1]->attribute.str_attribute);
//         if(structDefined(id)){
//             structure = structure_table[id];
//         }
//         else{ //2. 类型不存在，报错
//             reportError(out, T16_STRUCTURE_NODEF, node->lineno);
//         }
//     }
//     return structure;
// }

// //finished
// Type *irTYPE(parseTree *node){
//     std::string value(node->attribute.str_attribute);
//     Type *primitive;

//     if(value.compare("int") == 0){
//         primitive = new Type("", current_scope_level, Type::INT);
//     }
//     else if(value.compare("float") == 0){
//         primitive = new Type("", current_scope_level, Type::FLOAT);
//     }
//     else if(value.compare("char") == 0){
//         primitive = new Type("", current_scope_level, Type::CHAR);
//     }
//     return primitive;
// }

// //finished, 这里应该返回一个vector, 将下层的变量传上去
// std::vector<std::pair<std::string, Type *>> irExtDecList(parseTree *node, Type *type){
//     std::vector<std::pair<std::string, Type *>> variableList;
//     //返回的是一个变量pair
//     auto variable = irVarDec(node->kids[0], type); //这里不是同一插入层-1
//     if(node->kids_num > 1){
//         auto variables = irExtDecList(node->kids[2], type);
//         variableList = variables;
//     }
//     variableList.push_back(variable);
//     return variableList;
// }

// //finished 这个函数的目的就是返回一个pair
// std::pair<std::string, Type *> irParamDec(parseTree *node){
//     Type *type;

//     type = irSpecifier(node->kids[0]);
//     return irVarDec(node->kids[1], type);
// }

// //finished
// bool paramNameExist(std::vector<std::pair<std::string, Type *>> paramList,std::string id){
//     for (auto iter = paramList.begin(); iter != paramList.end(); iter++){
//         if(iter->first.compare(id) == 0){
//             return true;
//         }
//     }
//     return false;
// }

// //finished 
// std::vector<std::pair<std::string, Type *>> irVarList(parseTree *node){
//     std::vector<std::pair<std::string, Type *>> paramList;
//     // //我希望irParamDec能够返回一个<string, type *> pair
//     auto param = irParamDec(node->kids[0]);
//     if(node->kids_num > 1){ //irVarList
//         auto paramSubList = irVarList(node->kids[2]);
//         paramList = paramSubList;
//     }
//     if(paramNameExist(paramList, param.first)){ //形参的名字重复啦！
//         reportError(out, T3_VAR_REDEF, node->lineno);
//         // paramList.clear(); TODOFINISHED形参名字，重复，不清空，只是不插入而已
//     }
//     else{
//         paramList.insert(paramList.begin(), param); //形参名字不重复，插入参数列表
//     }
//     return paramList;
// }

// //finished
// std::pair<std::string, Type *> irFunDec(parseTree *node, Type *type){
//     Type *function;
//     //1. 检查变量名是否重复
//     // ir_debug_log("func_id = 0x%px.\n", node->kids[1]->attribute.str_attribute);
//     std::string id(node->kids[0]->attribute.str_attribute);
//     ir_debug_log("func_id = %s.\n", id.c_str());
//     if(variableDefined(id)){
//         reportError(out, T4_FUNC_REDEF, node->lineno);
//         //重复了，先报个错 TODOFINISHED
//     }
//     //2. 检查是否有形参
//     if(node->kids_num == 3){ //无形参
//         std::vector<std::pair<std::string, Type *>> paramList;
//         function = new Function(id, type, paramList);
//     }
//     else if(node->kids_num == 4){ //有形参    //3. 检查形参互相之间是否有重复
//         auto paramList = irVarList(node->kids[2]);
//         //如果下层出现问题，还是返回完整的参数列表  TODOFINISHED
//         function = new Function(id, type, paramList);
//     } 
//     //4. 向上层返回Type *, 如果函数有错误，则返回NULL
//     return make_pair(id, function);
// }

// //  和exp一样，是最复杂的部分，结合test一起来做吧
// void irStmt(parseTree *node, Type *returnType){
//     if(node->kids_num == 1){
//         // CompSt
//         enterCompst(NULL);
//         irCompSt(node->kids[0], returnType);
//         outofCompst();
//     }
//     else if(node->kids_num == 2){
//         // Exp SEMI
//         irExp(node->kids[0]);
//     }
//     else if(node->kids_num == 3){
//         // RETURN Exp SEMI
//         Type *tmp = irExp(node->kids[1]);
//         //TODO: 这里可能会有问题
//         ir_debug_log("When return, the tmp = 0x%px\n", tmp);
//         if(returnType == NULL){
//             ir_debug_log("return_type == NULL\n");
//         }
//         if(tmp == NULL || *tmp != *returnType){
//             reportError(out, T8_FUNC_RETURN_UNMATCH_DECLARED, node->lineno);
//         }
//     }
//     else if(node->kids_num == 5){
//         if(node->kids[0]->token_name.compare("IF") == 0){
//             // IF LP Exp RP Stmt
//             irExp(node->kids[2]);
//             irStmt(node->kids[4], returnType);
//         }
//         else{
//             // WHILE LP Exp RP Stmt
//             irExp(node->kids[2]);
//             irStmt(node->kids[4], returnType);
//         }
//     }
//     else{
//         // IF LP Exp RP Stmt ELSE Stmt
//         irExp(node->kids[2]);
//         irStmt(node->kids[4], returnType);
//         irStmt(node->kids[6], returnType);
//     }
// }

// //finished  funDec中包含了你这个函数的参数列表和返回值
// void irStmtList(parseTree *node, Type *returnType){
//     if(node->kids_num == 1){
//         //最后一句，需要有返回值？ 不需要！
//         irStmt(node->kids[0], returnType);
//     }
//     else if(node->kids_num == 2){
//         irStmt(node->kids[0], returnType);
//         irStmtList(node->kids[1], returnType);
//     }
//     else{
//         // StmtList = %empty do nothing?
//     }
// }

