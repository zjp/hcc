%skeleton "lalr1.cc"
%require "3.0"
%debug
%defines
%define api.namespace{holeyc}
%define api.parser.class {Parser}
%define parse.error verbose
%output "parser.cc"
%token-table

%code requires{
	#include <list>
	#include "tokens.hpp"
	#include "ast.hpp"
	namespace holeyc {
		class Scanner;
	}

//The following definition is required when 
// we don't have the %locations directive
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

//End "requires" code
}

%parse-param { holeyc::Scanner &scanner }
%parse-param { holeyc::ProgramNode** root }

%code{
   // C std code for utility functions
   #include <iostream>
   #include <cstdlib>
   #include <fstream>

   // Our code for interoperation between scanner/parser
   #include "scanner.hpp"
   #include "ast.hpp"
   #include "tokens.hpp"

  //Request tokens from our scanner member, not 
  // from a global function
  #undef yylex
  #define yylex scanner.yylex
}


/*
The %union directive is a way to specify the 
set of possible types that might be used as
translation attributes in the syntax-directed
translations. 

TODO: You will have to add to this list to 
create new translation value types
*/
%union {
   holeyc::Token *                     transToken;
   holeyc::IDToken *                   transIDToken;
   holeyc::ProgramNode *               transProgram;
   std::list<holeyc::DeclNode *> *     transDeclList;
   holeyc::DeclNode *                  transDecl;
   holeyc::VarDeclNode *               transVarDecl;
   holeyc::TypeNode *                  transType;
   holeyc::IDNode *                    transID;
}

%define parse.assert

%token                   END	   0 "end file"
%token	<transToken>     AND
%token	<transToken>     AT
%token	<transToken>     ASSIGN
%token	<transToken>     BOOL
%token	<transToken>     BOOLPTR
%token	<transToken>     CARAT
%token	<transToken>     CHAR
%token	<transCharToken> CHARLIT
%token	<transToken>     CHARPTR
%token	<transToken>     COMMA
%token	<transToken>     CROSS
%token	<transToken>     CROSSCROSS
%token	<transToken>     DASH
%token	<transToken>     DASHDASH
%token	<transToken>     ELSE
%token	<transToken>     EQUALS
%token	<transToken>     FALSE
%token	<transToken>     FROMCONSOLE
%token	<transIDToken>   ID
%token	<transToken>     IF
%token	<transToken>     INT
%token	<transIntToken>  INTLITERAL
%token	<transToken>     INTPTR
%token	<transToken>     GREATER
%token	<transToken>     GREATEREQ
%token	<transToken>     LBRACE
%token	<transToken>     LCURLY
%token	<transToken>     LESS
%token	<transToken>     LESSEQ
%token	<transToken>     LPAREN
%token	<transToken>     NOT
%token	<transToken>     NOTEQUALS
%token	<transToken>     NULLPTR
%token	<transToken>     OR
%token	<transToken>     RBRACE
%token	<transToken>     RCURLY
%token	<transToken>     RETURN
%token	<transToken>     RPAREN
%token	<transToken>     SEMICOLON
%token	<transToken>     SLASH
%token	<transToken>     STAR
%token	<transStrToken>  STRLITERAL
%token	<transToken>     TOCONSOLE
%token	<transToken>     TRUE
%token	<transToken>     VOID
%token	<transToken>     WHILE


/* Nonterminals
*  TODO: You will need to add more nonterminals
*  to this list as you add productions to the grammar
*  below (along with indicating the appropriate translation
*  attribute type). Note that the specifier in brackets
*  indicates the type of the translation attribute using
*  the names defined in the %union directive above
*/
/*    (attribute type)    (nonterminal)    */
%type <transProgram>    program
%type <transDeclList>   globals
%type <transDecl>       decl
%type <transVarDecl>    varDecl
%type <transType>       type
%type <transID>         id

%right ASSIGN
%left OR
%left AND
%nonassoc LESS GREATER LESSEQ GREATEREQ EQUALS NOTEQUALS
%left DASH CROSS
%left STAR SLASH
%left NOT 
%left AT CARAT

%%

program 	: globals
		  {
		  $$ = new ProgramNode($1);
		  *root = $$;
		  }

globals 	: globals decl 
		  {
		  $$ = $1;
		  DeclNode * aGlobalDecl = $2;
		  $1->push_back(aGlobalDecl);
		  }
		| /* epsilon */
		  {
		  std::list<DeclNode *> * startingGlobals;
		  startingGlobals = new std::list<DeclNode *>();
	 	  $$ = startingGlobals;
		  }
		; 

decl 		: varDecl SEMICOLON
		  {
		  //TODO: Make sure to fill out this rule
		  // (as well as any other empty rule!)
		  }
		| fnDecl 
		  { }

varDecl 	: type id
		  { 
		  size_t typeLine = $1->line();
		  size_t typeCol = $1->col();
		  $$ = new VarDeclNode(typeLine, typeCol, $1, $2);
		  }

type 		: INT
		  {
		  bool isPtr = false;
		  $$ = new IntTypeNode($1->line(), $1->col(), isPtr);
		  }
		| INTPTR
		  { }
		| BOOL
		  { }
		| BOOLPTR
		  { }
		| CHAR
		  { }
		| CHARPTR
		  { }
		| VOID
		  { }

fnDecl 		: type id formals fnBody
		  { }

formals 	: LPAREN RPAREN
		  { }
		| LPAREN formalsList RPAREN
		  { }


formalsList	: formalDecl
		  { }
		| formalDecl COMMA formalsList 
		  { }

formalDecl 	: type id
		  { }

fnBody		: LCURLY stmtList RCURLY
		  { }

stmtList 	: /* epsilon */
		  { }
		| stmtList stmt
		  { }

stmt		: varDecl SEMICOLON
		  { }
		| assignExp SEMICOLON
		  { }
		| lval DASHDASH SEMICOLON
		  { }
		| lval CROSSCROSS SEMICOLON
		  { }
		| FROMCONSOLE lval SEMICOLON
		  { }
		| TOCONSOLE exp SEMICOLON
		  { }
		| IF LPAREN exp RPAREN LCURLY stmtList RCURLY
		  { }
		| IF LPAREN exp RPAREN LCURLY stmtList RCURLY ELSE LCURLY stmtList RCURLY
		  { }
		| WHILE LPAREN exp RPAREN LCURLY stmtList RCURLY
		  { }
		| RETURN exp SEMICOLON
		  { }
		| RETURN SEMICOLON
		  { }
		| callExp SEMICOLON
		  { }

exp		: assignExp 
		  { }
		| exp DASH exp
		  { }
		| exp CROSS exp
		  { }
		| exp STAR exp
		  { }
		| exp SLASH exp
		  { }
		| exp AND exp
		  { }
		| exp OR exp
		  { }
		| exp EQUALS exp
		  { }
		| exp NOTEQUALS exp
		  { }
		| exp GREATER exp
		  { }
		| exp GREATEREQ exp
		  { }
		| exp LESS exp
		  { }
		| exp LESSEQ exp
		  { }
		| NOT exp
		  { }
		| DASH term
		  { }
		| term 
		  { }

assignExp	: lval ASSIGN exp
		  { }

callExp		: id LPAREN RPAREN
		  { }
		| id LPAREN actualsList RPAREN
		  { }

actualsList	: exp
		  { }
		| actualsList COMMA exp
		  { }

term 		: lval
		  { }
		| callExp
		  { }
		| NULLPTR
		  { }
		| INTLITERAL 
		  { }
		| STRLITERAL 
		  { }
		| CHARLIT 
		  { }
		| TRUE
		  { }
		| FALSE
		  { }
		| LPAREN exp RPAREN
		  { }

lval		: id
		  {
		  }
		| id LBRACE exp RBRACE
		  {
		  }
		| AT id
		  {
		  }
		| CARAT id
		  {
		  }

id		: ID
		  {
		  $$ = new IDNode($1);
		  }
	
%%

void holeyc::Parser::error(const std::string& msg){
	std::cout << msg << std::endl;
	std::cerr << "syntax error" << std::endl;
}
