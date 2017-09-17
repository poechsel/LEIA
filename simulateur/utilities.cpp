#include "utilities.h"

/* make sure we only read characters belonging to a hexadecimal chain */
std::string stripNonHex(const std::string source) {
	std::string output;
	for (auto c : source) {
		if (('0' <= c && c <= '9')
			|| ('a' <= c && c <= 'f')
			|| ('A' <= c && c <= 'F'))
		output.push_back(c);
	}
	return output;
}

/* convert an hex char to an int */
uword convHex(char c) {
	if ('0' <= c && c <= '9')
		return c - '0';
	if ('a' <= c && c <= 'f')
		return 10 + c - 'a';
	if ('A' <= c && c <= 'F')
		return 10 + c - 'A';
	return 0;
}


/* takes a source and convert it to an array of opcodes */
std::vector<uword> toOpCodes(std::string source) {
	if (source.size() % 4 != 0) {
		printf ("Oups, code incorrect\n");
		return std::vector<uword>();
	}
	std::vector<uword> out;
	for (unsigned int i = 0; i < source.size(); i += 4) {
		uword temp = 0;
		temp |= (convHex(source[i]) << 12);
		temp |= (convHex(source[i+1]) << 8);
		temp |= (convHex(source[i+2]) << 4);
		temp |= (convHex(source[i+3]));
		out.push_back(temp);
	}
	return out;
}


/* read from binary file
 * deprecated but keep, we never know
 * */
bool readFromBin(std::string file_path, Machine &machine) {
	std::ifstream file(file_path, std::ios::binary | std::ios::ate);
	file.seekg(0, std::ios::end);
	auto file_size = file.tellg();
	if ((file_size & 1) == 1) {
		printf("[Erreur], le fichier n'est pas correct\n");
		return false;
	} else {
		file.seekg(0, std::ios::beg);
		unsigned short code[file_size>>1];
	    char current;
		int i = 0;
		while (file.get(current)) {
			code[i] = ((unsigned char) current) << 8;
			file.get(current);
			code[i] |=(unsigned char) current;
			machine.program.push_back(code[i]);
			i++;
		}
		return true;
	}
}

/* load the program from a path */
bool readFromStr(std::string file_path, Machine &machine) {
	std::ifstream file(file_path);
    if(file.fail()) {
        std::cout << "cannot read file: '" << file_path << "'\n";
        exit(1);
        //Note: returning false does not seem enough so we exit() ourselves
    }
	std::string code_str = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	machine.program = toOpCodes(stripNonHex(code_str));
	return true;
}

/* transfer the code to the memory */
void loadCodeToMemory(Machine &machine) {
	for (unsigned int i = 0; i < machine.program.size(); ++i) {
		machine.memory[/*PROGRAM_BEGIN -*/ i] = machine.program[i];
	}
}

