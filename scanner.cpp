#include <fstream>
#include "scanner.hpp"

using namespace holeyc;

using TokenKind = holeyc::Parser::token;
using Lexeme = holeyc::Parser::semantic_type;

void Scanner::outputTokens(std::ostream& outstream){
	Lexeme lexeme;
	int tokenKind;
	while(true){
		tokenKind = this->yylex(&lexeme);
		if (tokenKind == TokenKind::END){
			outstream << "EOF" 
			  << " [" << this->lineNum 
			  << "," << this->colNum << "]"
			  << std::endl;
			return;
		} else {
			outstream << lexeme.transToken->toString()
			  << std::endl;
		}
	}
}
