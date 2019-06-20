#include "VM.h"
#include <sstream>
#include <fstream>

VirtualMachine::VirtualMachine() : memory_(256), regA_(0), regB_(0), programCounter_(0), zeroBit_(false), overflowBit_(false), symbolDict_() {}

VirtualMachine::~VirtualMachine()
{
	//  Delete the contents of memory_ and symbolDict_
	clearMemory();
}


// Execute the current instruction of the VM
// Return true if a non-halt instruction was executed
// Return false if a halt was executed, or the program counter is pointing to a null space in memory
bool VirtualMachine::step()
{
	if ((memory_.at(programCounter_) == nullptr) || ((dynamic_cast<HLT*>(memory_.at(programCounter_)) != nullptr)))
	{ // Either there is no instruction to execute, or the instruction to execute is a halt
		return false;
	}
	else
	{
		memory_.at(programCounter_)->execute();
		return true;
	}
}

// Execute all remaining instructions loaded into the VM
// Starting from the current one, and proceeding until a halt or empty memory slot is reached
void VirtualMachine::executeAll()
{
	while (step()) {}
}


// From the specified filename, read instructions into memory
void VirtualMachine::readInstructions(const std::string filename)
{
	// Clear all values stored in the program
	clearMemory();
	regA_ = 0;
	regB_ = 0;
	zeroBit_ = false;
	overflowBit_ = false;
	programCounter_ = 0;

	std::ifstream instructions(filename);
	std::string inst;
	std::string instType;
	char name;
	int value;
	std::stringstream instStream;
	int index = 0;

	while (getline(instructions, inst) && index < memory_.capacity())
	{
		instStream = std::stringstream(inst);
		instStream >> instType;
		if (instType == "DEC")
		{
			instStream >> name;
			memory_.at(index) = new DEC(this, name);
		}
		else if (instType == "LDA")
		{
			instStream >> name;
			memory_.at(index) = new LDA(this, name);
		}
		else if (instType == "LDB")
		{
			instStream >> name;
			memory_.at(index) = new LDB(this, name);
		}
		else if (instType == "LDI")
		{
			instStream >> value;
			memory_.at(index) = new LDI(this, value);
		}
		else if (instType == "ST")
		{
			instStream >> name;
			memory_.at(index) = new ST(this, name);
		}
		else if (instType == "XCH")
		{
			memory_.at(index) = new XCH(this);
		}
		else if (instType == "JMP")
		{
			instStream >> value;
			memory_.at(index) = new JMP(this, value);
		}
		else if (instType == "JZS")
		{
			instStream >> value;
			memory_.at(index) = new JZS(this, value);
		}
		else if (instType == "JVS")
		{
			instStream >> value;
			memory_.at(index) = new JVS(this, value);
		}
		else if (instType == "ADD")
		{
			memory_.at(index) = new ADD(this);
		}
		else if (instType == "HLT")
		{
			memory_.at(index) = new HLT(this);
		}

		index++;
	}

	instructions.close();
}

// Return a string containing a human-readable summary of the VM's current state
std::string VirtualMachine::toString()
{
	std::stringstream output;
	output << "Register A: " << regA_ << "\n";
	output << "Register B: " << regB_ << "\n";
	output << "Program counter: " << programCounter_ << "\n";
	output << "Zero bit: " << int(zeroBit_) << "\n";
	output << "Overflow bit: " << int(overflowBit_) << "\n";
	output << "Current memory:\n";
	for (int i = 0; i < memory_.size(); i++)
	{
		output << i << ": ";
		if (memory_.at(i) == nullptr)
		{
			output << "null\n";
		}
		else
		{
			output << memory_.at(i)->toString() << "\n";
		}
	}

	return output.str();	
}


// Clear the memory and symbol dictionary
void VirtualMachine::clearMemory()
{
	for (int i = 0; i < memory_.size(); i++)
	{
		delete memory_.at(i);
		memory_.at(i) = nullptr;
	}

	symbolDict_.clear();
}




Instruction::Instruction(VirtualMachine * vm) : vm_(vm) {}

Instruction::~Instruction() {}


DEC::DEC(VirtualMachine * vm, char name) : Instruction(vm), name_(name), value_(0), hasExecuted_(false) {}

DEC::~DEC() {}

void DEC::execute()
{
	if (!hasExecuted_)
	{
		vm_->symbolDict_[name_] = vm_->programCounter_; // Add the new identifer to the VM's symbol dictionary
		hasExecuted_ = true; // Switch modes
	}
	vm_->programCounter_++;
}

int DEC::value() {	return value_;}

void DEC::value(int v) { value_ = v; }

std::string DEC::toString()
{
	std::stringstream output;
	if (hasExecuted_)
	{
		output << name_ << ": " << value_;
	}
	else
	{
		output << "DEC " << name_;
	}
	return output.str();
}

LDA::LDA(VirtualMachine * vm, char name) : Instruction(vm), name_(name) {}

LDA::~LDA() {}

void LDA::execute()
{
	if (vm_->symbolDict_.count(name_) != 0) // If the identifier has been declared
	{
		int address = vm_->symbolDict_[name_]; // Find its location in memory
		int toLoad = dynamic_cast<DEC*>(vm_->memory_.at(address))->value(); // Find the value stored at that address
		vm_->regA_ = toLoad; // Load it into register A
	}

	vm_->programCounter_++;
}

std::string LDA::toString()
{
	std::stringstream output;
	output << "LDA " << name_;
	return output.str();
}

LDB::LDB(VirtualMachine * vm, char name) : Instruction(vm), name_(name) {}

LDB::~LDB() {}

void LDB::execute()
{
	if (vm_->symbolDict_.count(name_) != 0) // If the identifier has been declared
	{
		int address = vm_->symbolDict_[name_]; // Find its location in memory
		int toLoad = dynamic_cast<DEC*>(vm_->memory_.at(address))->value(); // Find the value stored at that address
		vm_->regB_ = toLoad; // Load it into register B
	}

	vm_->programCounter_++;
}

std::string LDB::toString()
{
	std::stringstream output;
	output << "LDB " << name_;
	return output.str();
}

LDI::LDI(VirtualMachine * vm, int value) : Instruction(vm), value_(value) {}

LDI::~LDI() {}

void LDI::execute()
{
	vm_->regA_ = value_;
	vm_->programCounter_++;
}

std::string LDI::toString()
{
	std::stringstream output;
	output << "LDI " << value_;
	return output.str();
}

ST::ST(VirtualMachine * vm, char name) : Instruction(vm), name_(name) {}

ST::~ST() {}

void ST::execute()
{
	if (vm_->symbolDict_.count(name_) != 0)
	{
		int address = vm_->symbolDict_[name_]; // Find the address of the identifier in memory
		int toStore = vm_->regA_; // Find the value currently in the accumulator
		dynamic_cast<DEC*>(vm_->memory_.at(address))->value(toStore); // Store that value in the appropriate DEC.
	}

	vm_->programCounter_++;
}

std::string ST::toString()
{
	std::stringstream output;
	output << "ST " << name_;
	return output.str();
}

XCH::XCH(VirtualMachine * vm) : Instruction(vm) {}

XCH::~XCH() {}

void XCH::execute()
{	// Swap the value stored in registers A and B
	int temp = vm_->regA_;
	vm_->regA_ = vm_->regB_;
	vm_->regB_ = temp;
	vm_->programCounter_++;
}

std::string XCH::toString()
{
	std::stringstream output;
	output << "XCH";
	return output.str();
}

JMP::JMP(VirtualMachine * vm, int destination) : Instruction(vm), destination_(destination) {}

JMP::~JMP() {}

void JMP::execute()
{
	// No boundary checking to see if the destination is valid, we assume the user supplies good SAL code
	vm_->programCounter_ = destination_;
}

std::string JMP::toString()
{
	std::stringstream output;
	output << "JMP " << destination_;
	return output.str();
}

JZS::JZS(VirtualMachine * vm, int destination) : Instruction(vm), destination_(destination) {}

JZS::~JZS() {}

void JZS::execute()
{
	if (vm_->zeroBit_)
	{
		vm_->programCounter_ = destination_;
	}
	else
	{
		vm_->programCounter_++;
	}
}

std::string JZS::toString()
{
	std::stringstream output;
	output << "JZS " << destination_;
	return output.str();
}

JVS::JVS(VirtualMachine * vm, int destination) : Instruction(vm), destination_(destination) {}

JVS::~JVS() {}

void JVS::execute()
{
	if (vm_->overflowBit_)
	{
		vm_->programCounter_ = destination_;
	}
	else
	{
		vm_->programCounter_++;
	}
}

std::string JVS::toString()
{
	std::stringstream output;
	output << "JVS " << destination_;
	return output.str();
}

ADD::ADD(VirtualMachine * vm) : Instruction(vm) {}

ADD::~ADD() {}

void ADD::execute()
{
	bool zero = false;
	bool overflow = false;
	
	int A = vm_->regA_;
	int B = vm_->regB_;
	int total = A + B;
	if (total > 536870911) // Apologies for the magic numbers, to emulate acting on 32-bit 2's-complement integers instead of ints
	{
		total -= 1073741824;
		overflow = true;
	}
	else if (total < -536870912)
	{
		total += 1073741824;
		overflow = true;
	}

	if (total == 0)
	{
		zero = true;
	}

	vm_->zeroBit_ = zero;
	vm_->overflowBit_ = overflow;
	vm_->regA_ = total;
	vm_->programCounter_++;
}

std::string ADD::toString()
{
	std::stringstream output;
	output << "ADD";
	return output.str();
}

HLT::HLT(VirtualMachine * vm) : Instruction(vm) {}

HLT::~HLT() {}

void HLT::execute() {}

std::string HLT::toString()
{
	std::stringstream output;
	output << "HLT";
	return output.str();
}