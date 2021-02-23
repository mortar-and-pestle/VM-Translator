#include "Code.h"

Code::Code(std::ofstream& outF, const std::string bootstrapFlag) : outFile(outF) 
{ 
	if (bootstrapFlag != "-n") { std::cout << "\n***writing bootstrap code***\n\n";  writeInit(); }
	std::cout << "***writing hack assembly code***\n\n";
}

void Code::setFileName(const std::string fileName){	currentFileName = fileName; }

std::string Code::generateUniqueLabel(std::string baseStr, CommandType type)
{
	if (type == CommandType::hackAddress)
	{
		baseStr.insert(baseStr.length(), ".");
		baseStr.insert(baseStr.length(), std::to_string(uniqueLabelExtension));
		return baseStr;
	}
	else if (type == CommandType::cLabel)
	{
		baseStr.insert(0, "$"); //$label
		baseStr.insert(0, currentFunctionName); //f$label
		baseStr.insert(0, "("); //(f$label
		baseStr.insert(baseStr.length(), ")"); //(f$label)

		return baseStr;
	}
	else if (type == CommandType::cGoto)
	{
		baseStr.insert(0, "$"); //$label
		baseStr.insert(0, currentFunctionName); //f$label

		return baseStr;
	}
	else
	{
		baseStr.insert(baseStr.length() - 1, ".");
		baseStr.insert(baseStr.length() - 1, std::to_string(uniqueLabelExtension));
		return baseStr;
	}
}

void Code::writeArithmetic(const std::string command)
{
	try
	{
		if (command == "add")
		{
			// x+y where y is top of stack
			outFile << "//add\n";

			//Pop y into D
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";
			outFile << "\tD=M\n";

			//Pop x into A
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";

			//Store sum onto stack
			outFile << "\tM=D+M\n";

			//Restore stack pointer
			outFile << "\t@SP\n";
			outFile << "\tM=M+1\n";
		}
		else if (command == "sub")
		{
			//x-y where y is top of stack
			outFile << "//sub\n";

			//Pop y into D
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";
			outFile << "\tD=M\n";

			//Pop x into A
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";

			//Subtract D from M and place onto stack
			outFile << "\tM=M-D\n";

			//Restore stack pointer
			outFile << "\t@SP\n";
			outFile << "\tM=M+1\n";
		}
		else if (command == "neg")
		{
			//-y where y is top of stack
			outFile << "//neg\n";

			//Pop y into D and A
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";
			outFile << "\tAD=M\n";

			//Subtract A from D twice to get negative of D
			outFile << "\tD=D-A\n";
			outFile << "\tD=D-A\n";

			//Store -y onto stack
			outFile << "\t@SP\n";
			outFile << "\tA=M\n";
			outFile << "\tM=D\n";

			//Restore stack pointer
			outFile << "\t@SP\n";
			outFile << "\tM=M+1\n";
		}
		else if (command == "eq")
		{
			//x == y where y is top of stack
			outFile << "//eq\n";

			//Base values to generate unique labels when the same VM command is called multiple times
			std::string baseRESETSPAddressStr{ "\t@RESETSTACK" };
			std::string baseRESETSPLabelStr{ "(RESETSTACK)" };
			std::string baseEQUALAddressStr{ "\t@EQUAL" };
			std::string baseEQUALLabelStr{ "(EQUAL)" };

			//Pop y into D
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";
			outFile << "\tD=M\n";

			//Pop x into A
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";
			outFile << "\tA=M\n";

			//Check if x==y by checking if their difference is 0. Jump to EQUAL if so.
			outFile << "\tD=D-A\n";
			outFile << generateUniqueLabel(baseEQUALAddressStr, CommandType::hackAddress) << "\n"; //outFile << "@EQUAL\n";
			outFile << "\tD;JEQ\n";

			//Since x!=y, store false on stack and jump to END
			outFile << "\t@SP\n";
			outFile << "\tA=M\n";
			outFile << "\tM=0\n";
			outFile << generateUniqueLabel(baseRESETSPAddressStr, CommandType::hackAddress) << "\n"; //outFile << "@RESETSTACK\n";
			outFile << "\t0;JMP\n";

			//Since x==y, store true on stack
			outFile << generateUniqueLabel(baseEQUALLabelStr, CommandType::hackLabel) << "\n"; //outFile << "(EQUAL)\n";
			outFile << "\tD=-1\n"; //outFile << "\t@32767\n"; //outFile << "\tD=A\n";
			outFile << "\t@SP\n";
			outFile << "\tA=M\n";
			outFile << "\tM=D\n";

			//Restore stack pointer
			outFile << generateUniqueLabel(baseRESETSPLabelStr, CommandType::hackLabel) << "\n"; //outFile << "(RESETSTACK)\n";
			outFile << "\t@SP\n";
			outFile << "\tM=M+1\n";
		}
		else if (command == "gt")
		{
			//x > y where y is top of stack
			outFile << "//gt\n";

			//Base values to generate unique labels when the same VM command is called multiple times
			std::string baseRESETSPAddressStr{ "\t@RESETSTACK" };
			std::string baseRESETSPLabelStr{ "(RESETSTACK)" };
			std::string baseGTAddressStr{ "\t@GREATER" };
			std::string baseGTLabelStr{ "(GREATER)" };

			//Pop y into D
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";
			outFile << "\tD=M\n";

			//Pop x into A
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";
			outFile << "\tA=M\n";

			//Check if x>y by checking if x-y>0. Jump to GREATER if so.
			outFile << "\tD=A-D\n";
			outFile << generateUniqueLabel(baseGTAddressStr, CommandType::hackAddress) << "\n"; //outFile << "@GREATER\n";
			outFile << "\tD;JGT\n";

			//Since x<=y, store false on stack and jump to FINISH
			outFile << "\t@SP\n";
			outFile << "\tA=M\n";
			outFile << "\tM=0\n";
			outFile << generateUniqueLabel(baseRESETSPAddressStr, CommandType::hackAddress) << "\n"; //outFile << "@RESETSP\n";
			outFile << "\t0;JMP\n";

			//Since x>y, store true on stack
			outFile << generateUniqueLabel(baseGTLabelStr, CommandType::hackLabel) << "\n"; //outFile << "(GREATER)\n";
			outFile << "\tD=-1\n"; //outFile << "\tD=A\n"; outFile << "\t@32767\n";
			outFile << "\t@SP\n";
			outFile << "\tA=M\n";
			outFile << "\tM=D\n";

			//Restore stack pointer
			outFile << generateUniqueLabel(baseRESETSPLabelStr, CommandType::hackLabel) << "\n"; //outFile << "(RESETSP)\n";
			outFile << "\t@SP\n";
			outFile << "\tM=M+1\n";
		}
		else if (command == "lt")
		{
			//x < y where y is top of stack
			outFile << "//lt\n";

			//Base values to generate unique labels when the same VM command is called multiple times
			std::string baseRESETSPAddressStr{ "\t@RESETSTACK" };
			std::string baseRESETSPLabelStr{ "(RESETSTACK)" };
			std::string baseLTAddressStr{ "\t@LESS" };
			std::string baseLTLabelStr{ "(LESS)" };

			//Pop y into D
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";
			outFile << "\tD=M\n";

			//Pop x into A
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";
			outFile << "\tA=M\n";

			//Check if x<y by checking if y-x>0. Jump to LESS if so.
			outFile << "\tD=D-A\n";
			outFile << generateUniqueLabel(baseLTAddressStr, CommandType::hackAddress) << "\n"; //outFile << "@LESS\n";
			outFile << "\tD;JGT\n";

			//Since x>=y, store false on stack and jump to FINISH
			outFile << "\t@SP\n";
			outFile << "\tA=M\n";
			outFile << "\tM=0\n";
			outFile << generateUniqueLabel(baseRESETSPAddressStr, CommandType::hackAddress) << "\n"; //outFile << "@RESETSP\n";
			outFile << "\t0;JMP\n";

			//Since x==y, store true on stack
			outFile << generateUniqueLabel(baseLTLabelStr, CommandType::hackLabel) << "\n"; //outFile << "(LESS)\n";
			outFile << "\tD=-1\n"; //outFile << "\tD=A\n"; outFile << "\t@32767\n";
			outFile << "\t@SP\n";
			outFile << "\tA=M\n";
			outFile << "\tM=D\n";

			//Restore stack pointer
			outFile << generateUniqueLabel(baseRESETSPLabelStr, CommandType::hackLabel) << "\n"; //outFile << "(RESETSP)\n";
			outFile << "\t@SP\n";
			outFile << "\tM=M+1\n";
		}
		else if (command == "and")
		{
			//x && y where y is top of stack
			outFile << "//and\n";

			//Pop y into D
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";
			outFile << "\tD=M\n";

			//Store bit-wise AND of x and y onto stack
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";
			outFile << "\tM=D&M\n";

			//Restore stack pointer
			outFile << "\t@SP\n";
			outFile << "\tM=M+1\n";
		}
		else if (command == "or")
		{
			//x || y where y is top of stack
			outFile << "//or\n";

			//Pop y into D
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";
			outFile << "\tD=M\n";

			//Store bit-wise OR of x and y onto stack
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";
			outFile << "\tM=D|M\n";

			//Restore stack pointer
			outFile << "\t@SP\n";
			outFile << "\tM=M+1\n";
		}
		else if (command == "not")
		{
			//!y where y is top of stack
			outFile << "//not\n";

			//Store !y onto stack
			outFile << "\t@SP\n";
			outFile << "\tM=M-1\n";
			outFile << "\tA=M\n";
			outFile << "\tM=!M\n";

			//Restore stack pointer
			outFile << "\t@SP\n";
			outFile << "\tM=M+1\n";
		}
		else { throw Errors::errorUnknownArithCommand; }
	}
	catch (Errors&)
	{
		std::cerr << "ERROR:  Command: " << command << "is not recognized.\n";
		close();
		throw;
	}
	catch (std::ios_base::failure&)
	{
		std::cerr << "ERROR:  Could not write to file.\n";
		close();
		throw;
	}

	//Increment unique ID
	++uniqueLabelExtension;
}

void Code::writePushPop(const CommandType type, const std::string segment, const int index)
{
	try
	{
		if (type == CommandType::cPush)
		{
			if (segment == "constant")
			{
				//push constant i
				outFile << "//push constant " << index << "\n";

				//Load constant into D
				outFile << "\t@" << index << "\n";
				outFile << "\tD=A\n";

				//Push D onto stack and Restore SP
				outFile << "\t@SP\n";
				outFile << "\tM=M+1\n";
				outFile << "\tA=M-1\n";
				outFile << "\tM=D\n";
			}
			else if (segment == "local")
			{
				//push local i
				outFile << "//push local " << index << "\n";

				//Put LCL pointer into D
				outFile << "\t@LCL\n";
				outFile << "\tD=M\n";

				//Increment that pointer by index i and put into A to access
				outFile << "\t@" << index << "\n";
				outFile << "\tA=D+A\n";

				//Retrieve file from LCL segment and place into D
				outFile << "\tD=M\n";

				//Push D onto stack and restore SP
				outFile << "\t@SP\n";
				outFile << "\tM=M+1\n";
				outFile << "\tA=M-1\n";
				outFile << "\tM=D\n";
			}
			else if (segment == "argument")
			{
				//push argument i
				outFile << "//push argument " << index << "\n";

				//Put ARG pointer into D
				outFile << "\t@ARG\n";
				outFile << "\tD=M\n";

				//Increment that pointer by index i and put into A to access
				outFile << "\t@" << index << "\n";
				outFile << "\tA=D+A\n";

				//Retrieve file from ARG segment and place into D
				outFile << "\tD=M\n";

				//Push D onto stack and restore SP
				outFile << "\t@SP\n";
				outFile << "\tM=M+1\n";
				outFile << "\tA=M-1\n";
				outFile << "\tM=D\n";
			}
			else if (segment == "this")
			{
				//push this i
				outFile << "//push this " << index << "\n";

				//Put this pointer into D
				outFile << "\t@THIS\n";
				outFile << "\tD=M\n";

				//Increment that pointer by index i and put into A to access
				outFile << "\t@" << index << "\n";
				outFile << "\tA=D+A\n";

				//Retrieve file from this segment and placce into D
				outFile << "\tD=M\n";

				//Push D onto stack and restore SP
				outFile << "\t@SP\n";
				outFile << "\tM=M+1\n";
				outFile << "\tA=M-1\n";
				outFile << "\tM=D\n";
			}
			else if (segment == "that")
			{
				//push that i
				outFile << "//push that " << index << "\n";

				//Put LCL pointer into D
				outFile << "\t@THAT\n";
				outFile << "\tD=M\n";

				//Increment that pointer by index i and put into A to access
				outFile << "\t@" << index << "\n";
				outFile << "\tA=D+A\n";

				//Retrieve file from THAT segment and place into D
				outFile << "\tD=M\n";

				//Push D onto stack and restore SP
				outFile << "\t@SP\n";
				outFile << "\tM=M+1\n";
				outFile << "\tA=M-1\n";
				outFile << "\tM=D\n";
			}
			else if (segment == "temp")
			{
				if (index > 7 || index < 0) { throw Errors::errorInvalidIndex; }
				std::string tempAddress{ std::to_string(5 + index) }; //Temp memory begins at RAM[5]

				//push temp i
				outFile << "//push temp " << index << "\n";

				//Put temp i into D
				outFile << "\t@" << tempAddress << "\n";
				outFile << "\tD=M\n";

				//Push D onto stack and restore SP
				outFile << "\t@SP\n";
				outFile << "\tM=M+1\n";
				outFile << "\tA=M-1\n";
				outFile << "\tM=D\n";
			}
			else if (segment == "pointer")
			{
				if (index > 1 || index < 0) { throw Errors::errorInvalidIndex; }

				std::string pointerAddress{ std::to_string(3 + index) }; //Pointer memory begins at RAM[3]

				//push pointer i
				outFile << "//push pointer " << index << "\n";

				//Store RAM[pointer+i] into D
				outFile << "\t@" << pointerAddress << "\n";
				outFile << "\tD=M\n";

				//Push D onto stack and restore SP
				outFile << "\t@SP\n";
				outFile << "\tM=M+1\n";
				outFile << "\tA=M-1\n";
				outFile << "\tM=D\n";
			}
			else if (segment == "static")
			{
				//push static i
				outFile << "//push static " << index << "\n";

				//Store value in @Xxx.i into D. Actual location is determined by assembler
				outFile << "\t@" << currentFileName << "." << index << "\n";
				outFile << "\tD=M\n";

				//Push D onto stack and restore SP
				outFile << "\t@SP\n";
				outFile << "\tM=M+1\n";
				outFile << "\tA=M-1\n";
				outFile << "\tM=D\n";
			}
			else { throw Errors::errorUnknownCommandSegement; }
		}
		else if (type == CommandType::cPop)
		{
			
			if (segment == "local")
			{
				//push local i
				outFile << "//pop local " << index << "\n";

				//Calculate local+i and place into R13
				outFile << "\t@LCL\n";
				outFile << "\tD=M\n";
				outFile << "\t@" << index << "\n";
				outFile << "\tD=D+A\n";
				outFile << "\t@R13\n";
				outFile << "\tM=D\n";

				//Put stack value into D
				outFile << "\t@SP\n";
				outFile << "\tM=M-1\n";
				outFile << "\tA=M\n";
				outFile << "\tD=M\n";

				//Place D into RAM[local+i] which is in R13
				outFile << "\t@R13\n";
				outFile << "\tA=M\n";
				outFile << "\tM=D\n";
			}
			else if (segment == "argument")
			{
				//push argument i
				outFile << "//pop argument " << index << "\n";

				//Calculate argument+i and place into R13
				outFile << "\t@ARG\n";
				outFile << "\tD=M\n";
				outFile << "\t@" << index << "\n";
				outFile << "\tD=D+A\n";
				outFile << "\t@R13\n";
				outFile << "\tM=D\n";

				//Put stack value into D
				outFile << "\t@SP\n";
				outFile << "\tM=M-1\n";
				outFile << "\tA=M\n";
				outFile << "\tD=M\n";

				//Place D into RAM[argument+i] which is in R13
				outFile << "\t@R13\n";
				outFile << "\tA=M\n";
				outFile << "\tM=D\n";
			}
			else if (segment == "this")
			{
				//push local i
				outFile << "//pop this " << index << "\n";

				//Calculate this+i and place into R13
				outFile << "\t@THIS\n";
				outFile << "\tD=M\n";
				outFile << "\t@" << index << "\n";
				outFile << "\tD=D+A\n";
				outFile << "\t@R13\n";
				outFile << "\tM=D\n";

				//Put stack value into D
				outFile << "\t@SP\n";
				outFile << "\tM=M-1\n";
				outFile << "\tA=M\n";
				outFile << "\tD=M\n";

				//Place D into RAM[this+i] which is in R13
				outFile << "\t@R13\n";
				outFile << "\tA=M\n";
				outFile << "\tM=D\n";
			}
			else if (segment == "that")
			{
				//push that i
				outFile << "//pop that " << index << "\n";

				//Calculate that+i and place into R13
				outFile << "\t@THAT\n";
				outFile << "\tD=M\n";
				outFile << "\t@" << index << "\n";
				outFile << "\tD=D+A\n";
				outFile << "\t@R13\n";
				outFile << "\tM=D\n";

				//Put stack value into D
				outFile << "\t@SP\n";
				outFile << "\tM=M-1\n";
				outFile << "\tA=M\n";
				outFile << "\tD=M\n";

				//Place D into RAM[that+i] which is in R13
				outFile << "\t@R13\n";
				outFile << "\tA=M\n";
				outFile << "\tM=D\n";
			}
			else if (segment == "temp")
			{
				if (index > 7 || index < 0) { throw Errors::errorInvalidIndex; }
				std::string tempAddress{ std::to_string(5 + index) }; //Temp memory begins at RAM[5]

				//push temp i
				outFile << "//pop temp " << index << "\n";

				//Place value from stack into D
				outFile << "\t@SP\n";
				outFile << "\tM=M-1\n";
				outFile << "\tA=M\n";
				outFile << "\tD=M\n";

				//Put temp i into D
				outFile << "\t@" << tempAddress << "\n";
				outFile << "\tM=D\n";
			}
			else if (segment == "pointer")
			{
				if (index > 1 || index < 0) { throw Errors::errorInvalidIndex; }
				std::string pointerAddress{ std::to_string(3 + index) }; //Pointer memory begins at RAM[3]

				//pop pointer i
				outFile << "//pop pointer " << index << "\n";

				//Place value from stack into D
				outFile << "\t@SP\n";
				outFile << "\tM=M-1\n";
				outFile << "\tA=M\n";
				outFile << "\tD=M\n";

				//Put temp i into D
				outFile << "\t@" << pointerAddress << "\n";
				outFile << "\tM=D\n";
			}
			else if (segment == "static")
			{
				//pop static i
				outFile << "//pop static " << index << "\n";

				//Pop value into D
				outFile << "\t@SP\n";
				outFile << "\tM=M-1\n";
				outFile << "\tA=M\n";
				outFile << "\tD=M\n";

				//Store value in static section of RAM(Actual location is calculated by Assembler)
				outFile << "\t@" << currentFileName << "." << index << "\n";
				outFile << "\tM=D\n";
			}
			else { throw Errors::errorUnknownCommandSegement; }
		}
	}
	catch (Errors& error)
	{
		if (error == Errors::errorUnknownCommandSegement)
		{
			std::cerr << "Error:  Unknown segment: " << segment << " found.\n";
		}
		else if (error == Errors::errorInvalidIndex)
		{
			std::cerr << "Error:  Invalid index: " << index << " found.\n";
		}
		close();
		throw;
	}
	catch (std::ios_base::failure&)
	{
		std::cerr << "ERROR:  Could not write to file.\n";
		close();
		throw;
	}
}

void Code::writeInit()
{
	//Set SP to 256 as specified by standard mapping
	outFile << "//Bootstrap code\n";
	outFile << "\t@256\n";
	outFile << "\tD=A\n";
	outFile << "\t@SP\n";
	outFile << "\tM=D\n";

	//call Sys.init
	writeCall("Sys.init", 0);
}

void Code::writeLabel(const std::string label)
{
	//label b

	std::string baseLabel{ label };

	try
	{
		outFile << "//label " << label << "\n";
		outFile << generateUniqueLabel(baseLabel, CommandType::cLabel) << "\n";
	}
	catch (std::ios_base::failure&)
	{
		std::cerr << "ERROR:  Could not write to file.\n";
		close();
		throw;
	}
}

void Code::writeGoto(const std::string label)
{
	//goto label

	std::string baseLabel{ label };

	try
	{
		outFile << "//goto " << label << "\n";
		outFile << "\t@" << generateUniqueLabel(baseLabel, CommandType::cGoto) << "\n";
		outFile << "\t0;JMP\n";
	}
	catch (std::ios_base::failure&)
	{
		std::cerr << "ERROR:  Could not write to file.\n";
		close();
		throw;
	}
}

void Code::writeIf(const std::string label)
{
	//if-goto b

	std::string baseLabel{ label };

	try
	{
		outFile << "//if-goto " << label << "\n";

		//Pop y into D
		outFile << "\t@SP\n";
		outFile << "\tM=M-1\n";
		outFile << "\tA=M\n";
		outFile << "\tD=M\n";

		//if D>0, jump to label
		outFile << "\t@" << generateUniqueLabel(baseLabel, CommandType::cGoto) << "\n";
		outFile << "\tD;JNE\n";
	}
	catch (std::ios_base::failure&)
	{
		std::cerr << "ERROR:  Could not write to file.\n";
		close();
		throw;
	}
}

void Code::writeCall(const std::string functName, const int numArgs)
{
	//Base values to generate unique labels when the same VM command is called multiple times
	std::string baseRETURNAddressStr{ "\t@RETURN" };
	std::string baseRETURNLabelStr{ "(RETURN)" };


	try
	{
		outFile << "//call " << functName << " " << numArgs << "\n";

		//push return address and restore SP
		outFile <<  generateUniqueLabel(baseRETURNAddressStr, CommandType::hackAddress) << "\n";
		outFile << "\tD=A\n";
		outFile << "\t@SP\n";
		outFile << "\tM=M+1\n";
		outFile << "\tA=M-1\n";
		outFile << "\tM=D\n";

		//push LCL address and restore SP
		outFile << "\t@LCL\n";
		outFile << "\tD=M\n";
		outFile << "\t@SP\n";
		outFile << "\tM=M+1\n";
		outFile << "\tA=M-1\n";
		outFile << "\tM=D\n";

		//push ARG address and restore SP
		outFile << "\t@ARG\n";
		outFile << "\tD=M\n";
		outFile << "\t@SP\n";
		outFile << "\tM=M+1\n";
		outFile << "\tA=M-1\n";
		outFile << "\tM=D\n";

		//push THIS address and restore SP
		outFile << "\t@THIS\n";
		outFile << "\tD=M\n";
		outFile << "\t@SP\n";
		outFile << "\tM=M+1\n";
		outFile << "\tA=M-1\n";
		outFile << "\tM=D\n";

		//push THAT address and restore SP
		outFile << "\t@THAT\n";
		outFile << "\tD=M\n"; //D=A
		outFile << "\t@SP\n";
		outFile << "\tM=M+1\n";
		outFile << "\tA=M-1\n";
		outFile << "\tM=D\n";

		//ARG = SP - n - 5
		outFile << "\t@SP\n";
		outFile << "\tD=M\n";
		outFile << "\t@" << std::to_string(numArgs) << "\n";
		outFile << "\tD=D-A\n";
		outFile << "\t@5\n";
		outFile << "\tD=D-A\n";
		outFile << "\t@ARG\n";
		outFile << "\tM=D\n";

		//LCL = SP
		outFile << "\t@SP\n";
		outFile << "\tD=M\n";
		outFile << "\t@LCL\n";
		outFile << "\tM=D\n";

		//goto f
		outFile << "\t@" << functName << "\n";
		outFile << "\t0;JMP\n";

		//(return address)
		outFile << generateUniqueLabel(baseRETURNLabelStr, CommandType::hackLabel) << "\n";
	}
	catch (std::ios_base::failure&)
	{
		std::cerr << "ERROR:  Could not write to file.\n";
		close();
		throw;
	}
	++uniqueLabelExtension;
}

void Code::writeReturn()
{
	try
	{
		//return
		outFile << "//return\n";

		//FRAME(R13) = LCL
		outFile << "\t@LCL\n";
		outFile << "\tD=M\n";
		outFile << "\t@R13\n";
		outFile << "\tM=D\n";

		//RET(R14) = *(FRAME-5)
		outFile << "\t@5\n";
		outFile << "\tA=D-A\n";
		outFile << "\tD=M\n";
		outFile << "\t@R14\n";
		outFile << "\tM=D\n";

		//*ARG = pop()
		//Put stack value into D
		outFile << "\t@SP\n";
		outFile << "\tM=M-1\n";
		outFile << "\tA=M\n";
		outFile << "\tD=M\n";
		//Put D into *ARG(argumnet 0)
		outFile << "\t@ARG\n";
		outFile << "\tA=M\n";
		outFile << "\tM=D\n";

		//SP=ARG+1
		outFile << "\tD=A+1\n";
		outFile << "\t@SP\n";
		outFile << "\tM=D\n";

		//THAT = *(FRAME-1)
		outFile << "\t@R13\n";
		outFile << "\tM=M-1\n";
		outFile << "\tA=M\n";
		outFile << "\tD=M\n";
		outFile << "\t@THAT\n";
		outFile << "\tM=D\n";

		//THIS = *(FRAME-2)
		outFile << "\t@R13\n";
		outFile << "\tM=M-1\n";
		outFile << "\tA=M\n";
		outFile << "\tD=M\n";
		outFile << "\t@THIS\n";
		outFile << "\tM=D\n";
		
		//ARG = *(FRAME-3)
		outFile << "\t@R13\n";
		outFile << "\tM=M-1\n";
		outFile << "\tA=M\n";
		outFile << "\tD=M\n";
		outFile << "\t@ARG\n";
		outFile << "\tM=D\n";

		//LCL = *(FRAME-4)
		outFile << "\t@R13\n";
		outFile << "\tM=M-1\n";
		outFile << "\tA=M\n";
		outFile << "\tD=M\n";
		outFile << "\t@LCL\n";
		outFile << "\tM=D\n";

		//goto return address
		outFile << "\t@R14\n";
		outFile << "\tA=M\n";
		outFile << "\t0;JMP\n";
	}
	catch (std::ios_base::failure&)
	{
		std::cerr << "ERROR:  Could not write to file.\n";
		close();
		throw;
	}
}

void Code::writeFunction(const std::string functName, const int numLocals)
{
	currentFunctionName = functName;

	try
	{
		outFile << "//function " << functName << " " << numLocals << "\n";
		outFile << "(" << functName << ")\n";

		for (int i = 0; i < numLocals; ++i)
		{
			writePushPop(CommandType::cPush, "constant", 0);
		}
	}
	catch (std::ios_base::failure&)
	{
		std::cerr << "ERROR:  Could not write to file.\n";
		close();
		throw;
	}
}

void Code::writeCommandToFile(const CommandType type, const std::string segment, const int index)
{
	if (type == CommandType::cArithmetic) { writeArithmetic(segment); }
	else if (type == CommandType::cPush || type == CommandType::cPop) { writePushPop(type, segment, index); }
	else if (type == CommandType::cLabel) { writeLabel(segment); }
	else if (type == CommandType::cGoto) { writeGoto(segment); }
	else if (type == CommandType::cIf) { writeIf(segment); }
	else if (type == CommandType::cCall) { writeCall(segment, index); }
	else if (type == CommandType::cFunction) { writeFunction(segment, index); }
	else if (type == CommandType::cReturn) { writeReturn(); }
	else { std::cerr << "ERROR:  Command type is unhandled\n"; }
}

void Code::close()
{
	try
	{
		outFile.close();
		std::cout << "***finished translating to hack assembly***\n";
	}
	catch (std::ios_base::failure&)
	{
		std::cerr << "ERROR:  Could not close file.\n";
		throw Errors::errorClosingWriteFile;
	}
}

Code::~Code()
{
	close();
}