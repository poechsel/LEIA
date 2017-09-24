#include "simulateur.h"

std::string evaluate(const uword opcode, Machine &machine, Param &param, Screen* screen) {
    unsigned int speed_factor = 1;
    std::string out;
    //std::cout<<"azr\n";
	switch (opcode>>12) {
		case 0b0000: //wmem
			if (opcode & 0b0000100000000000) { //push, obsolete
				machine.memory[machine.registers[14]++] = machine.registers[toUWord(opcode)];
			} else {
				machine.memory[machine.registers[toUWord(opcode)]] = machine.registers[(opcode>>4)&0xF];
			}
			machine.pc++;
            speed_factor = machine.clock_ticks.wmem_t;
			break;
		case 0b0001: //add
			if (opcode & (1<<11)) {
				machine.registers[getDestination(opcode)] = machine.registers[(opcode>>4)&0xF] + toUWord(opcode);
			} else {
				machine.registers[getDestination(opcode)] = machine.registers[(opcode>>4)&0xF] + machine.registers[toUWord(opcode)];
			}
            speed_factor = machine.clock_ticks.add_t;
			machine.pc++;
			break;
		case 0b0010: //sub
			if (opcode & (1<<11)) {
				machine.registers[getDestination(opcode)] = machine.registers[(opcode>>4)&0xF] - toUWord(opcode);
			} else {
				machine.registers[getDestination(opcode)] = machine.registers[(opcode>>4)&0xF] - machine.registers[toUWord(opcode)];
			}
            speed_factor = machine.clock_ticks.sub_t;
			machine.pc++;
			break;
		case 0b0011:{ //snif
			word a = machine.registers[(opcode>>4) & 0xF];
			word b = (opcode & (1<<11))? toWord(opcode) : machine.registers[toUWord(opcode)];

			switch (getDestination(opcode)) {
				case 0b000:
					machine.pc += (a == b);
					break;
				case 0b001:
					machine.pc +=  (a != b);
					break;
				case 0b010:
					machine.pc += (a > b);
					break;
				case 0b011:
					machine.pc += (a < b);
					break;
				case 0b100:
					machine.pc += ((uword)a >  (uword)b);
					break;
				case 0b101:
					machine.pc += ((uword)a >= (uword)b);
					break;
				case 0b110:
					machine.pc += ((uword)a < (uword)b);
					break;
				case 0b111:
					machine.pc += ((uword)a <= (uword)b);
					break;
			}
            speed_factor = machine.clock_ticks.snif_t;
			machine.pc++;
			break;}
		case 0b0100:{ //and
			word a = machine.registers[(opcode>>4) & 0xF];
			word b = (opcode & (1<<11)) ? toWord(opcode) : machine.registers[toUWord(opcode)];
			machine.registers[getDestination(opcode)] = a & b;
            speed_factor = machine.clock_ticks.and_t;
			machine.pc++;
			break;}
		case 0b0101:{ //or
			word a = machine.registers[(opcode>>4) & 0xF];
			word b = (opcode & (1<<11)) ? toWord(opcode) : machine.registers[toUWord(opcode)];
			machine.registers[getDestination(opcode)] = a | b;
            speed_factor = machine.clock_ticks.or_t;
			machine.pc++;
			break;}
		case 0b0110:{ //xor
			word a = machine.registers[(opcode>>4) & 0xF];
			word b = (opcode & (1<<11)) ? toWord(opcode) : machine.registers[toUWord(opcode)];
			machine.registers[getDestination(opcode)] = a ^ b;
            speed_factor = machine.clock_ticks.xor_t;
			machine.pc++;
			break;}
		case 0b0111: { //lsl
			uword a = machine.registers[(opcode>>4) & 0xF];
			uword b = (opcode & (1<<11))? toUWord(opcode) : machine.registers[toUWord(opcode)];
			machine.registers[getDestination(opcode)] = a << b;
            speed_factor = machine.clock_ticks.lsl_t;
			machine.pc++;
			break;}
		case 0b1000: { //lsr
			uword a = machine.registers[(opcode>>4) & 0xF];
			uword b = (opcode & (1<<11))? toUWord(opcode) : machine.registers[toUWord(opcode)];
			machine.registers[getDestination(opcode)] = a >> b;
            speed_factor = machine.clock_ticks.lsr_t;
			machine.pc++;
			break;}
		case 0b1001:{ //asr
			word a = machine.registers[(opcode>>4) & 0xF];
			uword b = (opcode & (1<<11))? toUWord(opcode) : machine.registers[toUWord(opcode)];
			machine.registers[getDestination(opcode)] = a >> b;
            speed_factor = machine.clock_ticks.asr_t;
			machine.pc++;
			break;}
		case 0b1010: //call
			if (param.skip_call)
					machine.in_call++;
			machine.registers[15] = machine.pc+1;
			machine.pc = ((uword) (opcode & 0xFFF)) * 16;
            speed_factor = machine.clock_ticks.call_t;
			break;
		case 0b1011: //jump
			if (opcode == 0b1011000000000001) {
				machine.pc = machine.registers[15];
				if (param.skip_call)
					machine.in_call--;
                speed_factor = machine.clock_ticks.return_t;
            } else { 
				word t = (opcode & 0xFFF);
				if (t & 0b100000000000) 
					t |= 0xF000;
				machine.pc += t;
                speed_factor = machine.clock_ticks.jump_t;
			}
			break;
		case 0b1100:{ //letl
			uword d = getExtDestination(opcode);
			machine.registers[d] = (opcode & 0xFF);
			if ((machine.registers[d] >> 7) & 1)
				machine.registers[d] |= 0xFF00;
			machine.pc++;
            speed_factor = machine.clock_ticks.letl_t;
			break;}
		case 0b1101:{ //leth
			machine.registers[getExtDestination(opcode)] = (machine.registers[getExtDestination(opcode)] & 0x00FF) | ((opcode & 0xFF) << 8);
			machine.pc++;
            speed_factor = machine.clock_ticks.leth_t;
			break;}
		case 0b1110: // extensions : refresh and print
			switch ((opcode >> 10) & 0b11) {
			case 1: {
                        std::stringstream buffer;
                        buffer << (word)machine.registers[toUWord(opcode)] << "\n";
                        out = buffer.str();
				break;}
			case 2: {
                        std::stringstream buffer;
                        buffer << (char)(opcode & 0xff);
                        out = buffer.str();
				break;}
			default:
				/*refresh = true;
				while (refresh) {};
			    */
                if (screen)
                    screen->updateContent(machine.memory);
                };
			machine.pc++;
			break;
		case 0b1111: //rmem
			machine.pc++;
			if (opcode & 0b10000) {
				machine.registers[getExtDestination(opcode)] = machine.registers[toUWord(opcode)];
                speed_factor = machine.clock_ticks.copy_t;
            } else if (opcode & 0b10000000) { //pop, obsolete
				machine.registers[toUWord(opcode)] = machine.memory[--machine.registers[14]];
			} else {
				machine.registers[getExtDestination(opcode)] = machine.memory[(uword) machine.registers[toUWord(opcode)]];
                speed_factor = machine.clock_ticks.rmem_t;
			}
			break;
	}

    volatile unsigned int a;
    for (unsigned int j = 0; j < speed_factor; ++j) {
        for (unsigned int i = 0; i < machine.clock_ticks.click_constant; ++i) {
            a += 1;
        }
    }
    return out;
}


void simulate(Machine &machine, Param &param, Screen *screen) {
	machine.pc = 0;
	uword previous_pc = -1;
	while (machine.pc != previous_pc) {
		if (param.debug_output)
			debug(machine);
		if (param.step_by_step)
			getchar();
		previous_pc = machine.pc;
		do {
            std::string out = evaluate(machine.memory[machine.pc], machine, param, screen);
            if (out != "")
                std::cout<<out;
		} while (param.skip_call && machine.in_call);
	}
}

void fullDebug(Machine &machine, Param &param, Screen *screen) {
    std::ios::fmtflags cin_flags( std::cin.flags() );
	machine.pc = 0;
	uword previous_pc = -1;
	bool correct_command = true;
	while (!correct_command || machine.pc != previous_pc) {
		previous_pc = machine.pc;
		correct_command = true;
		for (uword i = machine.pc; i < machine.pc + 10; ++i) {
				std::cout<<i<<"("<<i<<")"<<":\t"<<dissassemble(machine.memory[i])<<"\n";
		}
		std::string action;
		std::cin>>action;
		DebugCommand command = parseCommand(action);
		if (command.command == "p") {
			debugRegister(machine, command.number);
			correct_command = false;
		} else {
			for (unsigned int i = 0; i < command.number; ++i) {
				if (command.command == "d") {
					debug(machine);
					correct_command = false;
				} else if (command.command == "n") {
					param.skip_call = true;
					while (param.skip_call) {
                        std::string out = evaluate(machine.memory[machine.pc], machine, param, screen);
                        if (out != "")
                            std::cout<<out;
						if (!machine.in_call)
							param.skip_call = false;
					}
				} else if (command.command == "s") {
                    std::string out = evaluate(machine.memory[machine.pc], machine, param, screen);
                    if (out != "")
                        std::cout<<out;
				} else if (command.command == "x") {
                    int address;
                    std::cin.unsetf(std::ios::dec);
                    std::cin.unsetf(std::ios::hex);
                    std::cin.unsetf(std::ios::oct);   
                    std::cin >> address;
                    std::cin.flags(cin_flags);
                    if (0 <= address && address <= 0xffff) {
                        for (int offset = std::max(0, address - 16); 
                                offset <= std::min(0xffff, address + 16); ++offset) {
                            if (offset == address) {
                                std::cout << "> ";
                            } else {
                                std::cout << "  ";
                            }
                            std::cout << "[" << offset << "]" << ": ";
                            std::cout << machine.memory[offset] << "(" << std::hex << machine.memory[offset] << std::dec << ")\n";
                        }
                    } else {
                        std::cout<<"given memory adress is incorrect\n";
                    }
                    correct_command = false;
                } else {
					correct_command = false;
					std::cout<<"Unknown command\n";
				}
			}
		}
		if (command.number != 1 && machine.pc == previous_pc)
			correct_command = false;
	}
}

