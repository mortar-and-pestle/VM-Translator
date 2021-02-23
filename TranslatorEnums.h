#ifndef TRANSLATORENUMS_H
#define TRANSLATORENUMS_H

// Type of command(Used in both Parser and Code)
enum class CommandType
{
	cArithmetic,
	cPush,
	cPop,
	cLabel,
	cGoto,
	cIf,
	cFunction,
	cReturn,
	cCall,
	hackAddress,
	hackLabel
};

// Various error codes(Used in both Parser and Code)
enum class Errors
{
	//errorOpeningReadFile,
	errorUnknownTypeCommand,
	errorArg1Command,
	errorArg2Command,
	errorOpeningWriteFile,
	errorClosingWriteFile,
	errorUnknownArithCommand,
	errorWritingArithCommand,
	errorUnknownCommandSegement,
	errorInvalidIndex
};

// Passed argument type(Used in VMtranslator only)
enum class ArgumentType
{
	vmFile,
	directory
};
#endif
