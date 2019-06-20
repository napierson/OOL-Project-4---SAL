#pragma once
#include <vector>
#include <string>
#include <map>

class Instruction;

class VirtualMachine {
public:
	VirtualMachine();
	~VirtualMachine();

	bool step();
	void executeAll();
	void readInstructions(const std::string filename);
	std::string toString();

	std::vector<Instruction*> memory_; // Could have made these protected and written accessors
	int regA_;	// Or could have made them protected and declared all the concrete Instruction classes
	int regB_; // to be friends of VirtualMachine. Instead they are public and I will write the client code
	int programCounter_; // To not take advantage of this exposed functionality
	bool zeroBit_;
	bool overflowBit_;
	std::map<char, int> symbolDict_;

protected:
	void clearMemory();

	VirtualMachine(const VirtualMachine& vm); // No copying
	VirtualMachine& operator=(const VirtualMachine& vm); // No assignment
};

class Instruction {
public:
	virtual void execute()=0;
	virtual std::string toString()=0;

	virtual ~Instruction();

protected:
	Instruction(VirtualMachine* vm); // Instruction is an abstract class, no constructor

	VirtualMachine* vm_;
};

class DEC : public Instruction {
public:
	DEC(VirtualMachine* vm, char name);
	~DEC();

	void execute();
	std::string toString();
	int value();
	void value(int v);

protected:
	char name_;
	int value_;
	bool hasExecuted_;
};

class LDA : public Instruction {
public:
	LDA(VirtualMachine* vm, char name);
	~LDA();

	void execute();
	std::string toString();
protected:
	char name_;
};

class LDB : public Instruction {
public:
	LDB(VirtualMachine* vm, char name);
	~LDB();

	void execute();
	std::string toString();
protected:
	char name_;
};

class LDI : public Instruction {
public:
	LDI(VirtualMachine* vm, int value);
	~LDI();

	void execute();
	std::string toString();
protected:
	int value_;
};

class ST : public Instruction {
public:
	ST(VirtualMachine* vm, char name);
	~ST();

	void execute();
	std::string toString();
protected:
	char name_;
};

class XCH : public Instruction {
public:
	XCH(VirtualMachine* vm);
	~XCH();

	void execute();
	std::string toString();
};

class JMP : public Instruction {
public:
	JMP(VirtualMachine* vm, int destination);
	~JMP();

	void execute();
	std::string toString();
protected:
	int destination_;
};

class JZS : public Instruction {
public:
	JZS(VirtualMachine* vm, int destination);
	~JZS();

	void execute();
	std::string toString();
protected:
	int destination_;
};

class JVS : public Instruction {
public:
	JVS(VirtualMachine* vm, int destination);
	~JVS();

	void execute();
	std::string toString();
protected:
	int destination_;
};

class ADD : public Instruction {
public:
	ADD(VirtualMachine* vm);
	~ADD();

	void execute();
	std::string toString();
};

class HLT : public Instruction {
public:
	HLT(VirtualMachine* vm);
	~HLT();

	void execute();
	std::string toString();
};