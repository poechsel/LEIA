#include "debug.h"


void debugRegister(const Machine& machine, const int reg) {
	printf("r%d => \t(uword) %d \t(word) %d \t(hex)%x\n", reg, machine.registers[reg], (word)machine.registers[reg], machine.registers[reg]);
};
		
void debug(const Machine &machine) {
	if (machine.in_call)
		return;
	printf("Next operation = %x : \n", machine.memory[machine.pc]);
	for (int i = 0; i < 16; i++) {
		debugRegister(machine, i);
	}
	printf("\tPC = %d\n", machine.pc);
}

inline std::string getRegStr(const word s) {
	return " r" + std::to_string(s);
}
inline std::string getAdress(const word s) {
	return " [r" + std::to_string(s) + "]";
}
inline std::string getNumber(const word s) {
	return " " + std::to_string(s);
}

std::string dissassemble_remainder(const uword opcode) {
	if (opcode & (1<<11)) {
		return getRegStr(getDestination(opcode)) + getRegStr((opcode>>4)&0xF) + getNumber(toUWord(opcode));
	} else {
		return getRegStr(getDestination(opcode)) + getRegStr((opcode>>4)&0xF) + getRegStr(toUWord(opcode));
	}
}
std::string dissassemble(const uword opcode) {
	switch (opcode>>12) {
		case 0b0000: //wmem
			if (opcode & 0b0000100000000000)
				return "push" + getRegStr(toUWord(opcode));
			else
				return "wmem" + getRegStr((opcode>>4)&0xF) + getAdress(toUWord(opcode));
			break;
		case 0b0001: //add
			return "add " + dissassemble_remainder(opcode);
			break;
		case 0b0010: //sub
			return "sub " + dissassemble_remainder(opcode);
			break;
		case 0b0011:{ //snif
			std::string comparisons[8] = {" eq", " neq", " sgt", " slt", " gt", " ge", " lt", " le"};
		
			if (opcode & (1<<11)) {
				return "snif " + getRegStr((opcode>>4)&0xF) + comparisons[getDestination(opcode)] + getNumber(toUWord(opcode));
			} else {
				return "snif " + getRegStr((opcode>>4)&0xF) + comparisons[getDestination(opcode)] + getRegStr(toUWord(opcode));
			}
			break;}
		case 0b0100:{ //and
			return "and " + dissassemble_remainder(opcode);
			break;}
		case 0b0101:{ //or
			return "or " + dissassemble_remainder(opcode);
			break;}
		case 0b0110:{ //xor
			return "xor " + dissassemble_remainder(opcode);
			break;}
		case 0b0111: { //lsl
			return "lsl " + dissassemble_remainder(opcode);
			break;}
		case 0b1000: { //lsr
			return "lsr " + dissassemble_remainder(opcode);
			break;}
		case 0b1001:{ //asr
			return "asr " + dissassemble_remainder(opcode);
			break;}
		case 0b1010: //call 
			return "call " + getNumber((opcode & 0xFFF) * 16);
			break;
		case 0b1011: //jump
			if (opcode == 0b1011000000000001) {
				return "return";
			} else { 
				word t = (opcode & 0xFFF);
				if (t & 0b100000000000) 
					t |= 0xF000;
				return "jump " + getNumber(t);
			}
			break;
		case 0b1100:{ //letl
			return "letl " + getRegStr(getExtDestination(opcode)) + getNumber(opcode & 0xFF);
			break;}
		case 0b1101:{ //leth
			return "leth " + getRegStr(getExtDestination(opcode)) + getNumber(opcode & 0xFF);
			break;}
		case 0b1110:
			switch((opcode >> 10) & 0b11) {
				case 0b00:
					return "refresh";
					break;
			}
			break;
		case 0b1111: //rmem
			if (opcode == 0b1111000000010000) {
				return "";
			} else {
				if (opcode & 0b10000)
					return "copy " + getRegStr(getExtDestination(opcode)) + getRegStr(toUWord(opcode));
				else if (opcode & 0b10000000)
					return "pop " + getRegStr(toUWord(opcode));
				else
					return "rmem " + getRegStr(getExtDestination(opcode)) + getAdress(toUWord(opcode));
			}
			break;
	}
	return "";
}

DebugCommand parseCommand(const std::string command) {
	DebugCommand temp;
	temp.number = 0;
	unsigned int i = 0;
	for (; i < command.size(); ++i) {
		if ('0' <= command[i] && command[i] <= '9') {
			temp.number *= 10;
			temp.number += command[i] - '0';
		} else {
			break;
		}
	}
	if (i == 0)
		temp.number = 1;
	temp.command = command.substr(i);
	return temp;
}
