#include <fstream>
#include <string.h>

#include "errors.hpp"
#include "scanner.hpp"
#include "ast.hpp"
#include "name_analysis.hpp"
#include "type_analysis.hpp"

using namespace holeyc;

static void usageAndDie(){
	std::cerr << "Usage: holeycc <infile> <options>\n"
	<< " [-t <tokensFile>]: Output tokens to <tokensFile>\n"
	<< " [-p]: Parse the input to check syntax\n"
	<< " [-u <unparseFile>]: Unparse to <unparseFile>\n"
	<< " [-n <nameFile]: Output name analysis to <namesFile>\n"
	<< " [-c]: Do type checking\n"
	<< " [-a <3ACFile>]: Output 3AC program to <3ACFile>\n"
	<< "\n"
	;
	std::cout << std::flush;
	std::cerr << std::flush;
	exit(1);
}

static void doTokenization(std::ifstream * input, const char * outPath){
	holeyc::Scanner scanner(input);
	if (strcmp(outPath, "--") == 0){
		scanner.outputTokens(std::cout);
	} else {
		std::ofstream outStream(outPath);
		if (!outStream.good()){
			std::string msg = "Bad output file ";
			msg += outPath;
			throw new holeyc::InternalError(msg.c_str());
		}
		scanner.outputTokens(outStream);
	}
}

static holeyc::ProgramNode * syntacticAnalysis(std::ifstream * input){
	if (input == nullptr){
		return nullptr;
	}

	holeyc::ProgramNode * root = nullptr;

	holeyc::Scanner scanner(input);
	#if 1
	holeyc::Parser parser(scanner, &root);
	#else
	holeyc::Parser parser(scanner);
	#endif

	int errCode = parser.parse();
	if (errCode != 0) { 
		return nullptr; 
	}
	
	return root;
}

static void outputAST(ASTNode * ast, const char * outPath){
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

static bool doUnparsing(std::ifstream * input, const char * outPath){
	holeyc::ProgramNode * ast = syntacticAnalysis(input);
	if (ast == nullptr){ 
		std::cerr << "No AST built\n";
		return false;
	}
	if (input == nullptr){ 
		return false; 
	}

	outputAST(ast, outPath);
	return true;
}

static holeyc::NameAnalysis * doNameAnalysis(std::ifstream * input){
	holeyc::ProgramNode * ast = syntacticAnalysis(input);
	if (ast == nullptr){ return nullptr; }

	return holeyc::NameAnalysis::build(ast);
}

static holeyc::TypeAnalysis * doTypeAnalysis(std::ifstream * input){
	holeyc::NameAnalysis * nameAnalysis = doNameAnalysis(input);
	if (nameAnalysis == nullptr){ return nullptr; }

	return holeyc::TypeAnalysis::build(nameAnalysis);
}

static holeyc::IRProgram * do3AC(std::ifstream * input){
	holeyc::TypeAnalysis * typeAnalysis = doTypeAnalysis(input);
	if (typeAnalysis == nullptr){ return nullptr; }

	return typeAnalysis->ast->to3AC(typeAnalysis);
}

static void write3AC(holeyc::IRProgram * prog, const char * outPath){
	if (outPath == nullptr){
		throw new InternalError("Null 3AC file given");
	}
	std::string flatProg = prog->toString();
	if (strcmp(outPath, "--") == 0){
		std::cout << flatProg << std::endl;
	} else {
		std::ofstream outStream(outPath);
		outStream << flatProg << std::endl;
		outStream.close();
	}
}

int main(int argc, char * argv[]){
	if (argc <= 1){ usageAndDie(); }
	std::ifstream * input = new std::ifstream(argv[1]);
	if (input == NULL){ usageAndDie(); }
	if (!input->good()){
		std::cerr << "Bad path " <<  argv[1] << std::endl;
		usageAndDie();
	}

	const char * tokensFile = nullptr; // Output file if 
	                                   // printing tokens
	bool checkParse = false;	   // Flag set if doing 
					   // syntactic analysis
	const char * unparseFile = NULL;   // Output file if 
	                                   // unparsing
	const char * nameFile = NULL;	   // Output file if doing
					   // name analysis
	bool useful = false; // Check whether the command is 
                         // a no-op
	bool checkTypes = false;	   // Flag set if doing 
					   // syntactic analysis
	const char * threeACFile = NULL;	   // Output file if doing
					   // 3AC conversion
	for (int i = 1; i < argc; i++){
		if (argv[i][0] == '-'){
			if (argv[i][1] == 't'){
				i++;
				if (i >= argc){ usageAndDie(); }
				tokensFile = argv[i];
				useful = true;
			} else if (argv[i][1] == 'p'){
				i++;
				checkParse = true;
				useful = true;
			} else if (argv[i][1] == 'u'){
				i++;
				if (i >= argc){ usageAndDie(); }
				unparseFile = argv[i];
				useful = true;
			} else if (argv[i][1] == 'n'){
				i++;
				if (i >= argc){ usageAndDie(); }
				nameFile = argv[i];
				useful = true;
			} else if (argv[i][1] == 'c'){
				i++;
				checkTypes = true;
				useful = true;
			} else if (argv[i][1] == 'a'){
				i++;
				if (i >= argc){ usageAndDie(); }
				threeACFile = argv[i];
				useful = true;
			} else {
				std::cerr << "Unknown option"
				  << " " << argv[i] << "\n";
				usageAndDie();
			}
		}
	}

	if (useful == false){
		std::cerr << "You didn't specify an operation to do!\n";
		usageAndDie();
	}


	try {
		if (tokensFile != nullptr){
			doTokenization(input, tokensFile);
		}
		if (checkParse){
			if (!syntacticAnalysis(input)){
				std::cerr << "Parse failed";
			}
		}
		if (unparseFile != nullptr){
			doUnparsing(input, unparseFile);
		}
		if (nameFile){
			holeyc::NameAnalysis * na;
			na = doNameAnalysis(input); 
			if (na != nullptr){
				outputAST(na->ast, nameFile);
				return 0;
			}
			std::cerr << "Name Analysis Failed\n";
			return 1;
		}
		if (checkTypes){
			if (doTypeAnalysis(input) != nullptr){
				return 0;
			}
			std::cerr << "Type Analysis Failed\n";
			return 1;
		}
		if (threeACFile){
			if (auto prog = do3AC(input)){
				write3AC(prog, threeACFile);
			}
			if (doTypeAnalysis(input) != nullptr){
				return 0;
			}
			std::cerr << "Type Analysis Failed\n";
			return 1;
		}
	} catch (holeyc::ToDoError * e){
		std::cerr << "ToDoError: " << e->msg() << "\n";
		return 1;
	} catch (holeyc::InternalError * e){
		std::cerr << "InternalError: " << e->msg() << "\n";
		return 1;
	}

	return 0;
}
