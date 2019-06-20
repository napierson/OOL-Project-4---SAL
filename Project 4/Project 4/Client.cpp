#include "VM.h"
#include <iostream>
#include <string>
#include <fstream>
int main() {
	VirtualMachine vm;
	std::string instFile("input.sal");
	std::string logFile("output.txt");
	std::ofstream logStream;
	char inst;
	bool toContinue = true;

	
	while (toContinue)
	{
		std::cout << "What is your next command?\n";
		std::cout << "i - read input, d - execute in debug mode, r - run to completion\n";
		std::cout << "s - save program state, q - quit\n";
		std::cin >> inst;

		switch (inst) {
		case 'i':
			vm.readInstructions(instFile);
			std::cout << "\n" << vm.toString() << "\n";
			break;
		case 'd':
			vm.step();
			std::cout << "\n" << vm.toString() << "\n";
			break;
		case 'r':
			vm.executeAll();
			std::cout << "\n" << vm.toString() << "\n";
			break;
		case 's':
			std::remove(logFile.c_str());
			logStream.open(logFile);
			logStream << vm.toString();
			logStream.close();
			break;
		case 'q':
			toContinue = false;
			break;
		}
	}
}