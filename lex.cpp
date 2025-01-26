#include "lex.h"
#include <iostream>

using namespace std;

map <string, Token> StrToTok = {
		{"PROGRAM", PROGRAM },
		{"PRINT", PRINT},
		{"IF",  IF},
		{"ELSE" ,  ELSE},	
		{ "IDENT", IDENT },
		{ "END", END },
		{ "INTEGER", INTEGER },
		{ "REAL", REAL },
		{ "CHARACTER", CHARACTER },
		{ "ICONST", ICONST },
		{ "RCONST", RCONST },
		{ "SCONST", SCONST },
		{ "THEN", THEN },
		
			
		{"PLUS",  PLUS },
		{ "MINUS", MINUS },
		{ "MULT", MULT },
		{ "DIV", DIV },
		{ "POW", POW },
		{ "ASSOP", ASSOP },
		{ "EQ", EQ },
		{ "GTHAN", GTHAN },
		{ "LTHAN", LTHAN },	
		            
		{ "COMMA", COMMA },
		{ "LPAREN", LPAREN },
		{ "RPAREN", RPAREN },
		{ "DOT", DOT },
		{ "DCOLON", DCOLON },
		{ "CAT", CAT },
		{ "DEF", DEF },
		{"ERR",  ERR },

		{ "DONE", DONE,  },
};

ostream& operator<<(ostream& out, const LexItem& tok){
    Token t = tok.GetToken();
    
    if(t == IDENT){
        out<<"IDENT"<<": "<<"\'"<<tok.GetLexeme()<<"\'";
    }
    else if(t == IF || t == ELSE || t == PRINT || t == INTEGER || t == REAL || t == CHARACTER || t == END || t == THEN || t == PROGRAM || t == LEN){
		string nameOfTok;
		if(tok.GetToken() == IF){
			nameOfTok = "IF";
		}
		else if(tok.GetToken() == ELSE){
			nameOfTok = "ELSE";
		}
		else if(tok.GetToken() == PRINT){
			nameOfTok = "PRINT";
		}
		else if(tok.GetToken() == INTEGER){
			nameOfTok = "INTEGER";
		}
		else if(tok.GetToken() == REAL){
			nameOfTok = "REAL";
		}
		else if(tok.GetToken() == CHARACTER){
			nameOfTok = "CHARACTER";
		}
		else if(tok.GetToken() == END){
			nameOfTok = "END";
		}
		else if(tok.GetToken() == THEN){
			nameOfTok = "THEN";
		}
		else if(tok.GetToken() == PROGRAM){
			nameOfTok = "PROGRAM";
		}
		else{
			nameOfTok = "LEN";
		}
	
		out<<nameOfTok;
	}
    else if(t == ICONST || t == RCONST || t == BCONST){
		string nameOfTok;
		if(t == ICONST){
			nameOfTok = "ICONST";
		}
		else if(t == RCONST){
			nameOfTok = "RCONST";
		}
        else{
            nameOfTok = "BCONST";
        }
		out<<nameOfTok<<": ("<<tok.GetLexeme()<<")";
	}
    else if(t == SCONST){
		out<<"SCONST: "<<"\""<<tok.GetLexeme()<<"\"";
	}
    else if(t == PLUS || t == MINUS || t == MULT || t == DIV || t == ASSOP || t == EQ || t == POW || t == GTHAN || t == LTHAN || t == CAT){
		string nameOfTok;
		if(t == PLUS){
			nameOfTok = "PLUS";
		}
		else if(t == MINUS){
			nameOfTok = "MINUS";
		}
		else if(t == MULT){
			nameOfTok = "MULT";
		}
		else if(t == DIV){
			nameOfTok = "DIV";
		}
		else if(t == ASSOP){
			nameOfTok = "ASSOP";
		}
		else if(t == EQ){
			nameOfTok = "EQ";
		}
		else if(t == POW){
			nameOfTok = "POW";
		}
		else if(t == GTHAN){
			nameOfTok = "GTHAN";
		}
		else if(t == LTHAN){
			nameOfTok = "LTHAN";
		}
		else{
			nameOfTok = "CAT";
		}

		out<<nameOfTok;

	}
    else if(t == COMMA || t == LPAREN || t == RPAREN || t == DOT || t == DCOLON || t == DEF){
		string nameOfTok;
		if(t == COMMA){
			nameOfTok = "COMMA";
		}
		else if(t == LPAREN){
			nameOfTok = "LPAREN";
		}
		else if(t == RPAREN){
			nameOfTok = "RPAREN";
		}
		else if(t == DOT){
			nameOfTok = "DOT";
		}
		else if(t == DCOLON){
			nameOfTok = "DCOLON";
		}
		else{
			nameOfTok = "DEF";
		}

		out<<nameOfTok;
	}

	return out;
    

};

LexItem id_or_kw(const string& lexeme, int linenum){
    LexItem lex;
    auto it1 = StrToTok.find(lexeme);
    if(it1 != StrToTok.end()){
		lex = LexItem(it1->second,lexeme,linenum);

    }
    else{
		lex = LexItem(IDENT,lexeme,linenum);
    }

    return lex;

};

LexItem getNextToken(istream& in, int& linenum){
	
	enum TokState { START, INID, INSTRING, ININT, INREAL} lexState = START;

	string lexeme;
	char ch;
	bool isDoubleQuote = false;
	while(in.get(ch)){
		switch (lexState) {
			case START:
				if(!lexeme.empty()){
					lexeme = "";
				}
				
				if(ch == '\n'){
					linenum++;
				}
				else if(isspace(ch)){
					break;
				}
				else if(ch == '!'){
					while(in.peek() != '\n'){
						in.get();
						continue;
					}
				}


				if(isalpha(ch)){
					lexeme+=ch;
					lexState = INID;
				}
				else if(isdigit(ch)){
					lexeme+=ch;
					lexState = ININT;
				}
				else if(ch == '.'){

					if(!isdigit(in.peek())){
						lexeme+=ch;
						return LexItem(DOT,lexeme,linenum);
					}
					else{
						lexeme+=ch;
						lexState = INREAL;
					}
				}
				else if(ch == '\'' || ch == '\"'){
					isDoubleQuote = (ch == '\"') ? true:false;
					lexeme+="\"";
					lexState = INSTRING;
				}
				else if(ch == '+'){
					lexeme+=ch;
					return LexItem(PLUS,lexeme,linenum);
				}
				else if(ch == '-'){
					lexeme+=ch;
					return LexItem(MINUS,lexeme,linenum);
				}
				else if(ch == '*'){
					lexeme+=ch;
					if(in.peek() == '*'){
						lexeme+= in.get();
						return LexItem(POW,lexeme,linenum);
					}
					else if(in.peek() == ','){
						return LexItem(DEF,lexeme,linenum);
					}
					else{
						return LexItem(MULT,lexeme,linenum);
					}
				}
				else if(ch == '/'){
					lexeme+=ch;
					if(in.peek() == '/'){
						lexeme+=in.get();
						return LexItem(CAT,lexeme,linenum);
					}
					else{
						return LexItem(DIV,lexeme,linenum);
					}
				}
				else if(ch == '='){
					lexeme+=ch;
					if(in.peek() == '='){
						lexeme+=in.get();
						return LexItem(EQ,lexeme,linenum);
					}
					else{
						return LexItem(ASSOP,lexeme,linenum);
					}
				}
				else if(ch == '<'){
					lexeme+=ch;
					return LexItem(LTHAN,lexeme,linenum);
				}
				else if(ch == '>'){
					lexeme+=ch;
					return LexItem(GTHAN,lexeme,linenum);
				}
				else if(ch == ','){
					lexeme+=ch;
					return LexItem(COMMA,lexeme,linenum);
				}
				else if(ch == '('){
					lexeme+=ch;
					return LexItem(LPAREN,lexeme,linenum);
				}
				else if(ch == ')'){
					lexeme+=ch;
					return LexItem(RPAREN,lexeme,linenum);
				}
				else if(ch == ':'){
					lexeme+=ch;
					if(in.peek() == ':'){
						lexeme+= in.get();
						return LexItem(DCOLON,lexeme,linenum);
					}
					else{
						return LexItem(ERR,lexeme,linenum);
					}
				}
				else if(ch == '$'){
					lexeme+=ch;
					linenum++;
					return LexItem(ERR,lexeme,linenum);
				}
				


				break;
			case INID:
				if(isalpha(ch) || isdigit(ch) || ch == '_'){
					lexeme+=ch;
					while(isalpha(in.peek()) || isdigit(in.peek()) || in.peek() == '_'){
						lexeme+=in.get();
					}
				}
				else{
					in.putback(ch);
					lexState = START;
					return LexItem(IDENT,lexeme,linenum);
				}
				
				
				
				if(lexeme == "program" || lexeme == "PROGRAM"){
					lexState = START;
					return LexItem(PROGRAM,lexeme,linenum);
				}
				else if(lexeme == "end" || lexeme == "END"){
					lexState = START;
					return LexItem(END,lexeme,linenum);
				}
				else if(lexeme == "else" || lexeme == "ELSE"){
					lexState = START;
					return LexItem(ELSE,lexeme,linenum);
				}
				else if(lexeme == "if" || lexeme == "IF"){
					lexState = START;
					return LexItem(IF,lexeme,linenum);
				}
				else if(lexeme == "integer" || lexeme == "INTEGER"){
					lexState = START;
					return LexItem(INTEGER,lexeme,linenum);
				}
				else if(lexeme == "real" || lexeme == "REAL"){
					lexState = START;
					return LexItem(REAL,lexeme,linenum);
				}
				else if(lexeme == "character" || lexeme == "CHARACTER"){
					lexState = START;
					return LexItem(CHARACTER,lexeme,linenum);
				}
				else if(lexeme == "print" || lexeme == "PRINT"){
					lexState = START;
					return LexItem(PRINT,lexeme,linenum);
				}
				else if(lexeme == "len" || lexeme == "LEN"){
					lexState = START;
					return LexItem(LEN,lexeme,linenum);
				}
				else if(lexeme == "then" || lexeme == "THEN"){
					lexState = START;
					return LexItem(THEN,lexeme,linenum);
				}
				else if(ch == ','){
					return LexItem(COMMA,lexeme,linenum);
				}

			 	
				lexState = START;
				return LexItem(IDENT,lexeme,linenum);
			case ININT:
				if(isdigit(ch)){
					lexeme+=ch;
					while(isdigit(in.peek())){
						lexeme+=in.get();
					}

					if(in.peek() == '.'){
						lexeme+=in.get();
						lexState = INREAL;
						break;
					}
				}
				else if(ch == '.'){
					lexeme+=ch;
					lexState = INREAL;
					break;
				}
				else{
					in.putback(ch);
				}
				
				lexState = START;
				return LexItem(ICONST,lexeme,linenum);
			case INREAL:
				if(isdigit(ch)){
					lexeme+=ch;
					while(isdigit(in.peek())){
						lexeme+=in.get();
					}

					if(in.peek() == '.'){
						lexeme+=in.get();
						linenum++;
						return LexItem(ERR,lexeme,linenum);
					}
					
					lexState = START;
					return LexItem(RCONST,lexeme,linenum);
				}
				else{
					return LexItem(ERR,lexeme,linenum);
				}

				lexState = START;
				break;
			case INSTRING:
				if(isalpha(ch)){
					lexeme+=ch;
					if(isDoubleQuote == false){
						while(in.peek() != '\''){
							lexeme+=in.get();
							if(in.peek() == '\"' || in.peek() == '\n'){
								if(in.peek() != '\n'){
									lexeme+=in.get();
								}
								linenum++;
								return LexItem(ERR,lexeme,linenum);
							}
						}
						in.get();
						lexeme+="\"";

					}
					else{
						while(in.peek() != '\"'){
							lexeme+=in.get();
							if(in.peek() == '\'' || in.peek() == '\n'){
								if(in.peek() != '\n'){
									lexeme+=in.get();
								}
								linenum++;
								return LexItem(ERR,lexeme,linenum);
							}
						}
						lexeme += in.get();

					}

					lexState = START;
					return LexItem(SCONST,lexeme,linenum);
				}

				lexState = START;
				break;
			


		}
	}

	if(in.eof()){
		return LexItem(DONE,"",linenum);
	}
	
	
	return LexItem(ERR,"",linenum);



};
