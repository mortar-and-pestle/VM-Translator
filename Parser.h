#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include <string>
#include <fstream>
#include "TranslatorEnums.h"

class Parser
{	
	private:
		std::ifstream& file;
		std::string fullCommandLine;

		void sanitizeLine(std::string& command); //Remove comments from command line
		std::string parseCommand(); // Returns command
		std::string checkCommandTypeArg1();	//Returns arg1 if command isn't return
		std::string parseArg1(); //Parses command for arg1
		int checkCommandTypeArg2(); //Returns arg2 if command doesn't return
		int parseArg2(); //Parses command for arg2

	public:
		Parser(std::ifstream& inputFile); //Opens VM file for processing
		bool hasMoreCommands(); //Checks if there are more VM commands to grab
		void advance(); //Grabs the next line of VM command to process
		CommandType commandType(); //Determines the type of the VM command
		std::string getArg1(); //Returns arg1 of command
		int getArg2(); //Returns arg2 of command
};
#endif
