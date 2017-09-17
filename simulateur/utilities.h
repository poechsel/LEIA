#ifndef UTILITIES_H
#define UTILITIES_H 

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include "structs.h"

std::string stripNonHex(const std::string source);
uword convHex(char c);
std::vector<uword> toOpCodes(std::string source);
bool readFromBin(std::string file_path, Machine &machine);
bool readFromStr(std::string file_path, Machine &machine);
void loadCodeToMemory(Machine &machine);

#endif /* UTILITIES_H */
