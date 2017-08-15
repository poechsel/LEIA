#ifndef H_SIMULATEUR
#define H_SIMULATEUR

#include "structs.h"
#include "debug.h"

/* evaluate a instruction */
void evaluate(const uword opcode, Machine &machine, Param &param);
/* simulate a program */
void simulate(Machine &machine, Param &param);
/* debugger */
void fullDebug(Machine &machine, Param &param);

#endif
