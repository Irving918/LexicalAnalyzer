#include "parserInterp.h"

map<string, bool> defVar;
map<string, Token> SymTable;
map <string, int> charLength;
map <string, int> stringLengthTable;
int ifStatementCount = 0;
int ifStatementTrue = false;

map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

//Prog ::= PROGRAM IDENT {Decl}{Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line){

	LexItem t;
	bool stmt = true;
	bool decl = true;

	t = Parser::GetNextToken(in,line);

	if(t != PROGRAM){
		ParseError(line,"Missing PROGRAM.");
		return false;
	}

	t = Parser::GetNextToken(in,line);

	if(t != IDENT){
		ParseError(line,"Missing IDENT.");
		return false;
	}

	t = Parser::GetNextToken(in,line);

	while((t == INTEGER || t == REAL || t == CHARACTER) && decl){
		
		Parser::PushBackToken(t);

		decl = Decl(in,line);

		t = Parser::GetNextToken(in,line);
		
	}
	
	Parser::PushBackToken(t);
	

	t = Parser::GetNextToken(in,line);

	while((t == IDENT || t == IF || t == PRINT) && stmt){
		
		Parser::PushBackToken(t);

		stmt = Stmt(in,line);

		t = Parser::GetNextToken(in,line);	
	}
	
	Parser::PushBackToken(t);
	

	t = Parser::GetNextToken(in,line);

	if(t != END){
		ParseError(line,"Missing END.");
		return false;
	}

	
	t = Parser::GetNextToken(in,line);

	if(t != PROGRAM){
		ParseError(line,"Missing PROGRAM.");
		return false;
	}

	t = Parser::GetNextToken(in,line);

	if(t != IDENT){
		ParseError(line,"Missing IDENT.");
		return false;
	}

	cout<<"(DONE)"<<endl;

	return true;
}

//Decl ::= Type :: Varlist
bool Decl(istream& in, int& line){
	LexItem tok;
	LexItem typeTok;
	bool status = false;
	int strlen = 0;

	tok = Parser::GetNextToken(in,line);
	if(tok == CHARACTER || tok == INTEGER || tok == REAL){
		typeTok = tok;
	}

	Parser::PushBackToken(tok);


	status = Type(in,line);
	if(!status){
		ParseError(line,"Missing type.");
		return false;
	}

	tok = Parser::GetNextToken(in,line);

	if(tok != DCOLON){
		//cout<<tok.GetLexeme()<<endl;
		ParseError(line,"Missing DCOLON.");
		return false;
	}

	auto it = charLength.find("CHARACTER");
	if(it != charLength.end()){
		strlen = charLength["CHARACTER"];
	}



	status = VarList(in,line,typeTok,strlen);

	if(!status){
		ParseError(line,"Missing VarList.");
		return false;
	}

	/*
	auto it = stringLengthTable.find(varName);
	if(it != stringLengthTable.end()){
		cout<<"Definition of Strings with length of "<< stringLengthTable[varName]<<" in declaration statement."<<endl;
	}
	*/

	return status;
}

//Type ::= INTEGER | REAL | CHARACTER [ (LEN = ICONST) ]
bool Type(istream& in, int& line){
	LexItem t;
	bool isString = false;
	string actualString = "";
	string stringLength = "";

	t = Parser::GetNextToken(in,line);

	if(t != INTEGER && t != REAL && t != CHARACTER){
		//cout<<t.GetLexeme()<<endl;
		ParseError(line,"Missing integer,real or character.");
		return false;
	}

	if(t == CHARACTER){
		actualString = t.GetLexeme();
		isString = true;
	}

	t = Parser::GetNextToken(in,line);

	if(t == LPAREN && isString){
		t = Parser::GetNextToken(in,line);

		if(t != LEN){
			ParseError(line,"Missing LEN.");
			return false;
		}

		t = Parser::GetNextToken(in,line);

		if(t != ASSOP){
			ParseError(line,"Missing ASSOP.");
			return false;
		}

		t = Parser::GetNextToken(in,line);

		if(t != ICONST){
			ParseError(line,"Missing ICONST.");
			return false;
		}

		stringLength = t.GetLexeme();

		t = Parser::GetNextToken(in,line);

		if(t != RPAREN){
			ParseError(line,"Missing RPAREN");
			return false;
		}

		if(isString){
			//actualString is either character lowercase or uppercase 
			charLength["CHARACTER"] = stoi(stringLength);
		}

	}
	else{
		if(isString){
			charLength["CHARACTER"] = 1;
		}

		Parser::PushBackToken(t); 
	}

	return true;
}

//VarList ::= Var[=Expr] {, Var [= Expr]}
bool VarList(istream& in, int& line, LexItem & idtok, int strlen){
	LexItem tok;
	bool status = false;
	string varName = "";
	Value retVal;

	tok = Parser::GetNextToken(in,line);

	if(tok == IDENT){
		varName = tok.GetLexeme();
	}

	Parser::PushBackToken(tok);


	status = Var(in,line,tok);
	if(!status){
		ParseError(line,"Missing VAR.");
		return false;
	}

	//Insert the variable inside SymTable with its associated token
	auto it = SymTable.find(varName);
	if(it == SymTable.end()){
		SymTable[varName] = idtok.GetToken(); 
	}

	//if variable is a STRTING, insert the correct LENGTH
	if(idtok.GetToken() == CHARACTER){
		auto it2 = stringLengthTable.find(varName);
		if(it2 == stringLengthTable.end()){
			//does not find it
			stringLengthTable[varName] = strlen;
		}
	}


	tok = Parser::GetNextToken(in,line);


	if(tok == ASSOP){

		status = Expr(in,line,retVal);

		if(SymTable[varName] == REAL){
			retVal = (double)retVal.GetInt();
		}

		if(idtok.GetToken() == CHARACTER && strlen == 1){
			string output = retVal.GetString();
			
			if(output.length() > strlen){
				output = output.substr(0,strlen);
			}

			retVal = output;
		}
		else if(idtok.GetToken() == CHARACTER && strlen > 1){
			string output = retVal.GetString();


			if(output.length() < strlen){
				while(output.length() < strlen){
					output+=' ';
				}
			}

			retVal = output;
		}
		//insert the variable name as well as its associated value in the map
		TempsResults[varName] = retVal;


	}
	else{
		if(idtok.GetToken() == CHARACTER){
			string output = "";
			int maxLength = stringLengthTable[varName];
			while(output.length() < maxLength){
				output += ' ';
			}

			TempsResults[varName] = output;
		}
		Parser::PushBackToken(tok);
		defVar[varName] = false;
	}

	tok = Parser::GetNextToken(in,line);

	if(tok == COMMA){
		status = VarList(in,line,idtok,strlen);
	}
	else{
		Parser::PushBackToken(tok);
	}

	return status;
}

//Stmt ::= AssignStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
bool Stmt(istream& in, int& line){
	
	bool status = false;
	LexItem tok = Parser::GetNextToken(in,line);
	
	switch(tok.GetToken()){

		case IDENT:
			Parser::PushBackToken(tok);
			status = AssignStmt(in,line);
			break;
		case IF:
			Parser::PushBackToken(tok);
			status = SimpleIfStmt(in,line);
			break;
		case PRINT:
			status = PrintStmt(in,line);
			break;
		case ERR:
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		default:
			ParseError(line,"Missing statement");
			return false;

	}

	return status;
}


bool IdentList(istream& in, int& line);

//PrintStmt:= PRINT *, ExprList 
bool PrintStmt(istream& in, int& line) {
	LexItem t;
	ValQue = new queue<Value>;
	
	
	t = Parser::GetNextToken(in, line);
	
 	if( t != DEF ) {
		
		ParseError(line, "Print statement syntax error.");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	
	if( t != COMMA ) {
		
		ParseError(line, "Missing Comma.");
		return false;
	}
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}
	
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;
	return ex;
}//End of PrintStmt

//BlockIfStmt ::= IF (RelExpr) THEN {Stmt} [ELSE {STMT}] END IF
bool BlockIfStmt(istream& in, int& line){
	LexItem t;
	//bool status = false;

	//int currentIfBlock = ifStatementCount;
	if(ifStatementTrue){
		t = Parser::GetNextToken(in,line);
		
		while(t == IDENT || t == IF || t == PRINT){
		
			Parser::PushBackToken(t);

			Stmt(in,line);

			t = Parser::GetNextToken(in,line);	
		}
	
		Parser::PushBackToken(t); 	
	}
	else if(!ifStatementTrue){
		t = Parser::GetNextToken(in,line);
		
		while(t != ELSE && t != END){

			t = Parser::GetNextToken(in,line);	
		}
	
		Parser::PushBackToken(t); 
		
	}

	t = Parser::GetNextToken(in,line);

	if(t == ELSE){
		
		t = Parser::GetNextToken(in,line);
		
		while(t == IDENT || t == IF || t == PRINT){
		
			Parser::PushBackToken(t);

			Stmt(in,line);

			t = Parser::GetNextToken(in,line);	
		}
	
		Parser::PushBackToken(t);
		
	}
	else{
		Parser::PushBackToken(t);
	}

	t = Parser::GetNextToken(in,line);

	if(t != END){
		ParseError(line,"Missing END.");
		return false;
	}

	t = Parser::GetNextToken(in,line);

	if(t != IF){
		ParseError(line,"Missing IF.");
		return false;
	}

	//cout<<"End of Block If statement at nesting level "<<currentIfBlock<<endl;

	return true;


}

//SimpleIfStmt ::= IF (RelExpr) SimpleStmt
extern bool SimpleIfStmt(istream& in, int& line){
	LexItem t;
	bool status = false;
	Value retVal;

	t = Parser::GetNextToken(in,line);

	if(t != IF){
		ParseError(line,"Missing IF statement.");
		return false;
	}

	ifStatementCount++;



	t = Parser::GetNextToken(in,line);

	if(t != LPAREN){
		ParseError(line,"Missing LPAREN.");
		return false;
	}

	status = RelExpr(in,line,retVal);

	if(!status){
		ParseError(line,"Missing RelExpr");
		return false;
	}

	if(retVal.GetType() != VBOOL){
		ParseError(line,"Run-Time Error Illegal Type for If statement condition");
		return false;
	}

	if(retVal.GetBool() == true){
		ifStatementTrue = true;
	}

	t = Parser::GetNextToken(in,line);

	if(t != RPAREN){
		ParseError(line,"Missing RPAREN");
		return false;
	}

	t = Parser::GetNextToken(in,line);

	if(t == THEN){
		status = BlockIfStmt(in,line);
	}
	else{
		Parser::PushBackToken(t);

		status = SimpleStmt(in,line);

		if(!status){
			ParseError(line,"Missing SimpleStmt.");
			return false;
		}

		cout<<"Print statement in a Simple If statement."<<endl;
	}

	return status;

}

//SimpleStmt ::= AssignStmt | PrintStmt
bool SimpleStmt(istream& in, int& line){
	LexItem tok;
	bool status = false;

	tok = Parser::GetNextToken(in,line);

	switch(tok.GetToken()){

		case IDENT:
			Parser::PushBackToken(tok);
			status = AssignStmt(in,line);
			break;
		case PRINT:
			status = PrintStmt(in,line);
			break;
		default:
			ParseError(line,"Missing assign or print stmt.");
			return false;
		
	}

	return status;

}

//AssignStmt ::= Var = Expr
bool AssignStmt(istream& in, int& line){
	LexItem t;
	bool status = false;
	Value retVal;
	string varName;


	t = Parser::GetNextToken(in,line);
	
	if(t == IDENT){
		varName = t.GetLexeme();
	}

	Parser::PushBackToken(t);

	status = Var(in,line,t);

	if(!status){
		ParseError(line,"Missing VAR.");
		return false;
	}

	t = Parser::GetNextToken(in,line);

	if (t != ASSOP){
		ParseError(line,"Missing ASSOP.");
		return false;
	}

	t = Parser::GetNextToken(in,line);
	auto it = TempsResults.find(t.GetLexeme());
	if(it != TempsResults.end()){
		Value test = TempsResults[t.GetLexeme()];

		auto iter = SymTable.find(varName);
		if(iter != SymTable.end()){
			if(SymTable[varName] == CHARACTER){
				if(test.GetType() != VSTRING){
					ParseError(line,"Illegal mixed-mode assignment operation");
					return false;
				}
			}

		}
	}


	Parser::PushBackToken(t);


	status = Expr(in,line,retVal);

	if(!status){
		ParseError(line,"Missing Expr");
		return false;
	}



	if(retVal.GetType() == VSTRING){
	
		auto it = stringLengthTable.find(varName);
		if(it != stringLengthTable.end()){
			string output = retVal.GetString();
			int maxLength = stringLengthTable[varName];

			if(output.length() > maxLength){
				output = output.substr(0,maxLength);
			}
			else if(output.length() < maxLength){
				
				while(output.length() < maxLength){
					output += ' ';
				}
			}

			retVal = output;
					
		}
	}

	TempsResults[varName] = retVal;

	return status;
}



//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value retVal;
	
	status = Expr(in, line, retVal);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(retVal);
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		
		status = ExprList(in, line);
		
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of ExprList

//RelExpr ::= Expr [ ( == | < | > ) Expr ]
bool RelExpr(istream& in, int& line, Value & retVal){
	bool status = false;
	LexItem tok;
	Value oper;

	status = Expr(in,line,retVal);
	if(!status){
		ParseError(line,"Missing Expr.");
		return false;
	}


	tok = Parser::GetNextToken(in,line);

	if(tok == EQ || tok == LTHAN || tok == GTHAN){

		oper = retVal;

		if(tok == EQ){
			status = Expr(in,line,retVal);
			retVal = oper.operator==(retVal);
		}
		else if(tok == LTHAN){
			status = Expr(in,line,retVal);
			retVal = oper.operator<(retVal);
		}
		else{
			status = Expr(in,line,retVal);
			retVal = oper.operator>(retVal);
		}

	}
	else{
		Parser::PushBackToken(tok);
	}

	return status;

}

//Expr ::= MultExpr { (+ | - | // ) MultExpr }
bool Expr(istream& in, int& line, Value & retVal){
	LexItem tok;
	bool status = false;
	Value oper;

	/*
	tok = Parser::GetNextToken(in,line);

	if(tok == IDENT){
		auto it = defVar.find(tok.GetLexeme());

		if(it == defVar.end()){
			return false;
		}	
	}

	Parser::PushBackToken(tok);
	*/


	status = MultExpr(in,line,retVal);
	if(!status){
		ParseError(line,"Missing MultExpr");
		return false;
	}


	tok = Parser::GetNextToken(in,line);

	if(tok == PLUS || tok == MINUS || tok == CAT){

		oper = retVal;
		if(tok == PLUS){

			tok = Parser::GetNextToken(in,line);
			if(oper.GetType() == VREAL){
				if(tok != REAL || tok != INTEGER){
					ParseError(line,"Illegal operand type for the operation");
					return false;
				}
			}

			Parser::PushBackToken(tok);


			status = Expr(in,line,retVal);

			retVal = oper.operator+(retVal);


		}
		else if(tok == MINUS){
			status = Expr(in,line,retVal);
			
			retVal = oper.operator-(retVal);
		}
		else{
			status = Expr(in,line,retVal);

			retVal = oper.Catenate(retVal);
		}

	}
	else{
		Parser::PushBackToken(tok);
	}

	return status;
}

//MultExpr ::= TermExpr { ( * | / ) TermExpr }
bool MultExpr(istream& in, int& line, Value & retVal){
	bool status = false;
	LexItem tok;
	Value oper;

	status = TermExpr(in,line,retVal);
	if(!status){
		ParseError(line,"Missing TermExpr");
		return false;
	}

	tok = Parser::GetNextToken(in,line);
	
	if(tok == MULT || tok == DIV){

		oper = retVal;

		if(tok == MULT){
			status = MultExpr(in,line,retVal);

			retVal = oper.operator*(retVal);
		}
		else{
			tok = Parser::GetNextToken(in,line);
			Value test = TempsResults[tok.GetLexeme()];

			if(test.GetReal() == 0.0){
				ParseError(line,"Run-Time-Error-Illegal divison by Zero");
				return false;
			}
			Parser::PushBackToken(tok);

			status = MultExpr(in,line,retVal);

			retVal = oper.operator/(retVal);
		}

	}
	else{
		Parser::PushBackToken(tok);
	}

	return status;
}

//TermExpr ::= SFactor { ** SFactor }
bool TermExpr(istream& in, int& line, Value & retVal){
	bool status = false;
	LexItem tok;
	Value oper;

	status = SFactor(in,line,retVal);
	if(!status){
		ParseError(line,"Missing SFactor");
		return false;
	}

	
	tok = Parser::GetNextToken(in,line);
	
	if(tok == POW){

		oper = retVal;
	
		status = TermExpr(in,line,retVal);

		retVal = oper.Power(retVal);

	}
	else{
		Parser::PushBackToken(tok);
	}

	return status;
	
}

//SFactor ::= [+ | -] Factor 
bool SFactor(istream& in, int& line, Value & retVal){
	LexItem t;
	bool status = false;
	int sign  = 0; //0 denotes positive sign, 1 denotes negative sign


	t = Parser::GetNextToken(in,line);

	if(t == PLUS || t == MINUS){
		if(t == PLUS){
			sign = 0;
		}
		else{
			sign = 1;
		}
		
	}
	else{
		Parser::PushBackToken(t);
	}


	

	status = Factor(in,line,sign,retVal);

	if(sign == 1 && retVal.GetType() == VSTRING){
		ParseError(line,"Illegal Operand Type for Sign Operator");
		return false;
	}

	if(!status){
		ParseError(line,"Missing Factor");
		return false;
	}

	return status;

}


//Var ::= IDENT
bool Var(istream& in, int& line, LexItem & idtok){
	LexItem t;
	string varName = "";

	t = Parser::GetNextToken(in,line);

	if(t != IDENT){
		ParseError(line,"Missing IDENT.");
		return false;
	}

	varName = t.GetLexeme();

	return true;

}

//Factor ::= IDENT | ICONST | RCONST | SCONST | (Expr)
bool Factor(istream& in, int& line, int sign, Value & retVal){
	LexItem tok;
	bool status = false;

	tok = Parser::GetNextToken(in,line);
	
	if(tok != IDENT && tok != ICONST && tok != RCONST && tok != SCONST && tok != LPAREN){
		ParseError(line,"Missing IDENT | ICONST | RCONST | SCONST | LPAREN");
		return false;
	}

	if(tok == LPAREN){

		status = Expr(in,line,retVal);

		if(!status){
			ParseError(line,"Missing Expr");
			return false;
		}

		tok = Parser::GetNextToken(in,line);

		if(tok != RPAREN){
			ParseError(line,"Missing RPAREN");
			return false;
		}
	}
	else{
		if(tok == IDENT){

			auto it = TempsResults.find(tok.GetLexeme());

			if(it == TempsResults.end()){
				ParseError(line,"Illegal operand type for the operation");
				return false;
		
			}
			else{
				retVal = TempsResults[tok.GetLexeme()];

				if(retVal.GetType() == VINT && sign == 1){
					retVal = retVal.operator*(-1);
				}
				else if(retVal.GetType() == VSTRING){
					string output = retVal.GetString();
					int maxLength = charLength["CHARACTER"];
					
					if(output.length() > maxLength){
						output = output.substr(0,maxLength);
					}

					retVal = output;
				}
			}

		}
		else if(tok == SCONST || tok == ICONST || tok == RCONST){
			if(tok == ICONST){
				
				retVal = stoi(tok.GetLexeme());
				
			}
			else if(tok == RCONST){
				retVal = stod(tok.GetLexeme());
			}
			else{
				retVal = tok.GetLexeme();
			}
		}
	}


	return true;
}



