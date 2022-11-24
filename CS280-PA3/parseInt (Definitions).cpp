/* Implementation of Recursive-Descent Parser
 * parseInt.cpp
 * Programming Assignment 3
 * Fall 2022
 */

#include "parseInt.h"

map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value>
    TempsResults; // Container of temporary locations of Value objects for
                  // results of expressions, variables values and constants
queue<Value> *ValQue; // declare a pointer variable to a queue of Value objects

namespace Parser {
bool pushed_back = false;
LexItem pushed_token;

static LexItem GetNextToken(istream &in, int &line) {
  if (pushed_back) {
    pushed_back = false;
    return pushed_token;
  }
  return getNextToken(in, line);
}

static void PushBackToken(LexItem &t) {
  if (pushed_back) {
    abort();
  }
  pushed_back = true;
  pushed_token = t;
}

} // namespace Parser

static int error_count = 0;

int ErrCount() { return error_count; }

void ParseError(int line, string msg) {
  ++error_count;
  cout << line << ": " << msg << endl;
}

bool isCorrectDataType(Token dataType, Value &val){
  switch(dataType){
    case INT:
    if(val.IsReal()){
      val = Value((int)(val.GetReal()));
    }
    return val.IsInt() || val.IsReal();

    case BOOL:
    return val.IsBool();

    case FLOAT:
    if(val.IsInt()){
      val = Value((float)(val.GetInt()));
    }
    return val.IsReal() || val.IsInt();

    default:
    return false;
  }
}

extern bool Prog(istream &in, int &line) {
  bool status;

  LexItem t = Parser::GetNextToken(in, line);
  if (t != PROGRAM) {
    ParseError(line, "Missing PROGRAM.");
    return false;
  }

  t = Parser::GetNextToken(in, line);
  if (t != IDENT) {
    ParseError(line, "Missing Program Name.");
    return false;
  }
  SymTable[t.GetLexeme()] = PROGRAM;
  

  status = StmtList(in, line);
  t = Parser::GetNextToken(in, line);
  if (t != PROGRAM || !status) {
    ParseError(line, "Incorrect Program Body.");
    return false;
  }
  return status;
}

extern bool StmtList(istream &in, int &line) {
  bool status;

  LexItem t = Parser::GetNextToken(in, line);
  if (t == END) {
    return true;
  }

  Parser::PushBackToken(t);
  status = Stmt(in, line);
  t = Parser::GetNextToken(in, line);
  // cout << t.GetToken() << ", " << t.GetLexeme() << ", " << t.GetLinenum() <<
  // endl;
  if (t == PROGRAM && !status) {
    ParseError(line, "Syntactic error in Program Body.");
    return false;
  } else {
    Parser::PushBackToken(t);
  }
  return status;
}

extern bool Stmt(istream &in, int &line) {
  bool status;
  LexItem t = Parser::GetNextToken(in, line);
  if (t == END) {
    return true;
  }

  if (t == PROGRAM) {
    Parser::PushBackToken(t);
    return false;
  }

  if (t == INT || t == FLOAT || t == BOOL) {
    Parser::PushBackToken(t);
    status = DeclStmt(in, line);
    if (!status) {
      ParseError(line, "Incorrect Declaration Statement.");
      return false;
    }
  } else if (t == IDENT || t == IF || t == PRINT) {
    Parser::PushBackToken(t);
    status = ControlStmt(in, line);
    if (!status) {
      ParseError(line, "Incorrect control Statement.");
      return false;
    }
  } else {
    Parser::PushBackToken(t);
    return true;
  }
  LexItem semiCol = Parser::GetNextToken(in, line);
  // cout << semiCol.GetLexeme() << ", " << semiCol.GetLinenum() << endl;
  if (semiCol != SEMICOL) {
    ParseError(line, "Missing semicolon at end of Statement.");
    return false;
  } else {
    status = Stmt(in, line);
  }

  return status;
}

extern bool DeclStmt(istream &in, int &line) {
  LexItem type = Parser::GetNextToken(in, line);
  return VarList(in, line, type);
}

extern bool VarList(istream &in, int &line, LexItem &type) {
  bool status = false;
  LexItem idtok = Parser::GetNextToken(in, line);
  status = Var(in, line, idtok);
  if (!status) {
    ParseError(line, "Incorrect variable in Declaration Statement.");
    return false;
  }
  SymTable[idtok.GetLexeme()] = type.GetToken();
  LexItem tok = Parser::GetNextToken(in, line);

  if (tok == COMMA) {
    status = VarList(in, line, type);
  } else if (tok == IDENT) {
    ParseError(line, "Missing comma in declaration statement.");
    return false;
  } else {
    Parser::PushBackToken(tok);
    return true;
  }
  return status;
}

extern bool Var(istream &in, int &line, LexItem &idtok) {
  bool status;
  if (idtok != IDENT) {
    ParseError(line, "Invalid Variable.");
    return false;
  }
  status = defVar.insert({idtok.GetLexeme(), true}).second;
  if (!status) {
    ParseError(line, "Variable Redefinition");
    return false;
  }
  return status;
}

// ControlStmt ::= AssigStmt | IfStmt | PrintStmt
extern bool ControlStmt(istream &in, int &line) {
  bool status;

  LexItem t = Parser::GetNextToken(in, line);
  // cout << t.GetLexeme() << ", " << t.GetLinenum() << endl;
  switch (t.GetToken()) {

  case PRINT:
    status = PrintStmt(in, line);
    break;

  case IF:
    status = IfStmt(in, line);
    break;

  case IDENT:
    Parser::PushBackToken(t);
    status = AssignStmt(in, line);
    break;

  default:
    Parser::PushBackToken(t);
    return false;
  }

  return status;
} // End of ControlStmt

extern bool AssignStmt(istream &in, int &line) {
  LexItem variable = Parser::GetNextToken(in, line);
  if (defVar.count(variable.GetLexeme()) != 1) {
    ParseError(line, "Undeclared Variable");
    return false;
  }
  LexItem t = Parser::GetNextToken(in, line);
  if (t != ASSOP) {
    ParseError(line, "Missing assignment operator");
    return false;
  }
  Value val;
  bool status = Expr(in, line, val);
  if(status){
    Token dataType = SymTable[variable.GetLexeme()];
    if(val.IsErr()){
      ParseError(line, "Missing Expression in Assignment Statement");
      return false;
    }
    if(isCorrectDataType(dataType, val)){
      TempsResults[variable.GetLexeme()] = val;
    }else{
      ParseError(line, "Illegal Assignment Operation");
      return false;
    }
  }
  return status;
}

extern bool IfStmt(istream &in, int &line) {
  bool status;
  LexItem t = Parser::GetNextToken(in, line);

  if (t != LPAREN) {
    ParseError(line, "Missing Left Parenthesis");
    return false;
  }

  Value val;
  status = Expr(in, line, val);
  if (!status) {
    ParseError(line, "Missing if statement Logic Expression");
    return false;
  }

  t = Parser::GetNextToken(in, line);
  if (t != RPAREN) {

    ParseError(line, "Missing Right Parenthesis");
    return false;
  }

  t = Parser::GetNextToken(in, line);
  if (t != THEN) {
    // ParseError(line, "Missing expression list after Then");
    return false;
  }

  if(val.IsBool()){
    if(val.GetBool()){
      status = StmtList(in, line);
      if (!status) {
        // ParseError(line, "If Statement Syntax Error");
        ParseError(line, "Missing Statement for If-Stmt Then-Part");
        return false;
      }
    }else{
      while(t != ELSE && t != END){
        t = Parser::GetNextToken(in, line);
      }
      Parser::PushBackToken(t);
    }
  }else{
    ParseError(line, "Missing Statement for If-Stmt If-Part");
    return false;
  }

  t = Parser::GetNextToken(in, line);
  if (t == ELSE) {
    if(val.GetBool()){
      while(t != END){
        t = Parser::GetNextToken(in, line);
      }
    }else{
      status = StmtList(in, line);
      if (!status) {
        ParseError(line, "Missing expression list after Else");
        return false;
      }
    }
  } else {
    Parser::PushBackToken(t);
  }
  
  t = Parser::GetNextToken(in, line);
  if(t == END){
    t = Parser::GetNextToken(in, line);
  }
  if (t != IF) {
    ParseError(line, "Missing closing keywords of IF statement.");
    return false;
  }
  return true;
}

// bool IdentList(istream& in, int& line);

// PrintStmt:= PRINT (ExpreList)
bool PrintStmt(istream &in, int &line) {
  LexItem t;
  // cout << "in PrintStmt" << endl;
  ValQue = new queue<Value>;
  t = Parser::GetNextToken(in, line);
  if (t != LPAREN) {

    ParseError(line, "Missing Left Parenthesis");
    return false;
  }

  bool ex = ExprList(in, line);

  if (!ex) {
    ParseError(line, "Missing expression list after Print");
    while (!(*ValQue).empty()) {
      ValQue->pop();
    }
    delete ValQue;
    return false;
  }

  // Evaluate: print out the list of expressions' values
  while (!(*ValQue).empty()) {
    Value nextVal = (*ValQue).front();
    if(nextVal.IsErr()){
      ParseError(line, "Undefined Variable");
      return false;
    }
    cout << nextVal;
    ValQue->pop();
  }
  cout << endl;

  t = Parser::GetNextToken(in, line);
  if (t != RPAREN) {

    ParseError(line, "Missing Right Parenthesis");
    return false;
  }

  return true;
} // End of PrintStmt

// ExprList:= Expr {,Expr}
extern bool ExprList(istream &in, int &line) {
  bool status = false;
  Value val;
  status = Expr(in, line, val);
  if (!status) {
    ParseError(line, "Missing Expression");
    return false;
  }
  ValQue -> push(val);
  LexItem tok = Parser::GetNextToken(in, line);

  if (tok == COMMA) {
    status = ExprList(in, line);
  } else if (tok.GetToken() == ERR) {
    ParseError(line, "Unrecognized Input Pattern");
    cout << "(" << tok.GetLexeme() << ")" << endl;
    return false;
  } else {
    Parser::PushBackToken(tok);
    return true;
  }
  return status;
} // End of ExprList

extern bool Expr(istream &in, int &line, Value &retVal) {
  LexItem tok;
  Value val1, val2;
  bool t1 = LogANDExpr(in, line, val1);
  if (!t1) {
    return false;
  }
  
  retVal = val1;
  tok = Parser::GetNextToken(in, line);
  if (tok.GetToken() == ERR) {
    ParseError(line, "Unrecognized Input Pattern");
    return false;
  }
  
  while (tok == OR) {
    t1 = LogANDExpr(in, line, val2);
    if (!t1) {
      ParseError(line, "Missing operand after operator");
      return false;
    }

    retVal = retVal || val2;
    if (retVal.IsErr()) {
      ParseError(line, "Illegal OR operation.");
      return false;
    }
    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
      ParseError(line, "Unrecognized Input Pattern");
      return false;
    }
  }
  Parser::PushBackToken(tok);
  return true;
}

// LogANDExpr ::= EqualExpr { && EqualExpr }
bool LogANDExpr(istream &in, int &line, Value &retVal) {
  LexItem tok;
  Value val1, val2;
  bool t1 = EqualExpr(in, line, val1);
  // cout << "status of EqualExpr and val1: " << t1<< " " << val1.IsErr() << " "
  // << val1.IsBool() << endl; cout << "status of EqualExpr: " << t1<< endl;

  if (!t1) {
    return false;
  }
  // cout << "value of var1 in AND op " << val1.GetBool() << endl;
  retVal = val1;
  tok = Parser::GetNextToken(in, line);
  if (tok.GetToken() == ERR) {
    ParseError(line, "Unrecognized Input Pattern");
    //cout << "(" << tok.GetLexeme() << ")" << endl;
    return false;
  }
  while (tok == AND) {
    t1 = EqualExpr(in, line, val2);
    // cout << "value of var1 in AND op " << val1.GetBool() << endl;
    if (!t1) {
      ParseError(line, "Missing operand after operator");
      return false;
    }
    // evaluate expression for Logical AND
    retVal = retVal && val2;
    // cout << "AND op result " << retVal.IsBool() << " " << retVal.GetBool() <<
    // endl;
    if (retVal.IsErr()) {
      ParseError(line, "Illegal AND operation.");
      // cout << "(" << tok.GetLexeme() << ")" << endl;
      return false;
    }
    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
      ParseError(line, "Unrecognized Input Pattern");
      //cout << "(" << tok.GetLexeme() << ")" << endl;
      return false;
    }
  }
  Parser::PushBackToken(tok);
  return true;
} // End of LogANDExpr

bool EqualExpr(istream &in, int &line, Value &retVal, bool equalFlag) {
  bool status = RelExpr(in, line, retVal);
  if(!status){
    ParseError(line, "Missing operand after operator");
    return false;
  }
  
  LexItem t = Parser::GetNextToken(in, line);
  if (t == EQUAL && equalFlag) {
    ParseError(line, "Illegal Equality Expression.");
    return false;
  }
  Parser::PushBackToken(t);
  return true;
}

extern bool EqualExpr(istream &in, int &line, Value &retVal) {
  LexItem tok;
  Value val1, val2;
  bool t1 = RelExpr(in, line, val1);
  if (!t1) {
    return false;
  }
  retVal = val1;
  tok = Parser::GetNextToken(in, line);
  
  if (tok.GetToken() == ERR) {
    ParseError(line, "Unrecognized Input Pattern");
    return false;
  }
  
  if (tok == EQUAL) {
    t1 = EqualExpr(in, line, val2, true);
    if (!t1) {
      ParseError(line, "Missing operand after operator");
      return false;
    }
    
    retVal = retVal == val2;
    if (retVal.IsErr()) {
      ParseError(line, "Illegal EQUAL operation.");
      // cout << "(" << tok.GetLexeme() << ")" << endl;
      return false;
    }
    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
      ParseError(line, "Unrecognized Input Pattern");
      //cout << "(" << tok.GetLexeme() << ")" << endl;
      return false;
    }
  }
  Parser::PushBackToken(tok);
  return true;
}

bool RelExpr(istream &in, int &line, Value &retVal, bool equalFlag) {
  bool status = AddExpr(in, line, retVal);
  if(!status){
    ParseError(line, "Missing operand after operator");
    return false;
  }
  
  LexItem t = Parser::GetNextToken(in, line);
  if ((t == GTHAN || t == LTHAN) && equalFlag) {
    ParseError(line, "Illegal Relational Expression.");
    return false;
  }
  Parser::PushBackToken(t);
  return true;
}

extern bool RelExpr(istream &in, int &line, Value &retVal) {
  LexItem tok;
  Value val1, val2;
  bool t1 = AddExpr(in, line, val1);
  if (!t1) {
    return false;
  }
  retVal = val1;
  tok = Parser::GetNextToken(in, line);
  
  if (tok.GetToken() == ERR) {
    ParseError(line, "Unrecognized Input Pattern");
    return false;
  }
  
  if (tok == GTHAN || tok == LTHAN) {
    t1 = RelExpr(in, line, val2, true);
    if (!t1) {
      ParseError(line, "Missing operand after operator");
      return false;
    }
    retVal = (tok == GTHAN)? retVal > val2 : retVal < val2;

    if (retVal.IsErr()) {
      ParseError(line, (tok == GTHAN) ? "Illegal Greater Than operation." : "Illegal Less Than operation");
      return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
      ParseError(line, "Unrecognized Input Pattern");
      return false;
    }
  }
  Parser::PushBackToken(tok);
  return true;
}

extern bool AddExpr(istream &in, int &line, Value &retVal) {
  LexItem tok;
  Value val1, val2;
  bool t1 = MultExpr(in, line, val1);
  if (!t1) {
    return false;
  }
  
  retVal = val1;
  tok = Parser::GetNextToken(in, line);
  if (tok.GetToken() == ERR) {
    ParseError(line, "Unrecognized Input Pattern");
    return false;
  }
  
  while (tok == PLUS || tok == MINUS) {
    t1 = MultExpr(in, line, val2);
    if (!t1) {
      ParseError(line, "Missing operand after operator");
      return false;
    }

    retVal = (tok == PLUS) ? retVal + val2: retVal - val2;
    if (retVal.IsErr()) {
      ParseError(line, (tok == PLUS) ? "Illegal ADD operation." : "Illegal MINUS operation");
      return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
      ParseError(line, "Unrecognized Input Pattern");
      return false;
    }
  }
  Parser::PushBackToken(tok);
  return true;
}

extern bool MultExpr(istream &in, int &line, Value &retVal) {
  LexItem tok;
  Value val1, val2;
  bool t1 = UnaryExpr(in, line, val1);
  if (!t1) {
    return false;
  }
  
  retVal = val1;
  tok = Parser::GetNextToken(in, line);
  if (tok.GetToken() == ERR) {
    ParseError(line, "Unrecognized Input Pattern");
    return false;
  }
  while (tok == MULT || tok == DIV) {
    t1 = UnaryExpr(in, line, val2);
    if (!t1) {
      ParseError(line, "Missing operand after operator");
      return false;
    }
    if(tok == DIV && (val2.GetInt() == 0 || val2.GetReal() == 0)){
      ParseError(line, "Run-Time Error-Illegal Division by Zero");
      return false;
    }
    retVal = (tok == MULT) ? retVal * val2: retVal / val2;
    if (retVal.IsErr()) {
      ParseError(line, (tok == MULT) ? "Illegal MULT operation." : "Illegal DIV operation");
      return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
      ParseError(line, "Unrecognized Input Pattern");
      return false;
    }
  }
  Parser::PushBackToken(tok);
  return true;
}

extern bool UnaryExpr(istream &in, int &line, Value &retVal) {
  LexItem t = Parser::GetNextToken(in, line);
  int sign = 0;
  switch (t.GetToken()) {
  case MINUS:
    sign = 1;
    break;

  case PLUS:
    sign = 2;
    break;

  case NOT:
    sign = 3;
    break;

  default:
    Parser::PushBackToken(t);
    break;
  }
  return PrimaryExpr(in, line, sign, retVal);
}

extern bool PrimaryExpr(istream &in, int &line, int sign, Value &retVal) {
  LexItem valTok = Parser::GetNextToken(in, line);
  switch(valTok.GetToken()){
    case BCONST:
    retVal = (valTok.GetLexeme() == "TRUE") ? Value(true) : Value(false);
    break;

    case SCONST:
    retVal = Value(valTok.GetLexeme());
    break;

    case ICONST:
    retVal = Value(stoi(valTok.GetLexeme()));
    break;

    case RCONST:
    retVal = Value(stof(valTok.GetLexeme()));
    break;

    case IDENT:
    if(SymTable[valTok.GetLexeme()] == PROGRAM){
      ParseError(line, "Illegal use of program name as a variable");
      return false;
    }
    retVal = Value(TempsResults[valTok.GetLexeme()]);
    break;

    default:
    break;
  }
  
  bool ex;
  if (valTok == LPAREN) {
    Value val1;
    ex = Expr(in, line, val1);
    if (!ex) {
      ParseError(line, "Missing expression list after Print");
      return false;
    }
    valTok = Parser::GetNextToken(in, line);
    if (valTok != RPAREN) {

      ParseError(line, "Missing Right Parenthesis");
      return false;
    }
    retVal = val1;
  }
    
  switch(sign){
    case 1:
    if(retVal.IsInt()){
      retVal = Value(-retVal.GetInt());
      return true;
    }else if(retVal.IsReal()){
      retVal = Value(-retVal.GetReal());
      return true;
    }else{
      ParseError(line, "Illegal Operand Type for Sign Operator");
      return false;
    }

    case 2:
    if(!(retVal.IsInt() || retVal.IsReal())){
      ParseError(line, "Illegal Operand Type for Sign Operator");
      return false;
    }
    return true;

    case 3:
    if(retVal.IsBool()){
      retVal = Value(!retVal);
      return true;
    }
    return false;

    default:
    break;
  }
  return true;
}
