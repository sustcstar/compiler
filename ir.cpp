#include"ir.hpp"
#include"semantic.hpp"
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include<stdarg.h>
#include <sstream>

#define IR_DEBUG 0
#define LINE_MAX_LENGTH 100

//问题1：处理else if

//当前目标，过sample/01test
//计划：1. 过完全部样例
    //   2. 检查exp中可能漏掉的

FILE *irout;
// std::vector<TAC> tacs;
int place;
int label;

//finished
void ir_log(const char *str, ...){
#if IR_DEBUG==1
    va_list args;
    va_start(args, str);
    vfprintf(irout, str, args);
    va_end(args);
#endif
}

//finished
std::string formatToString(const char *str, ...){
    char cline[LINE_MAX_LENGTH];
    va_list args;
    va_start(args, str);
    vsprintf(cline, str, args);
    va_end(args);
    std::string str_cline(cline);
    return str_cline;
}

//finished
void initializeIR(){
    place = 0;
    label = 0;
    irout = stdout;
}

//finished
int new_place(){
    return place++;
}

//finished
int new_label(){
    return label++;
}

//finished
int to_int(parseTree *node){
    return node->attribute.value_attribute;
}

//TODO：暂时只考虑那四个玩意儿
std::string ir_cond_exp(parseTree *node, int lb_true, int lb_false){
    if(node->kids_num == 3){
        if(node->kids[1]->token_name.compare("LT") == 0){
            int t1 = new_place();
            int t2 = new_place();
            std::string code1 = irExp(node->kids[0], t1);
            std::string code2 = irExp(node->kids[2], t2);
            std::string code3 = formatToString("IF t%d < t%d GOTO label%d\n", t1, t2, lb_true) \
                                + formatToString("GOTO label%d\n", lb_false);
            return code1 + code2 + code3;
        }
        else if(node->kids[1]->token_name.compare("LE") == 0){
            int t1 = new_place();
            int t2 = new_place();
            std::string code1 = irExp(node->kids[0], t1);
            std::string code2 = irExp(node->kids[2], t2);
            std::string code3 = formatToString("IF t%d <= t%d GOTO label%d\n", t1, t2, lb_true) \
                                + formatToString("GOTO label%d\n", lb_false);
            return code1 + code2 + code3;
        }
        else if(node->kids[1]->token_name.compare("GT") == 0){
            int t1 = new_place();
            int t2 = new_place();
            std::string code1 = irExp(node->kids[0], t1);
            std::string code2 = irExp(node->kids[2], t2);
            std::string code3 = formatToString("IF t%d > t%d GOTO label%d\n", t1, t2, lb_true) \
                                + formatToString("GOTO label%d\n", lb_false);
            return code1 + code2 + code3;
        }
        else if(node->kids[1]->token_name.compare("GE") == 0){
            int t1 = new_place();
            int t2 = new_place();
            std::string code1 = irExp(node->kids[0], t1);
            std::string code2 = irExp(node->kids[2], t2);
            std::string code3 = formatToString("IF t%d >= t%d GOTO label%d\n", t1, t2, lb_true) \
                                + formatToString("GOTO label%d\n", lb_false);
            return code1 + code2 + code3;
        }
        else if(node->kids[1]->token_name.compare("NE") == 0){
            int t1 = new_place();
            int t2 = new_place();
            std::string code1 = irExp(node->kids[0], t1);
            std::string code2 = irExp(node->kids[2], t2);
            std::string code3 = formatToString("IF t%d != t%d GOTO label%d\n", t1, t2, lb_true) \
                                + formatToString("GOTO label%d\n", lb_false);
            return code1 + code2 + code3;
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

//finished TODO:目前只考虑整数参数
std::string irArgs(parseTree *node, std::vector<int>* arg_list){
    if(node->kids_num == 1){
        //Exp
        int tp = new_place();
        std::string code = irExp(node->kids[0], tp);
        arg_list->insert(arg_list->begin(), tp);
        return code;
    }
    else{
        //Exp COMMA Args
        int tp = new_place();
        std::string code1 = irExp(node->kids[0], tp);
        arg_list->insert(arg_list->begin(), tp);
        std::string code2 = irArgs(node->kids[2], arg_list);
        return code1 + code2;
    }
}

//TODO:左值目前只考虑ID
std::string irExp(parseTree *node, int place){
    ir_log("Line %d: In irExp, place = t%d\n", node->lineno, place);
    if(node->kids_num == 1){
        if(node->kids[0]->token_name.compare("INT") == 0){
            ir_log("irExp -> INT\n");
            int value = to_int(node->kids[0]);
            ir_log("t%d := #%d\n", place, value);
            return formatToString("t%d := #%d\n", place, value);
        }
        else if(node->kids[0]->token_name.compare("ID") == 0){
            std::string variable(node->kids[0]->attribute.str_attribute);
            return formatToString("t%d := %s\n", place, variable.c_str());
        }
    }
    else if(node->kids_num == 3){
        if(node->kids[1]->token_name.compare("ASSIGN") == 0){
            ir_log("Line %d: In irExp->ASSIGN\n", node->lineno);
            std::string variable(node->kids[0]->kids[0]->attribute.str_attribute);
            int tp = new_place();
            std::string code1 = irExp(node->kids[2], tp);
            std::string code2 = formatToString("%s := t%d\n", variable.c_str(), tp);
            std::string code3 = formatToString("t%d := %s\n", place, variable.c_str());
            ir_log("Line %d: In irExp->ASSIGN, Just before RETURN\n", node->lineno);
            return code1 + code2 + code3;
        }
        else if(node->kids[1]->token_name.compare("PLUS") == 0 || node->kids[1]->token_name.compare("MINUS") == 0 \
        || node->kids[1]->token_name.compare("MUL") == 0 || node->kids[1]->token_name.compare("DIV") == 0){
            int t1 = new_place();
            int t2 = new_place();
            ir_log("Line %d: token_name = %s, 1 = t%d, 2 = t%d\n", node->lineno, node->kids[1]->token_name.c_str(), \
            t1, t2);
            std::string code1 = irExp(node->kids[0], t1);
            std::string code2 = irExp(node->kids[2], t2);
            std::string code3;
            if(node->kids[1]->token_name.compare("PLUS") == 0){
                code3 = formatToString("t%d := t%d + t%d\n", place, t1, t2);
            }
            else if(node->kids[1]->token_name.compare("MINUS") == 0){
                code3 = formatToString("t%d := t%d - t%d\n", place, t1, t2);
            }
            else if(node->kids[1]->token_name.compare("MUL") == 0){
                code3 = formatToString("t%d := t%d * t%d\n", place, t1, t2);
            }
            else{
                code3 = formatToString("t%d := t%d / t%d\n", place, t1, t2);
            }
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
            ir_log("Line %d: In irExp->ID LP RP\n", node->lineno);
            std::string function_name(node->kids[0]->attribute.str_attribute);
            if(function_name.compare("read") == 0){
                ir_log("Line %d: READ\n", node->lineno);
                return formatToString("READ t%d\n", place);
            }
            else{
                return formatToString("t%d := CALL %s\n", place, function_name.c_str());
            }
        }
        else if(node->kids[0]->token_name.compare("LP") == 0){
            // LP Exp RP
            return irExp(node->kids[1], place);
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
            ir_log("Line %d: WRITE\n", node->lineno);
            int tp = new_place();
            return irExp(node->kids[2]->kids[0], tp) + formatToString("WRITE t%d\n", tp);
        }
        else{
            std::vector<int> arg_list;
            std::string code1 = irArgs(node->kids[2], &arg_list);
            std::string code2("");
            for(auto it = arg_list.begin(); it != arg_list.end(); it++){ 
                code2 = code2 + formatToString("ARG t%d\n", *it);
            }
            return code1 + code2 + formatToString("t%d := CALL %s\n", place, function_name.c_str());
        }
    }
}

//TODO: ELSE不一定翻译正确，毕竟没检查过
std::string irStmt(parseTree *node){
    ir_log("Line %d: In irStmt\n", node->lineno);
    if(node->kids_num == 1){
        // CompSt
        return irCompSt(node->kids[0]);
    }
    else if(node->kids_num == 2){
        ir_log("Line %d: In irStmt, before Exp SEMI\n", node->lineno);
        // Exp SEMI
        int tp = new_place();
        return irExp(node->kids[0], tp);
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

//fnished
int irVarList(parseTree *node){
    int count = 1;
    if(node->kids_num > 1){
        return count + irVarList(node->kids[2]);
    }
}

//finished TODO：目前参数列表只有int类型
std::string irFunDec(parseTree *node){
    if(node->kids_num == 3){
        // ID LP RP
        std::string function_name(node->kids[0]->attribute.str_attribute);
        ir_log("FUNCTION %s :\n", function_name.c_str());
        return formatToString("FUNCTION %s :\n", function_name.c_str());
    }
    else{
        // ID LP VarList RP
        std::string function_name(node->kids[0]->attribute.str_attribute);
        std::string code1 = formatToString("FUNCTION %s :\n", function_name.c_str());
        int varnum = irVarList(node->kids[2]);
        std::string code2("");
        for(int i = 0; i < varnum; i++){
            int tp = new_place();
            code2 = code2 + formatToString("PARAM t%d\n", tp);
        }
        return code1 + code2;
    }
}

//finished
std::string irDec(parseTree *node){
    ir_log("Line %d: In irDec\n", node->lineno);
    // VarDec ASSIGN Exp
    // TODO: VarDec只能是ID
    if(node->kids_num > 1){
        std::string variable(node->kids[0]->kids[0]->attribute.str_attribute);
        int tp = new_place();
        std::string code1 = irExp(node->kids[2], tp);
        ir_log("%s := t%d\n", variable.c_str(), tp);
        std::string code2 = formatToString("%s := t%d\n", variable.c_str(), tp);
        return code1 + code2;
    }
    else{
        // VarDec 只有定义则返回空串
        std::string empty("");
        return empty;
    }
}

//finished
std::string irDecList(parseTree *node){
    // Dec COMMA DecList
    std::string code = irDec(node->kids[0]);
    if(node->kids_num > 1){
        code = code + irDecList(node->kids[2]);
    }
    return code;
}

//finished TODO:只考虑int类型
std::string irDef(parseTree *node){
    // Specifier DecList SEMI
    return irDecList(node->kids[1]);
}

//finshed
std::string irDefList(parseTree *node){
    ir_log("Line %d: In irDefList\n", node->lineno);
    std::string code = irDef(node->kids[0]);
    if(node->kids_num > 1){
        code = code + irDefList(node->kids[1]);
    }
    return code;
}

//finished
std::string irStmtList(parseTree *node){
    // Stmt
    std::string code = irStmt(node->kids[0]);
    // Stmt StmtList
    if(node->kids_num > 1){
        code = code + irStmtList(node->kids[1]);
    }
    return code;
}

//finished
std::string irCompSt(parseTree *node){
    if(node->kids_num == 3){    //直接StmtList
        std::string code = irStmtList(node->kids[1]);
        return code;
    }
    else{  
        std::string code1 = irDefList(node->kids[1]); 
        std::string code2 = irStmtList(node->kids[2]);
        return code1 + code2;
    }
}

//finished TODO:只考虑函数定义
std::string irExtDef(parseTree *node){ 
    if(node->kids_num == 3){
        if(node->kids[1]->token_name.compare("FunDec") == 0){
            std::string code1 = irFunDec(node->kids[1]);
            ir_log("After irFunDec\n");
            std::string code2 = irCompSt(node->kids[2]);
            return code1 + code2;
        }
    }
}

//finished
std::string irExtDefList(parseTree *node){
    std::string code("");
    if(node->kids_num > 0){
        code = code + irExtDef(node->kids[0]);
    }
    if(node->kids_num > 1){
        code = code + irExtDefList(node->kids[1]);
    }
    return code;
}

//finished
void irProgram(parseTree *root){
    std::string code;
    initializeIR();
    if(root->kids_num > 0){
        ir_log("before irExtDefList\n");
        code = irExtDefList(root->kids[0]);
    }
    //TODO：打印TACs
    fprintf(irout, "%s", code.c_str());
}
