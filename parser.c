#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "parser.h"

Token *iTk;		// the iterator in the tokens list
Token *consumedTk;		// the last consumed token

void tkerr(const char *fmt,...)
{
	fprintf(stderr,"error in line %d: ",iTk->line);
	va_list va;
	va_start(va,fmt);
	vfprintf(stderr,fmt,va);
	va_end(va);
	fprintf(stderr,"\n");
	exit(EXIT_FAILURE);
	}

bool consume(int code)
{
	if(iTk->code==code){
		consumedTk=iTk;
		iTk=iTk->next;
		return true;
		}
	return false;
	}

// typeBase: TYPE_INT | TYPE_DOUBLE | TYPE_CHAR | STRUCT ID
bool typeBase()
{
    if(consume(TYPE_INT)){
        return true;
    }
    if(consume(TYPE_DOUBLE)){
        return true;
    }
    if(consume(TYPE_CHAR)){
        return true;
    }
    if(consume(STRUCT)){
        if(consume(ID)){
            return true;
        }
    }
    return false;
}

// unit: ( structDef | fnDef | varDef )* END
bool unit()
{
	for(;;){
		if(structDef()){}
		else if(fnDef()){}
		else if(varDef()){}
		else break;
	}
	if(consume(END)){
		return true;
	} 
    tkerr("syntax error");
	return false;
}

    // arrayDecl: LBRACKET INT? RBRACKET
bool arrayDecl(){
    Token *start = iTk;
    if(consume(LBRACKET)){
        consume(INT); // optional
        if(consume(RBRACKET)){
            return true;
        }
        tkerr("missing ]");
    }
    iTk = start;
    return false;
}

// varDef: typeBase ID arrayDecl? SEMICOLON
bool varDef(){
    Token *start = iTk;
    if(typeBase()){
        if(consume(ID)){
            if(arrayDecl()){}   // optional
            if(consume(SEMICOLON)){
                return true;
            }
            tkerr("missing ;");
        }
        tkerr("missing var name");
    }
    iTk = start;
    return false;
}

// structDef: STRUCT ID LACC varDef* RACC SEMICOLON
bool structDef(){
    Token *start = iTk; //saving current position in tokens list, if parsing isn't successful we come back
    if(consume(STRUCT)){
        if(consume(ID)){
            if(consume(LACC)){
                for(;;){
                    if(varDef()){}
                    else break;
                }
                if(consume(RACC)){
                    if(consume(SEMICOLON)){
                        return true;
                    }
                    tkerr("missing ; after struct");
                }
                tkerr("missing } at the end of the struct");
            }
            iTk = start;
            return false; 
        }
        tkerr("missing struct name");
    }
    iTk = start;
    return false;
}

// fnParam: typeBase ID arrayDecl?
bool fnParam(){
    Token *start = iTk;
    if (typeBase()){
        if (consume(ID)){
            if (arrayDecl()) {}
            return true;
        }
        tkerr("missing param name");
    }
    iTk = start;
    return false;
}

//stmCompound: LACC (varDef | stm)* RACC
bool stmCompound(){
    Token *start = iTk;
    if(consume(LACC)){
        for(;;){
            if(varDef()){}
            else if(stm()){}
            else break;
        }
        if(consume(RACC)){
            return true;
        }
        tkerr("missing }");
    }
    iTk = start;
    return false;
}

//exprPrimary: ID ( LPAR ( expr ( COMMA expr )* )? RPAR )? | INT | DOUBLE | CHAR | STRING | LPAR expr RPAR
bool exprPrimary(){
    Token *start = iTk;
    if (consume(ID)){
        if (consume(LPAR)){
            if (expr()){
                while (consume(COMMA)){
                    if (!expr()){
                        tkerr("missing expression after ,");
                    }
                }
            }
            if (consume(RPAR)){
                return true;
            }
            tkerr("missing )");
        }
        return true;
    }
    if (consume(INT)){
        return true;
    }
    if (consume(DOUBLE)){
        return true;
    }
    if (consume(CHAR)){
        return true;
    }
    if (consume(STRING)){
        return true;
    }
    if (consume(LPAR)){
        if (expr()){
            if (consume(RPAR)){
                return true;
            }
            tkerr("missing )");
        }
        tkerr("missing expression between brackets");
    }
    iTk = start;
    return false;
}

//exprPostfixPrim: LBRACKET expr RBRACKET exprPostfixPrim | DOT ID exprPostfixPrim | ε
bool exprPostfixPrim(){
    if (consume(LBRACKET)){
        if (expr()){
            if (consume(RBRACKET)){
                return exprPostfixPrim();
            }
            tkerr("missing ]");
        }
        tkerr("missing expression from []");
    }
    if (consume(DOT)){
        if (consume(ID)){
            return exprPostfixPrim();
        }
        tkerr("missing name after .");
    }
    return true;
}

//exprPostfix: exprPrimary exprPostfixPrim
bool exprPostfix(){
    Token *start = iTk;
    if (exprPrimary()){
        if (exprPostfixPrim()){
            return true;
        }
    }
    iTk = start;
    return false;
}


//exprUnary: ( SUB | NOT ) exprUnary | exprPostfix
bool exprUnary(){
    Token *start = iTk;
    if (consume(SUB)){
        if (exprUnary()){
            return true;
        }
        tkerr("missing expression after '-'");
    }
    iTk = start;
    if (consume(NOT)){
        if (exprUnary()){
            return true;
        }
        tkerr("missing expression after '!'");
    }
    iTk = start;
    if (exprPostfix()){
        return true;
    }
    iTk = start;
    return false;
}

//exprCast: LPAR typeBase arrayDecl? RPAR exprCast | exprUnary
bool exprCast(){
    Token *start = iTk;
    if (consume(LPAR)){
        if (typeBase()){
            if (arrayDecl()) {}   // optional
            if (consume(RPAR)){
                if (exprCast()){
                    return true;
                }
                tkerr("missing expression after cast");
            }
            tkerr("missing ) after cast");
        }
        iTk = start;
    }
    if (exprUnary()){
        return true;
    }
    iTk = start;
    return false;
}

//exprMul: exprCast exprMulPrim
bool exprMul(){
    Token *start = iTk;
    if (exprCast()){
        if (exprMulPrim()){
            return true;
        }
    }
    iTk = start;
    return false;
}

//exprMulPrim: ( MUL | DIV ) exprCast exprMulPrim | ε
bool exprMulPrim(){
    if (consume(MUL)){
        if (exprCast()){
            return exprMulPrim();
        }
        tkerr("missing expression after *");
    }
    if (consume(DIV)){
        if (exprCast()){
            return exprMulPrim();
        }
        tkerr("missing expression after /");
    }
    return true;
}

//exprAdd: exprMul exprAddPrim
bool exprAdd(){
    Token *start = iTk;
    if (exprMul()){
        if (exprAddPrim()){
            return true;
        }
    }
    iTk = start;
    return false;
}

//exprAddPrim: ( ADD | SUB ) exprMul exprAddPrim | ε
bool exprAddPrim(){
    if (consume(ADD)){
        if (exprMul()){
            return exprAddPrim();
        }
        tkerr("missing expression after +");
    }
    if (consume(ADD) || consume(SUB)){
        if (exprMul()){
            return exprAddPrim();
        }
        tkerr("missing expression after -");
    }
    return true;
}

//exprRel: exprAdd exprRelPrim
bool exprRel(){
    Token *start = iTk;
    if (exprAdd()){
        if (exprRelPrim()){
            return true;
        }
    }
    iTk = start;
    return false;
}

//exprRelPrim: ( LESS | LESSEQ | GREATER | GREATEREQ ) exprAdd exprRelPrim | ε
bool exprRelPrim(){
    if (consume(LESS)){
        if (exprAdd())
            return exprRelPrim();
        tkerr("missing expression after <");
    }
    else if (consume(LESSEQ)){
        if (exprAdd())
            return exprRelPrim();
        tkerr("missing expression after <=");
    }
    else if (consume(GREATER)){
        if (exprAdd())
            return exprRelPrim();
        tkerr("missing expression after >");
    }
    else if (consume(GREATEREQ)){
        if (exprAdd())
            return exprRelPrim();
        tkerr("missing expression after >=");
    }
    return true;
}

// exprEq: exprRel exprEqPrim
bool exprEq(){
    Token *start = iTk;
    if (exprRel()) {
        if (exprEqPrim()) {
            return true;
        }
    }
    iTk = start;
    return false;
}

// exprEqPrim: (EQUAL|NOTEQ) exprRel exprEqPrim | ε
bool exprEqPrim() {
    if (consume(EQUAL)) {
        if (exprRel()) {
            return exprEqPrim();
        }
        tkerr("missing expression after == ");
    }
    if (consume(NOTEQ)) {
        if (exprRel()) {
            return exprEqPrim();
        }
        tkerr("missing expression after !=");
    }
    return true;
}

// exprAnd: exprEq exprAndPrim
bool exprAnd(){
    Token *start = iTk;
    if (exprEq()) {
        if (exprAndPrim()) {
            return true;
        }
    }
    iTk = start;
    return false;
}

// exprAndPrim: AND exprEq exprAndPrim | ε
bool exprAndPrim() {
    if (consume(AND)) {
        if (exprEq()) {
            return exprAndPrim();
        }
        tkerr("missing expression after &&");
    }
    return true;
}

// exprOr: exprAnd exprOrPrim
bool exprOr(){
    Token *start = iTk;
    if (exprAnd()) {
        if (exprOrPrim()) {
            return true;
        }
    }
    iTk = start;
    return false;
}

// exprOrPrim: OR exprAnd exprOrPrim | ε
bool exprOrPrim(){
    if (consume(OR)) {
        if (exprAnd()) {
            return exprOrPrim();
        }
        tkerr("missing expression after ||");
    }
    return true;
}

//expAssign: exprUnary ASSIGN exprAssign | exprOr
bool exprAssign(){
    Token *start = iTk;

    if(exprUnary()) {
        if(consume(ASSIGN)){
            if (exprAssign()){
                return true;
            }
            tkerr("missing expression after =");
        }
    }
    iTk = start;
    if(exprOr()){
        return true;
    }
    iTk = start;
    return false;
}

//expr: exprAssign
bool expr(){
    return exprAssign();
}

/*stm: stmCompound 
        | IF LPAR expr RPAR stm (ELSE stm)?
        | WHILE LPAR expr RPAR stm
        | RETURN expr? SEMICOLON
        | expr? SEMICOLON
*/
bool stm(){
    Token *start = iTk;
    // stmCompound
    if (stmCompound()) {
        return true;
    }
    // IF LPAR expr RPAR stm ( ELSE stm )?
    iTk = start;
    if (consume(IF)) {
        if (consume(LPAR)) {
            if (expr()) {
                if (consume(RPAR)) {
                    if (stm()) {
                        if (consume(ELSE)) {
                            if (stm()) {
                                return true;
                            } else {
                                tkerr("missing instr after else");
                            }
                        }
                        return true;
                    } else {
                        tkerr("missing instr after if");
                    }
                } else {
                    tkerr("invalid condition for if or missing )");
                }
            } else {
                tkerr("missing condition in if");
            }
        } else {
            tkerr("missing ( after if");
        }
    }
    // WHILE LPAR expr RPAR stm
    iTk = start;
    if (consume(WHILE)) {
        if (consume(LPAR)) {
            if (expr()) {
                if (consume(RPAR)) {
                    if (stm()) {
                        return true;
                    } else {
                        tkerr("missing instr after while");
                    }
                } else {
                    tkerr("invalid condition for while or missing )");
                }
            } else {
                tkerr("missing condition in while");
            }
        } else {
            tkerr("missing ( after while");
        }
    }
    // RETURN expr? SEMICOLON
    iTk = start;
    if (consume(RETURN)) {
        expr();  // optional
        if (consume(SEMICOLON)) {
            return true;
        } else
            tkerr("missing ; after return");
    }
    // expr? SEMICOLON
    iTk = start;
    if (expr()) {
        if (consume(SEMICOLON)) {
            return true;
        } else 
            tkerr("missing ; after expression");
    }

    iTk = start;
    if (consume(SEMICOLON)) {
        return true;
    }
    iTk = start;
    return false;
}

// fnDef: (typeBase | VOID) ID LPAR (fnParam(COMMA fnParam)*)? RPAR stmCompound
bool fnDef(){
    Token *start = iTk;
    // cazul 1: functie cu void
    if (consume(VOID)){
        if (!consume(ID)){
            tkerr("missing function name");
        }
        if (!consume(LPAR)){
            tkerr("missing ( after function name");
        }
    }
    // cazul 2: functie cu typeBase
    else if (typeBase()){
        if (!consume(ID)){
            iTk = start;
            return false;   // error in varDef 
        }
        if (!consume(LPAR)){
            iTk = start;
            return false;   // can be varDef
        }
    }
    else{
        iTk = start;
        return false;
    }
    if (fnParam()){
        while (consume(COMMA)){
            if (!fnParam()){
                tkerr("missing param after , or invalid param");
            }
        }
    }
    if (!consume(RPAR)){
        tkerr("invalid param sau missing )");
    }
    if (stmCompound()){
        return true;
    }
    tkerr("missing body function");
    return false;
}

void parse(Token *tokens){
	iTk=tokens;
	if(!unit())tkerr("syntax error");
	}
