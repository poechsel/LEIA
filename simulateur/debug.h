#ifndef H_DEBUG
#define H_DEBUG

#include <string>
#include <iostream>
#include <cstdio>
#include "structs.h"

void debugRegister(const Machine &machine, const int reg);
void debug(const Machine &machine);

inline std::string getRegStr(const word s);
inline std::string getAdress(const word s);
inline std::string getNumber(const word s);
std::string dissassemble_remainder(const uword opcode);
std::string dissassemble(const uword opcode);
DebugCommand parseCommand(const std::string command);

#endif 

