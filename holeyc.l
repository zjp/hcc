%{
#include <string>
#include <limits.h>

/* Get our custom yyFlexScanner subclass */
#include "scanner.hpp"
#undef YY_DECL
#define YY_DECL int holeyc::Scanner::yylex(holeyc::Parser::semantic_type * const lval)

using TokenKind = holeyc::Parser::token;

/* define yyterminate as returning an EOF token (instead of NULL) */
#define yyterminate() return ( TokenKind::END )

/* exclude unistd.h for Visual Studio compatibility. */
#define YY_NO_UNISTD_H

#define EXIT_ON_ERR 0


%}

%option nodefault
%option yyclass="holeyc::Scanner"
%option noyywrap
%option c++

DIGIT [0-9]
WHITESPACE [\040\t]
LETTER [a-zA-Z]
ESCAPEE [nt'"\\]

 /* */ 
NOT_NL_OR_ESCAPEE [^\nnt"\\]

NOT_NL_OR_DQ [^\n"]
NOT_NL_OR_DQ_OR_ESC [^\n"\\]

NOT_NL_OR_SQ [^\n']

%%
%{
	this->yylval = lval;
%}

int           { return makeBareToken(TokenKind::INT); }
intptr  	    { return makeBareToken(TokenKind::INTPTR); }
bool 		      { return makeBareToken(TokenKind::BOOL); }
boolptr		    { return makeBareToken(TokenKind::BOOLPTR); }
char 		      { return makeBareToken(TokenKind::CHAR); }
charptr 	    { return makeBareToken(TokenKind::CHARPTR); }
void 		      { return makeBareToken(TokenKind::VOID); }
if  		      { return makeBareToken(TokenKind::IF); }
else		      { return makeBareToken(TokenKind::ELSE); }
while		      { return makeBareToken(TokenKind::WHILE); }
return		    { return makeBareToken(TokenKind::RETURN); }
false  		    { return makeBareToken(TokenKind::FALSE); }
true 		    { return makeBareToken(TokenKind::TRUE); }
"FROMCONSOLE"	{ return makeBareToken(TokenKind::FROMCONSOLE);}
"TOCONSOLE"	  { return makeBareToken(TokenKind::TOCONSOLE); }
"NULLPTR"	    { return makeBareToken(TokenKind::NULLPTR); }
"@"		        { return makeBareToken(TokenKind::AT); }
"^"		        { return makeBareToken(TokenKind::CARAT); }
"["		        { return makeBareToken(TokenKind::LBRACE); }
"]"		        { return makeBareToken(TokenKind::RBRACE); }
"{"		        { return makeBareToken(TokenKind::LCURLY); }
"}"		        { return makeBareToken(TokenKind::RCURLY); }
"("		        { return makeBareToken(TokenKind::LPAREN); }
")"		        { return makeBareToken(TokenKind::RPAREN); }
";"		        { return makeBareToken(TokenKind::SEMICOLON); }
","		        { return makeBareToken(TokenKind::COMMA); }
"++"          { return makeBareToken(TokenKind::CROSSCROSS); }
"+"           { return makeBareToken(TokenKind::CROSS); }
"--"          { return makeBareToken(TokenKind::DASHDASH); }
"-"		        { return makeBareToken(TokenKind::DASH); }
"*"		        { return makeBareToken(TokenKind::STAR); }
"/"		        { return makeBareToken(TokenKind::SLASH); }
"!"	 	        { return makeBareToken(TokenKind::NOT); }
"&&"          { return makeBareToken(TokenKind::AND); }
"||"          { return makeBareToken(TokenKind::OR); }
"=="          { return makeBareToken(TokenKind::EQUALS); }
"!="          { return makeBareToken(TokenKind::NOTEQUALS); }
"<"		        { return makeBareToken(TokenKind::LESS); }
"<="          { return makeBareToken(TokenKind::LESSEQ); }
">"		        { return makeBareToken(TokenKind::GREATER); }
">="          { return makeBareToken(TokenKind::GREATEREQ); }
"="		        { return makeBareToken(TokenKind::ASSIGN); }
\'\\[tn\\]	  { return makeCharLitToken(yytext); }
\'\\\t	  	{ return makeCharLitToken("'\t"); }
\'\\[ ]  	{ return makeCharLitToken("' "); }
\'\\	        { errChrEscEmpty(lineNum, colNum);
                colNum += yyleng; }
\'\\[^\ntn\\] { errChrEsc(lineNum, colNum);
                colNum += yyleng; }
\'\t		      { return makeCharLitToken("'\t"); }
\'[^\n\\]     { return makeCharLitToken(yytext); }
\'\n          { errChrEmpty(lineNum, colNum); 
                colNum = 1;
                lineNum++; }
({LETTER}|_)({LETTER}|{DIGIT}|_)* { 
		            yylval->transToken = 
		            new IDToken(lineNum, colNum, yytext);
		            colNum += yyleng;
		            return TokenKind::ID; }

{DIGIT}+	    { double asDouble = std::stod(yytext);
			          int intVal = atoi(yytext);
			          bool overflow = false;
			          if (asDouble > INT_MAX){ overflow = true; }
			          if (strlen(yytext) > 10){ overflow = true; }

			          if (overflow){
				            errIntOverflow(lineNum, colNum);
				            intVal = INT_MAX;
			          }
			          yylval->transToken = 
			              new IntLitToken(lineNum, colNum, intVal);
			          colNum += yyleng;
			          return TokenKind::INTLITERAL; }

\"({NOT_NL_OR_DQ_OR_ESC}|\\{ESCAPEE})*\" {
   		          yylval->transToken = 
                    new StrToken(lineNum, colNum, yytext);
		            this->colNum += yyleng;
		            return TokenKind::STRLITERAL; }

\"({NOT_NL_OR_DQ_OR_ESC}|\\{ESCAPEE})* {
		            errStrUnterm(lineNum, colNum);
		            colNum = 1; /*Upcoming \n resets lineNum */
			    #if EXIT_ON_ERR
			    exit(1);
			    #endif
		            }

\"({NOT_NL_OR_DQ_OR_ESC}|\\{ESCAPEE})*\\{NOT_NL_OR_ESCAPEE}({NOT_NL_OR_DQ})*\" {
		            errStrEsc(lineNum, colNum);
		            colNum += yyleng; 
			    #if EXIT_ON_ERR
			    exit(1);
			    #endif
				}

\"({NOT_NL_OR_DQ_OR_ESC}|\\{ESCAPEE})*(\\{NOT_NL_OR_ESCAPEE})?({NOT_NL_OR_DQ_OR_ESC}|\\{ESCAPEE})*\\? {
		            errStrEscAndUnterm(lineNum, colNum);
		            colNum = 1; 
			    #if EXIT_ON_ERR
			    exit(1);
			    #endif
				}

\n|(\r\n)     { lineNum++; colNum = 1; }


[ \t]+	      { colNum += yyleng; }

("#")[^\n]*	  { /* Comment. Ignore. Don't need to update 
                   char num since everything up to end of 
                   line will never by part of a report*/ }

.		          { errIllegal(lineNum, colNum, yytext);
			    #if EXIT_ON_ERR
			    exit(1);
			    #endif
		            this->colNum += yyleng; }
%%
