#include "reportError.hpp"
#include <iostream>
#include <sstream>

// Error type [TypeID] at Line [LineNo]: [message]
void reportError(FILE *out, int INFO_type, int lineno){

    std::string message;
    std::stringstream ss;

    ss.str("");
    ss << "Error type %d at Line %d: ";

    switch(INFO_type){
        case T1_VAR_USED_NO_DEF:
            ss << "variable is used without definition";
            break;
        case T2_FUNC_USED_NO_DEF:
            ss << "function is invoked without definition";
            break;
        case T3_VAR_REDEF:
            ss << "variable is redefined in the same scope";
            break;
        case T4_FUNC_REDEF:
            ss << "function is redefined";
            break;
        case T5_UNMATCH_TYPE_ASSIGN:
            ss << "unmatching types on both sides of assignment operator (=)";
            break;
        case T6_RVAL_ON_ASSIGN_LEFT:
            ss << "rvalue on the left side of assignment operator";
            break;
        case T7_UNMATCH_OPERANDS:
            ss << "unmatching operands";
            break;
        case T8_FUNC_RETURN_UNMATCH_DECLARED:
            ss << "the function's return value type mismatches the declared type";
            break;
        case T9_FUNC_ARGS_UNMATCH_DECLARED:
            ss << "the function's arguments mismatch the declared parameters";
            break;
        case T10_INDEXING_ON_NON_ARRAY:
            ss << "applying indexing operator ([...]) on non-array type variables";
            break;
        case T11_INVOKE_NON_FUNC:
            ss << "applying function invocation operator (foo(...)) on non-function names";
            break;
        case T12_INDEX_NOT_INTEGER:
            ss << "array indexing with non-integer type expression";
            break;
        case T13_ACCESS_MEMBER_NON_STRUCTURE:
            ss << "accessing member of non-structure variable";
            break;
        case T14_ACCESS_NONDEF_STRUCTURE_MEMBER:
            ss << "accessing an undefined structure member";
            break;
        case T15_STRUCTURE_REDEF:
            ss << "redefine the same structure type";
            break;
        default:
            break;
    }

    ss << std::endl;
    message = ss.str();
    fprintf(out, message.c_str(), INFO_type, lineno);
}