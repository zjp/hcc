%{
#include <string>
#include <limits.h>

/* Get our custom yyFlexScanner subclass */
#include "scanner.hpp"
#include "tokens.hpp"
#undef  YY_DECL
#define YY_DECL int holyc::Scanner::yylex( holyc::Parser::semantic_type * const lval )

/* define yyterminate as this instead of NULL */
#define yyterminate() return( TokenKind::END )

/* Exclude unistd.h for Visual Studio compatability. */
#define YY_NO_UNISTD_H

/* typedef to make the reference to token kinds shorter */
using TokenKind = holyc::Parser::token;

%}

%option debug
%option nodefault
%option yyclass="holyc::Scanner"
%option noyywrap
%option c++

DIGIT [0-9]
IDENTIFIER [a-zA-Z_][a-zA-Z0-9_]*
STRINGLITERAL ".*"
COMMENT #.*

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

"int" {
	int tokenKind = TokenKind::INT;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"intptr" {
	int tokenKind = TokenKind::INTPTR;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"bool" {
	int tokenKind = TokenKind::BOOL;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"boolptr" {
	int tokenKind = TokenKind::BOOLPTR;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"char" {
	int tokenKind = TokenKind::CHAR;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"charptr" {
	int tokenKind = TokenKind::CHARPTR;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"void" {
	int tokenKind = TokenKind::VOID;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"if" {
	int tokenKind = TokenKind::IF;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"else" {
	int tokenKind = TokenKind::ELSE;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"while" {
	int tokenKind = TokenKind::WHILE;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"return" {
	int tokenKind = TokenKind::RETURN;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"true" {
	int tokenKind = TokenKind::TRUE;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"false" {
	int tokenKind = TokenKind::FALSE;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"FROMCONSOLE" {
	int tokenKind = TokenKind::FROMCONSOLE;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"TOCONSOLE" {
	int tokenKind = TokenKind::TOCONSOLE;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"NULLPTR" {
	int tokenKind = TokenKind::NULLPTR;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += yyleng;
	return tokenKind;
}

"@" {
		int tokenKind = TokenKind::AT;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

"^" {
		int tokenKind = TokenKind::CARAT;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

"[" {
		int tokenKind = TokenKind::LBRACE;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

"]" {
		int tokenKind = TokenKind::RBRACE;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

"{" {
		int tokenKind = TokenKind::LCURLY;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

"}" {
		int tokenKind = TokenKind::RCURLY;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

"(" {
		int tokenKind = TokenKind::LPAREN;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

")" {
		int tokenKind = TokenKind::RPAREN;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

";" {
		int tokenKind = TokenKind::SEMICOLON;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

"," {
		int tokenKind = TokenKind::COMMA;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

"--" {
		int tokenKind = TokenKind::DASHDASH;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum += yyleng;
		return tokenKind;
}

"-" {
		int tokenKind = TokenKind::DASH;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

"+"	{
		// Keep track of the "kind" of token we matched. In this case,
		// it's a CROSS (aka the plus symbol). The full list of token
		// kinds is declared around line 197 of grammar.hh. The testing
		// functionScanner::outputTokens also indicates what each token
		// represents (declared in scanner.cpp line 9)
		int tokenKind = TokenKind::CROSS;

		//Instatiate a token. There are 4 classes of tokens defined based on 
		// - a literal string (StringLitToken)
		// - a literal integer (IntLitToken)
		// - an identifier (IDToken)
		// - a base Token (any other type)
		// The token object is a handy place to keep information about the
		// token (i.e. token metadata) such as the line and column on which
		// the token character started (which can be used later in debug 
		// messages, etc). 
		Token * token = new Token(lineNum, colNum, tokenKind);

		//Assign the token to a instance variable of the scanner.
		// (When Flex processes a .l file in C++ mode, it creates 
		// a class called Scanner, which is a subclass of yyFlexScanner)
		// By assigning a value to this field, we put the token
		// metadata in a place where scanner's client program can find it.
		this->yylval->tokenValue = token;

		//Since we consumed 1 character, increase the column number by 1
		colNum++;

		//Let the scanner's client program know that a CROSS token
		// has been matched
		return tokenKind;
		}

"++" {
		int tokenKind = TokenKind::CROSSCROSS;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum += yyleng;
		return tokenKind;
}

"*" {
		int tokenKind = TokenKind::STAR;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

"/" {
		int tokenKind = TokenKind::SLASH;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

"!" {
		int tokenKind = TokenKind::NOT;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

"==" {
		int tokenKind = TokenKind::EQUALS;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum += yyleng;
		return tokenKind;
}

"!=" {
		int tokenKind = TokenKind::NOTEQUALS;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum += yyleng;
		return tokenKind;
}

"<" {
		int tokenKind = TokenKind::LESS;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

">" {
		int tokenKind = TokenKind::GREATER;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

"&" {
		int tokenKind = TokenKind::AMP;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}

"<=" {
		int tokenKind = TokenKind::LESSEQ;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum += yyleng;
		return tokenKind;
}

">=" {
		int tokenKind = TokenKind::GREATEREQ;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum += yyleng;
		return tokenKind;
}

"=" {
		int tokenKind = TokenKind::ASSIGN;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
}


"&&" {
		int tokenKind = TokenKind::AND;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum += yyleng;
		return tokenKind;
}


"||" {
		int tokenKind = TokenKind::OR;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum += yyleng;
		return tokenKind;
}

{DIGIT}+ {
		int intVal = 0;
		if(strtol(yytext, nullptr, 0) > INT_MAX) {
			intVal = INT_MAX;
			errIntOverflow(lineNum,colNum);
		} else {
			intVal = atoi(yytext);
		}
		yylval->tokenValue = new IntLitToken(lineNum, colNum, intVal);
		colNum += yyleng;
		return TokenKind::INTLITERAL;
}

{IDENTIFIER} {
		int tokenKind = TokenKind::ID;
		this->yylval->tokenValue = new IDToken(lineNum, colNum, yytext);
		colNum += yyleng;
		return tokenKind;
}

	/* 1. ASCII space -> ASCII ] 
	 * 2. ASCII [ -> ASCII DEL 
	 * 3. '\\
	 * 4. '\t
	 * 5. '\n 
	 * 6. '\(space literal) */
'([\x20-\x5B]|[\x5D-\x7F]|\\\\|\\t|\\n|\\[ ]) {
		return makeCharLitToken(yytext);
}
	
	/* '\(tab literal) */
'\\[	] {
		return makeCharLitToken(yytext);
}

	/* '\(any other char) -- Bad escape sequence */
'\\. {
		errChrEsc(lineNum, colNum);
		colNum += yyleng;
}
	
	/* '\(no char) -- Empty char literal escape sequence */
'\\ {
		errChrEscEmpty(lineNum, colNum);
		colNum = 1;
}

	/* '(no char) -- Empty char literal */
' {
		errChrEmpty(lineNum, colNum);
		colNum = 1;
}

	/* '(any other char) -- Illegal Character */
'. {
		errIllegal(lineNum, colNum, yytext);
}

\".*\\[nt ]?.*\" {
		int tokenKind = TokenKind::STRLITERAL;
		this->yylval->tokenValue = new StrToken(lineNum, colNum, yytext);
		colNum += yyleng;
		return tokenKind;
}

\".*\\.*\" {
		errStrEsc(lineNum, colNum);
		colNum += yyleng;
}


{STRINGLITERAL} {
		int tokenKind = TokenKind::STRLITERAL;
		this->yylval->tokenValue = new StrToken(lineNum, colNum, yytext);
		colNum += yyleng;
		return tokenKind;
}

\".*\\[nt ]?.* {
		errStrUnterm(lineNum, colNum);
		colNum += yyleng;
}

\".*\\.* {
		errStrEscAndUnterm(lineNum, colNum);
		colNum += yyleng;
}

\".* {
		errStrUnterm(lineNum, colNum);
		colNum += yyleng;
}

{COMMENT} {
	/* Do nothing */
}
	
	/* Whitespace */
	
	/* Load the following program into a C compiler and observe the error 
	 * generated implies a tab is 1 character:
	 * 
	 * int main() {
	 *	a;
	 * }
	 */
\t { 
		colNum++;
}

[ ] {
		colNum++;
}

\n {
		//Update the line number, reset the columnNumber
		lineNum++;
		colNum = 1;
}

. {
		//TODO: Add the rest of the rules above this point so that legal characters
		// are properly accounted for. 
		std::string msg = "Illegal character ";
		msg += yytext;
		Report::fatal(lineNum,colNum,msg);
		colNum += yyleng;
}
%%
