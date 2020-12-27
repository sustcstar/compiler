#ifndef __IR__HPP
#define __IR__HPP

#include<iostream>
#include"parseTree.hpp"

// #include<string.h>
// #define LINE_LENGTH 100

// class TAC{
//     public:
//     int addr;

//     string parseAddr(int addr){ //地址转成string
//         char tacline[LINE_LENGTH];
//         if (addr > 0){
//             sprintf(tacline, "t%d", addr); 
//         }
//         else{
//             sprintf(tacline, "#%d", -addr);
//         }
//         return tacline;
//     }
// };

// class AssignTAC: public TAC{
//     public:
//     int raddr;
    
//     AssignTAC(int addr, int raddr){
//         this->addr = addr;
//         this->raddr = raddr;
//     }
//     string to_string(){
//         char tacline[LINE_LENGTH];
//         sprintf(tacline, "t%d := %s\n", addr, parseAddr(raddr).c_str());
//         return tacline;
//     }
// };

// void irExtDefList(parseTree *node);
// void irExtDef(parseTree *node);

std::string irExp(parseTree *node, int place);

#endif