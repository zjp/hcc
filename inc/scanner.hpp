#ifndef __HOLYC_SCANNER_HPP__
#define __HOLYC_SCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "grammar.hh"
#include "errors.hpp"

using TokenKind = holeyc::Parser::token;

namespace holeyc{

class Scanner : public yyFlexLexer{
public:
   
   Scanner(std::istream *in) : yyFlexLexer(in)
   {
	lineNum = 1;
	colNum = 1;
	hasError = false;
   };
   virtual ~Scanner() {
   };

   //get rid of override virtual function warning
   using FlexLexer::yylex;

   // YY_DECL defined in the flex holeyc.l
   virtual int yylex( holeyc::Parser::semantic_type * const lval);

   int makeBareToken(int tagIn){
        this->yylval->transToken = new Token(
	  this->lineNum, this->colNum, tagIn);
        colNum += static_cast<size_t>(yyleng);
        return tagIn;
   }

   int makeCharLitToken(const std::string text){
	char val;
	if (text.length() == 2){
		val = text.c_str()[1];
	} else if (text == "'\\t"){
		val = '\t';
	} else if (text == "'\\n"){
		val = '\n';
	} else if (text == "'\\\\"){
		val = '\\';
	} else {
		val = text.c_str()[1];
	}
	this->yylval->transToken = new CharLitToken(
		this->lineNum, this->colNum, val);
	colNum += static_cast<size_t>(yyleng);
	return TokenKind::CHARLIT;
   }

   void errIllegal(size_t l, size_t c, std::string match){
	Report::fatal(l, c, "Illegal character "
		+ match);
	hasError = true;
   }

   void errChrEscEmpty(size_t l, size_t c){
	Report::fatal(l, c, "Empty escape sequence in"
	" character literal");
	hasError = true;
   }

   void errChrEmpty(size_t l, size_t c){
	Report::fatal(l, c, "Empty character literal");
	hasError = true;
   }

   void errChrEsc(size_t l, size_t c){
	Report::fatal(l, c, "Bad escape sequence in"
	" char literal");
	hasError = true;
   }

   void errStrEsc(size_t l, size_t c){
	Report::fatal(l, c, "String literal with bad"
	" escape sequence ignored");
	hasError = true;
   }

   void errStrUnterm(size_t l, size_t c){
	Report::fatal(l, c, "Unterminated string"
	" literal ignored");
	hasError = true;
	
   }

   void errStrEscAndUnterm(size_t l, size_t c){
	Report::fatal(l, c, "Unterminated string literal"
	"  with bad escape sequence ignored");
	hasError = true;
   }

   void errIntOverflow(size_t l, size_t c){
	Report::fatal(l, c, "Integer literal too large;"
	"  using max value");
	hasError = true;
   }

   void warn(int lineNumIn, int colNumIn, std::string msg){
	std::cerr << lineNumIn << ":" << colNumIn 
		<< " ***WARNING*** " << msg << std::endl;
   }

   void error(int lineNumIn, int colNumIn, std::string msg){
	std::cerr << lineNumIn << ":" << colNumIn 
		<< " ***ERROR*** " << msg << std::endl;
   }

   static std::string tokenKindString(int tokenKind);

   void outputTokens(std::ostream& outstream);

private:
   holeyc::Parser::semantic_type *yylval = nullptr;
   size_t lineNum;
   size_t colNum;
   bool hasError;
};

} /* end namespace */

#endif /* END __NEGA_SCANNER_HPP__ */
