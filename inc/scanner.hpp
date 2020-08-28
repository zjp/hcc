#ifndef __HOLYC_SCANNER_HPP__
#define __HOLYC_SCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "grammar.hh"
#include "errors.hpp"

using TokenKind = holyc::Parser::token;

namespace holyc{

class Scanner : public yyFlexLexer{
public:
   
   Scanner(std::istream *in) : yyFlexLexer(in)
   {
	lineNum = 1;
	colNum = 1;
   };
   virtual ~Scanner() {
   };

   //get rid of override virtual function warning
   using FlexLexer::yylex;

   // YY_DECL defined in the flex holyc.l
   virtual int yylex( holyc::Parser::semantic_type * const lval);

   int makeBareToken(int tagIn){
        this->yylval->tokenValue = new Token(
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
	}
	this->yylval->tokenValue = new CharLitToken(
		this->lineNum, this->colNum, val);
	colNum += static_cast<size_t>(yyleng);
	return TokenKind::CHARLIT;
   }

   void errIllegal(size_t l, size_t c, std::string match){
	holyc::Report::fatal(l, c, "Illegal character "
		+ match);
   }

   void errChrEscEmpty(size_t l, size_t c){
	holyc::Report::fatal(l, c, "Empty escape sequence in"
	" character literal");
   }

   void errChrEmpty(size_t l, size_t c){
	holyc::Report::fatal(l, c, "Empty character literal");
   }

   void errChrEsc(size_t l, size_t c){
	holyc::Report::fatal(l, c, "Bad escape sequence in"
	" char literal");
   }

   void errStrEsc(size_t l, size_t c){
	holyc::Report::fatal(l, c, "String literal with bad"
	" escape sequence ignored");
	
   }

   void errStrUnterm(size_t l, size_t c){
	holyc::Report::fatal(l, c, "Unterminated string"
	" literal ignored");
	
   }

   void errStrEscAndUnterm(size_t l, size_t c){
	holyc::Report::fatal(l, c, "Unterminated string literal"
	"  with bad escape sequence ignored");
   }

   void errIntOverflow(size_t l, size_t c){
	holyc::Report::fatal(l, c, "Integer literal too large;"
	"  using max value");
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
   holyc::Parser::semantic_type *yylval = nullptr;
   size_t lineNum;
   size_t colNum;
};

} /* end namespace */

#endif /* END __HOLYC_SCANNER_HPP__ */
