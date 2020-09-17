#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include "errors.hpp"
#include "scanner.hpp"

using namespace holeyc;

static void usageAndDie(){
	std::cerr << "Usage: holeycc <infile> <options>\n"
	<< " [-u <unparseFile>]: Unparse to <unparseFile>\n"
	<< " [-p]: Parse the input to check syntax\n"
	<< " [-t <tokensFile>]: Output tokens to <tokensFile>\n"
	;
	exit(1);
}

static void writeTokenStream(const char * inPath, const char * outPath){
	std::ifstream inStream(inPath);
	if (!inStream.good()){
		std::string msg = "Bad input stream";
		msg += inPath;
		throw new InternalError(msg.c_str());
	}
	if (outPath == nullptr){
		std::string msg = "No tokens output file given";
		throw new InternalError(msg.c_str());
	}

	Scanner scanner(&inStream);
	if (strcmp(outPath, "--") == 0){
		scanner.outputTokens(std::cout);
	} else {
		std::ofstream outStream(outPath);
		if (!outStream.good()){
			std::string msg = "Bad output file ";
			msg += outPath;
			throw new InternalError(msg.c_str());
		}
		scanner.outputTokens(outStream);
		outStream.close();
	}
}

static bool parse(const char * inFile){
	std::ifstream inStream(inFile);
	if (!inStream.good()){
		std::string msg = "Bad input stream ";
		msg += inFile;
		throw new InternalError(msg.c_str());
	}

	holeyc::ProgramNode * root = nullptr;
	holeyc::Scanner scanner(&inStream);
	holeyc::Parser parser(scanner, &root);
	int errCode = parser.parse();
	if (errCode != 0){ return false; }

	return true;
}

static holeyc::ProgramNode * syntacticAnalysis(const char * inFile){
	std::ifstream inStream(inFile);
	if (!inStream.good()){
		std::string msg = "Bad input stream ";
		msg += inFile;
		throw new InternalError(msg.c_str());
	}

	holeyc::ProgramNode * root = nullptr;
	holeyc::Scanner scanner(&inStream);
	holeyc::Parser parser(scanner, &root);
	int errCode = parser.parse();
	if (errCode != 0){ return nullptr; }

	return root;
}

static void doUnparsing(holeyc::ProgramNode * ast, const char * outPath){
	if (outPath == nullptr){ 
		std::cerr << "No output path\n"; 
		return;
	}

	if (strcmp(outPath, "--") == 0){
		ast->unparse(std::cout, 0);
	} else {
		std::ofstream outStream(outPath);
		if (!outStream.good()){
			std::string msg = "Bad output file ";
			msg += outPath;
			throw new holeyc::InternalError(msg.c_str());
		}
		ast->unparse(outStream, 0);
	}
}

int 
main( const int argc, const char **argv )
{
	if (argc == 0){
		usageAndDie();
	}
	const char * inFile = NULL;
	const char * tokensFile = NULL;
	bool checkParse = false;
	const char * unparseFile = NULL;
	bool useful = false;
	int i = 1;
	for (int i = 1 ; i < argc ; i++){
		if (argv[i][0] == '-'){
			if (argv[i][1] == 't'){
				i++;
				tokensFile = argv[i];
				useful = true;
			} else if (argv[i][1] == 'p'){
				checkParse = true;
				useful = true;
			} else if (argv[i][1] == 'u'){
				i++;
				unparseFile = argv[i];
				useful = true;
			} else {
				std::cerr << "Unrecognized argument: ";
				std::cerr << argv[i] << std::endl;
				usageAndDie();
			}
		} else {
			if (inFile == NULL){
				inFile = argv[i];
			} else {
				std::cerr << "Only 1 input file allowed";
				std::cerr << argv[i] << std::endl;
				usageAndDie();
			}
		}
	}
	if (inFile == nullptr){
		usageAndDie();
	}
	if (!useful){
		std::cerr << "Whoops, you didn't tell holeycc what to do!\n";
		usageAndDie();
	}

	if (tokensFile != nullptr){
		try {
			writeTokenStream(inFile, tokensFile);
		} catch (InternalError * e){
			std::cerr << "Error: " << e->msg() << std::endl;
		}
	}

	if (checkParse){
		try {
			bool parsed = parse(inFile);
			if (!parsed){
				std::cerr << "Parse failed";
			}
		} catch (ToDoError * e){
			std::cerr << "ToDo: " << e->msg() << std::endl;
			exit(1);
		}
	}

	if (unparseFile != nullptr){
		try {
			ProgramNode * ast = syntacticAnalysis(inFile);
			if (ast){
				doUnparsing(ast, unparseFile);
			}
		} catch (InternalError * e){
			std::cerr << "Error: " << e->msg() << std::endl;
			exit(1);
		} catch (ToDoError * e){
			std::cerr << "ToDo: " << e->msg() << std::endl;
			exit(1);
		}
	}
	
	return 0;
}
