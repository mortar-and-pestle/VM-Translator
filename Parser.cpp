#include "Parser.h"

Parser::Parser(std::ifstream& inputFile) : file(inputFile) {}

bool Parser::hasMoreCommands()
{
	if (file.peek() == EOF) { return false; }
	else { return true; }
}

void Parser::sanitizeLine(std::string& dirtyCommand)
{
	size_t positionOfComment{};

	positionOfComment = dirtyCommand.find('/');

	//Delete Comments
	if (!(positionOfComment == std::string::npos))
	{
		dirtyCommand = dirtyCommand.erase(positionOfComment);
	}

	//Delete Remainging White Space
	while (dirtyCommand[dirtyCommand.length() - 1] == ' ') { dirtyCommand.pop_back(); }
}

void Parser::advance()
{
	while (file.peek() == '/' || file.peek() == '\n') { getline(file, fullCommandLine); }
	getline(file, fullCommandLine);
	sanitizeLine(fullCommandLine);
}

std::string Parser::parseCommand()
{
	std::string command{ fullCommandLine };
	size_t positionOfSpace{ fullCommandLine.find(" ") };

	if (positionOfSpace == std::string::npos) { return command; }
	else
	{
		return command.substr(0, positionOfSpace);
	}
}

CommandType Parser::commandType()
{
	std::string command{ parseCommand() };

	if (command.find("pop") != std::string::npos)
	{
		return CommandType::cPop;
	}
	else if (command.find("push") != std::string::npos)
	{
		return CommandType::cPush;
	}
	else if (command.find("add") != std::string::npos || command.find("sub") != std::string::npos || command.find("neg") != std::string::npos)
	{
		return CommandType::cArithmetic;
	}
	else if (command.find("eq") != std::string::npos || command.find("gt") != std::string::npos || command.find("lt") != std::string::npos)
	{
		return CommandType::cArithmetic;
	}
	else if (command.find("and") != std::string::npos || command.find("or") != std::string::npos || command.find("not") != std::string::npos)
	{
		return CommandType::cArithmetic;
	}
	else if (command.find("label") != std::string::npos)
	{
		return CommandType::cLabel;
	}
	else if (command.find("goto") != std::string::npos)
	{
		if (command.find("if") != std::string::npos) { return CommandType::cIf; }
		return CommandType::cGoto;
	}
	else if (command.find("function") != std::string::npos)
	{
		return CommandType::cFunction;
	}
	else if (command.find("call") != std::string::npos)
	{
		return CommandType::cCall;
	}
	else if (command.find("return") != std::string::npos)
	{
		return CommandType::cReturn;
	}
	else 
	{ 
		std::cerr << "ERROR: Could not determine command type\n";
		throw Errors::errorUnknownTypeCommand;
	}
}

std::string Parser::checkCommandTypeArg1()
{
	if (commandType() != CommandType::cReturn)
	{
		return parseArg1();
	}
	else { return ""; }
}

std::string Parser::parseArg1()
{
	// If Arith
	if (commandType() == CommandType::cArithmetic)
	{
		return fullCommandLine;
	}
	//If push segment i or pop segment i
	else if (commandType() == CommandType::cPush || commandType() == CommandType::cPop || commandType() == CommandType::cCall || commandType() == CommandType::cFunction)
	{
		size_t posOfFirstSpace{ fullCommandLine.find(' ') };
		size_t posOfLastSpace{ fullCommandLine.rfind(' ') };

		//return substring between spaces in command
		return fullCommandLine.substr(posOfFirstSpace+1, posOfLastSpace - posOfFirstSpace - 1);
	}
	//If Function
	else if (commandType() == CommandType::cIf || commandType() == CommandType::cLabel || commandType() == CommandType::cGoto)
	{
		size_t posOfFirstSpace{ fullCommandLine.find(' ') };
		return fullCommandLine.substr(posOfFirstSpace + 1, std::string::npos);
	}
	else
	{
		std::cerr << "ERROR:  Could not parse first argument of command.\n";
		throw Errors::errorArg1Command;
	}
}

std::string Parser::getArg1()
{
	return checkCommandTypeArg1();
}

int Parser::checkCommandTypeArg2()
{
	if (commandType() == CommandType::cPush || commandType() == CommandType::cPop || commandType() == CommandType::cFunction || commandType() == CommandType::cCall)
	{
		return parseArg2();
	}
	//Something needs to be done about this
	else { return 999; }
}

int Parser::parseArg2()
{
	size_t positionOfLastSpace{ fullCommandLine.rfind(' ') };
	try
	{
		return stoi(fullCommandLine.substr(positionOfLastSpace + 1, std::string::npos));
	}
	catch (std::invalid_argument&)
	{
		std::cerr << "Error:  Cannot convert string to integer.\n";
		throw;
	}
}

int Parser::getArg2()
{
	return checkCommandTypeArg2();
}
