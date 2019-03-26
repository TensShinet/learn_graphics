#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"


int main() {

	std::string stringvalues = "f 24/1/24 25/2/25 26/3/26";
	std::istringstream iss(stringvalues.c_str());

	char trash;
	Vec3f tmp;

	iss >> trash >> trash;
	std::cout<< trash << '\n';

	return 0;
}