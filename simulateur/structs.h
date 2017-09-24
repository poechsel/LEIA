#ifndef H_STRUCTS
#define H_STRUCTS

#include <string>
#include <vector>
#include <cstdint>

//constants about the environnement
const int WIDTH = 160;
const int HEIGHT = 128;
const int MEM_SCREEN_BEGIN = 0xb000;

//small definitions of type
typedef uint16_t uword;
typedef int16_t word;

//a variable to control the screen
extern volatile bool refresh;

//contain the number of clocks ticks corresponding to an instruction
struct ClockTicks {
    //constant to simulate a tick (in the form of a C iteration bound)
    unsigned int click_constant;

    unsigned int wmem_t;
    unsigned int add_t;
    unsigned int sub_t;
    unsigned int snif_t;
    unsigned int and_t;
    unsigned int or_t;
    unsigned int xor_t;
    unsigned int lsl_t;
    unsigned int lsr_t;
    unsigned int asr_t;
    unsigned int call_t;
    unsigned int jump_t;
    unsigned int return_t;
    unsigned int letl_t;
    unsigned int leth_t;
    unsigned int copy_t;
    unsigned int rmem_t;
};

//what represents our virtual machine
struct Machine {
	uword pc;
	uword registers[16];
	uword memory[0x10000];
	std::vector<uword> program;
    struct ClockTicks clock_ticks;
	int in_call;
};


//initialize a new clockticks struct
struct ClockTicks clockticks_new();

//the parameters associated with a simulation
struct Param {
	bool step_by_step;
	bool debug_output;
	bool fast_mode;
	bool full_debug;
	bool skip_call;
};

//a debug command
struct DebugCommand {
	unsigned int number;
	std::string command;
};

 
inline word toWord(const uword word) {
	return (word & 0b1000) ? (word & 0xF) | 0xFFF0 : (word & 0xF);
}
inline uword toUWord(const uword word) {
	return 0x0000 | (word & 0xF);
}
inline uword getDestination(const uword word) {
	return (word >> 8) & 0b111;
}
inline uword getExtDestination(const uword word) {
	return (word >> 8) & 0b1111;
}





#endif 
