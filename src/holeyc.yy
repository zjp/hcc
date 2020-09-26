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
    holeyc::Token*                      transToken;
    holeyc::IDToken*                    transIDToken;
    holeyc::IntLitToken*                transIntToken;
    holeyc::StrToken*                   transStrToken;
    holeyc::CharLitToken*               transCharToken;

    std::list<holeyc::DeclNode*>*       transDeclList;
    std::list<holeyc::FormalDeclNode*>* transFormalDeclList;
    std::list<holeyc::StmtNode*>*       transStmtList;
    std::list<holeyc::ExpNode*>*        transActualsList;

    holeyc::AndNode*                    transAnd;
    holeyc::DivideNode*                 transDivide;
    holeyc::EqualsNode*                 transEquals;
    holeyc::GreaterEqNode*              transGreaterEq;
    holeyc::GreaterNode*                transGreater;
    holeyc::LessEqNode*                 transLessEq;
    holeyc::LessNode*                   transLess;
    holeyc::MinusNode*                  transMinus;
    holeyc::NotEqualsNode*              transNotEquals;
    holeyc::OrNode*                     transOr;
    holeyc::PlusNode*                   transPlus;
    holeyc::TimesNode*                  transTimes;
    holeyc::DerefNode*                  transDeref;
    holeyc::IDNode*                     transID;
    holeyc::IndexNode*                  transIndex;
    holeyc::RefNode*                    transRef;
    holeyc::NegNode*                    transNeg;
    holeyc::NotNode*                    transNot;
    holeyc::FnDeclNode*                 transFnDecl;
    holeyc::FormalDeclNode*             transFormalDecl;
    holeyc::VarDeclNode*                transVarDecl;
    holeyc::AssignExpNode*              transAssignExp;
    holeyc::BinaryExpNode*              transBinaryExp;
    holeyc::CallExpNode*                transCallExp;
    holeyc::CharLitNode*                transCharLit;
    holeyc::FalseNode*                  transFalse;
    holeyc::IntLitNode*                 transIntLit;
    holeyc::LValNode*                   transLVal;
    holeyc::NullPtrNode*                transNullPtr;
    holeyc::StrLitNode*                 transStrLit;
    holeyc::TrueNode*                   transTrue;
    holeyc::UnaryExpNode*               transUnaryExp;
    holeyc::AssignStmtNode*             transAssignStmt;
    holeyc::CallStmtNode*               transCallStmt;
    holeyc::DeclNode*                   transDecl;
    holeyc::FromConsoleStmtNode*        transFromConsoleStmt;
    holeyc::IfElseStmtNode*             transIfElseStmt;
    holeyc::IfStmtNode*                 transIfStmt;
    holeyc::PostDecStmtNode*            transPostDecStmt;
    holeyc::PostIncStmtNode*            transPostIncStmt;
    holeyc::ReturnStmtNode*             transReturnStmt;
    holeyc::ToConsoleStmtNode*          transToConsoleStmt;
    holeyc::WhileStmtNode*              transWhileStmt;
    holeyc::BoolTypeNode*               transBoolType;
    holeyc::CharTypeNode*               transCharType;
    holeyc::IntTypeNode*                transIntType;
    holeyc::VoidTypeNode*               transVoidType;
    holeyc::ExpNode*                    transExp;
    holeyc::ProgramNode*                transProgram;
    holeyc::StmtNode*                   transStmt;
    holeyc::TypeNode*                   transType;
}

%define parse.assert

%token                   END     0 "end file"
%token  <transToken>     AND
%token  <transToken>     AT
%token  <transToken>     ASSIGN
%token  <transToken>     BOOL
%token  <transToken>     BOOLPTR
%token  <transToken>     CARAT
%token  <transToken>     CHAR
%token  <transCharToken> CHARLIT
%token  <transToken>     CHARPTR
%token  <transToken>     COMMA
%token  <transToken>     CROSS
%token  <transToken>     CROSSCROSS
%token  <transToken>     DASH
%token  <transToken>     DASHDASH
%token  <transToken>     ELSE
%token  <transToken>     EQUALS
%token  <transToken>     FALSE
%token  <transToken>     FROMCONSOLE
%token  <transIDToken>   ID
%token  <transToken>     IF
%token  <transToken>     INT
%token  <transIntToken>  INTLITERAL
%token  <transToken>     INTPTR
%token  <transToken>     GREATER
%token  <transToken>     GREATEREQ
%token  <transToken>     LBRACE
%token  <transToken>     LCURLY
%token  <transToken>     LESS
%token  <transToken>     LESSEQ
%token  <transToken>     LPAREN
%token  <transToken>     NOT
%token  <transToken>     NOTEQUALS
%token  <transToken>     NULLPTR
%token  <transToken>     OR
%token  <transToken>     RBRACE
%token  <transToken>     RCURLY
%token  <transToken>     RETURN
%token  <transToken>     RPAREN
%token  <transToken>     SEMICOLON
%token  <transToken>     SLASH
%token  <transToken>     STAR
%token  <transStrToken>  STRLITERAL
%token  <transToken>     TOCONSOLE
%token  <transToken>     TRUE
%token  <transToken>     VOID
%token  <transToken>     WHILE


/* Nonterminals
*  TODO: You will need to add more nonterminals
*  to this list as you add productions to the grammar
*  below (along with indicating the appropriate translation
*  attribute type). Note that the specifier in brackets
*  indicates the type of the translation attribute using
*  the names defined in the %union directive above
*/
/*    (attribute type)    (nonterminal)    */
/*
%type <transToken>
%type <transIDToken>
*/
%type <transDeclList>       globals
%type <transFormalDeclList> formals
%type <transFormalDeclList> formalsList
%type <transStmtList>       fnBody
%type <transStmtList>       stmtList
%type <transActualsList>    actualsList
/*
%type <transDivide>
%type <transEquals>
%type <transGreaterEq>
%type <transGreater>
%type <transLessEq>
%type <transLess>
%type <transMinus>
%type <transNotEquals>
%type <transOr>
%type <transPlus>
%type <transTimes>
%type <transDeref>
*/
%type <transID> id
/*
%type <transIndex>
%type <transRef>
%type <transNeg>
%type <transNot>
*/
%type <transFnDecl>     fnDecl
%type <transFormalDecl> formalDecl
%type <transVarDecl>    varDecl
%type <transAssignExp>  assignExp
/*
%type <transBinaryExp>
*/
%type <transCallExp> callExp
/*
%type <transCharLit>
%type <transFalse>
%type <transIntLit>
*/
%type <transLVal> lval
/*
%type <transNullPtr>
%type <transStrLit>
%type <transTrue>
%type <transUnaryExp>
%type <transAssignStmt>
%type <transCallStmt>
*/
%type <transDecl> decl
/*
%type <transFromConsoleStmt>
%type <transIfElseStmt>
%type <transIfStmt>
%type <transPostDecStmt>
%type <transPostIncStmt>
%type <transReturnStmt>
%type <transToConsoleStmt>
%type <transWhileStmt>
%type <transBoolType>
%type <transCharType>
%type <transIntType>
%type <transVoidType>
*/
%type <transExp> exp
%type <transExp> term
%type <transProgram> program
%type <transStmt> stmt
%type <transType> type

%right ASSIGN
%left OR
%left AND
%nonassoc LESS GREATER LESSEQ GREATEREQ EQUALS NOTEQUALS
%left DASH CROSS
%left STAR SLASH
%left NOT
%left AT CARAT

%%

program : globals
        {
            $$ = new ProgramNode($1);
            *root = $$;
        }

globals : globals decl
        {
            $$ = $1;
            DeclNode* aGlobalDecl = $2;
            $1->push_back(aGlobalDecl);
        }
        | /* epsilon */
        {
            std::list<DeclNode*>* startingGlobals;
            startingGlobals = new std::list<DeclNode*>();
            $$ = startingGlobals;
        }
        ;

decl : varDecl SEMICOLON
     {
         $$ = $1;
     }
     | fnDecl
     {
         $$ = $1;
     }

varDecl : type id
        {
            size_t typeLine = $1->line();
            size_t typeCol = $1->col();
            $$ = new VarDeclNode(typeLine, typeCol, $1, $2);
        }

type : INT
     {
         bool isPtr = false;
         $$ = new IntTypeNode($1->line(), $1->col(), isPtr);
     }
     | INTPTR
     {
         bool isPtr = true;
         $$ = new IntTypeNode($1->line(), $1->col(), isPtr);
     }
     | BOOL
     {
         bool isPtr = false;
         $$ = new BoolTypeNode($1->line(), $1->col(), isPtr);
     }
     | BOOLPTR
     {
         bool isPtr = true;
         $$ = new BoolTypeNode($1->line(), $1->col(), isPtr);
     }
     | CHAR
     {
         bool isPtr = false;
         $$ = new CharTypeNode($1->line(), $1->col(), isPtr);
     }
     | CHARPTR
     {
         bool isPtr = true;
         $$ = new CharTypeNode($1->line(), $1->col(), isPtr);
     }
     | VOID
     {
         $$ = new VoidTypeNode($1->line(), $1->col(), false);
     }

fnDecl : type id formals fnBody
       {
           $$ = new FnDeclNode($1, $2, $3, $4);
       }

formals : LPAREN RPAREN
        {
            $$ = new std::list<FormalDeclNode*>();
        }
        | LPAREN formalsList RPAREN
        {
            $$ = $2;
        }


formalsList : formalDecl
            {
                $$ = new std::list<FormalDeclNode*>();
                $$->push_back($1);
            }
            | formalDecl COMMA formalsList
            {
                $$ = new std::list<FormalDeclNode*>();
                $$->push_back($1);
                $$->insert($$->end(), $3->begin(), $3->end());
            }

formalDecl : type id
           {
               $$ = new FormalDeclNode($1, $2);
           }

fnBody : LCURLY stmtList RCURLY
       {
           $$ = $2;
       }

stmtList : /* epsilon */
         {
             $$ = new std::list<StmtNode*>();
         }
         | stmtList stmt
         {
             $1->insert($$->end(), $2);
             $$ = $1;
         }

stmt : varDecl SEMICOLON
     {
         $$ = $1;
     }
     | assignExp SEMICOLON
     {
         $$ = new AssignStmtNode($1->line(), $1->col(), $1);
     }
     | lval DASHDASH SEMICOLON
     {
         $$ = new PostDecStmtNode($1->line(), $1->col(), $1);
     }
     | lval CROSSCROSS SEMICOLON
     {
         $$ = new PostIncStmtNode($1->line(), $1->col(), $1);
     }
     | FROMCONSOLE lval SEMICOLON
     {
         $$ = new FromConsoleStmtNode($1->line(), $1->col(), $2);
     }
     | TOCONSOLE exp SEMICOLON
     {
         $$ = new ToConsoleStmtNode($1->line(), $1->col(), $2);
     }
     | IF LPAREN exp RPAREN LCURLY stmtList RCURLY
     {
        $$ = new IfStmtNode($1->line(), $1->col(), $3, $6); 
     }
     | IF LPAREN exp RPAREN LCURLY stmtList RCURLY ELSE LCURLY stmtList RCURLY
     {
        $$ = new IfStmtNode($1->line(), $1->col(), $3, $6, $10); 
     }
     | WHILE LPAREN exp RPAREN LCURLY stmtList RCURLY
     {
        $$ = new WhileStmtNode($1->line(), $1->col(), $3, $6); 
     }
     | RETURN exp SEMICOLON
     {
        $$ = new ReturnStmtNode($1->line(), $1->col(), $2);
     }
     | RETURN SEMICOLON
     {
        $$ = new ReturnStmtNode($1->line(), $1->col(), NULL); //unsure about sending NULL but what else is there to be sent?
     }
     | callExp SEMICOLON
     {
        $$ = new CallExpNode($1->line(), $1->col(), $1, NULL); //unsure about sending NULL but what else is there to be sent?
     }

exp : assignExp
    {
        $$ = $1;
    }
    | exp DASH exp
    {
        $$ = new MinusNode($1->line(), $1->col(), $1, $3);
    }
    | exp CROSS exp
    {
        $$ = new PlusNode($1->line(), $1->col(), $1, $3);
    }
    | exp STAR exp
    {
        $$ = new TimesNode($1->line(), $1->col(), $1, $3);
    }
    | exp SLASH exp
    {
        $$ = new DivideNode($1->line(), $1->col(), $1, $3);
    }
    | exp AND exp
    {
        $$ = new AndNode($1->line(), $1->col(), $1, $3);
    }
    | exp OR exp
    {
        $$ = new OrNode($1->line(), $1->col(), $1, $3);
    }
    | exp EQUALS exp
    {
        $$ = new EqualsNode($1->line(), $1->col(), $1, $3);
    }
    | exp NOTEQUALS exp
    {
        $$ = new NotEqualsNode($1->line(), $1->col(), $1, $3);
    }
    | exp GREATER exp
    {
        $$ = new GreaterNode($1->line(), $1->col(), $1, $3);
    }
    | exp GREATEREQ exp
    {
        $$ = new GreaterEqNode($1->line(), $1->col(), $1, $3);
    }
    | exp LESS exp
    {
        $$ = new LessNode($1->line(), $1->col(), $1, $3);
    }
    | exp LESSEQ exp
    {
        $$ = new LessEqNode($1->line(), $1->col(), $1, $3);
    }
    | NOT exp
    {
        $$ = new NotNode($1->line(), $1->col(), $2);
    }
    | DASH term
    {
        $$ = new NegNode($1->line(), $1->col(), $2);
    }
    | term
    {
        $$ = $1;
    }

assignExp : lval ASSIGN exp
          {
              $$ = new AssignExpNode($1->line(), $1->col(), $1, $3);
          }

callExp : id LPAREN RPAREN
        {
            $$ = new CallExpNode($1->line(), $1->col(), $1, nullptr);
        }
        | id LPAREN actualsList RPAREN
        {
            $$ = new CallExpNode($1->line(), $1->col(), $1, $3);
        }

actualsList : exp
            {
                $$ = new std::list<ExpNode*>();
                $$->push_back($1);
            }
            | actualsList COMMA exp
            {
                $1->insert($$->end(), $3);
                $$ = $1;
            }

term : lval
     {
         $$ = $1; //not sure if this works
     }
     | callExp
     {
        $$ = new CallExpNode($1->line(), $1->col());
     }
     | NULLPTR
     {
         $$ = new NullPtrNode($1->line(), $1->col());
     }
     | INTLITERAL
     {
         $$ = new IntLitNode($1->line(), $1->col(), $1->num());
     }
     | STRLITERAL
     {
         $$ = new StrLitNode($1->line(), $1->col(), $1->str());
     }
     | CHARLIT
     {
         $$ = new CharLitNode($1->line(), $1->col(), $1->val());
     }
     | TRUE
     {
         $$ = new TrueNode($1->line(), $1->col());
     }
     | FALSE
     {
         $$ = new FalseNode($1->line(), $1->col());
     }
     | LPAREN exp RPAREN
     {
        $$ = new ExpNode($1->line(), $1->col());
     }

lval : id
     {
       $$ = $1;
     }
     | id LBRACE exp RBRACE
     {
       $$ = new IndexNode($1, $3);
     }
     | AT id
     {
       $$ = new DerefNode($2, nullptr);
     }
     | CARAT id
     {
       $$ = new RefNode($2, nullptr);
     }

id : ID
   {
       $$ = new IDNode($1);
   }

%%

void holeyc::Parser::error(const std::string& msg){
  std::cout << msg << std::endl;
  std::cerr << "syntax error" << std::endl;
  std::cerr << "No AST built" << std::endl;
}
