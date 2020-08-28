#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include "scanner.hpp"
#include "errors.hpp"

static void usageAndDie(){
	std::cerr << "Usage: holycc <infile>"
	<< " [-t <tokensFile>]"
	<< "\n";
	exit(1);
}

static void doTokenization(std::ifstream * input, const char * outPath){
	holyc::Scanner scanner(input);
	if (strcmp(outPath, "--") == 0){
		scanner.outputTokens(std::cout);
	} else {
		std::ofstream outStream(outPath);
		if (!outStream.good()){
			std::string msg = "Bad output file ";
			msg += outPath;
			throw new holyc::InternalError(msg.c_str());
		}
		scanner.outputTokens(outStream);
	}
}

int main(int argc, char * argv[]){
	if (argc == 1){ usageAndDie(); }
	std::ifstream * input = new std::ifstream(argv[1]);

	const char * tokensFile = nullptr; // Output file if 
	                                   // printing tokens
	bool useful = false; // Check whether the command is 
                         // a no-op
	for (int i = 1; i < argc; i++){
		if (argv[i][0] == '-'){
			if (argv[i][1] == 't'){
				i++;
				tokensFile = argv[i];
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

	if (tokensFile != nullptr){
		doTokenization(input, tokensFile);
	}
	return 0;
}
