#ifndef CODE_H
#define CODE_H
#include <iostream>
#include <string>
#include <fstream>
#include "TranslatorEnums.h"

class Code
{
	private:
		std::ofstream& outFile;
		std::string currentFileName;
		int uniqueLabelExtension{};
		std::string currentFunctionName;

		std::string generateUniqueLabel(std::string baseStr, CommandType type); //Forms unique(restricted by int size) label for logical eq, gt, and lt commands
		void writeArithmetic(std::string command); //Writes arithmetic and logical commands
		void writePushPop(const CommandType type, const std::string segment, const int index); //Writes push/pop commands for various memory segments
		void writeInit(); //Writes bootstrap code at beginning of the assembly file
		void writeLabel(const std::string label); //Writes label  in assembly as function$label
		void writeGoto(const std::string label); //Writes goto in assembly 
		void writeIf(const std::string label); //Writes if in assembly
		void writeCall(const std::string functName, const int numArgs); //Writes call in assembly. Requires placing frame on stack
		void writeReturn(); //Writes return in assembly
		void writeFunction(const std::string functName, const int numLocals); //Writes function in assembly.
	public:
		Code(std::ofstream& outF, const std::string bootStrapFlag); //Opens .asm write file
		~Code(); //Close output file
		void setFileName(const std::string fileName); //Changes file name when additional VM files are processed. This is required due to static variables
		void writeCommandToFile(const CommandType type, const std::string segment, const int index); //Translates VM command to hack. Writes to file
		void close(); //Closes write file
};
#endif