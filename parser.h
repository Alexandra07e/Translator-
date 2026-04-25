#pragma once
#include <stdbool.h>
#include "lexer.h"

void parse(Token *tokens);
void tkerr(const char *fmt,...);
bool consume(int code);
bool typeBase();
bool unit();
bool arrayDecl();
bool varDef();
bool structDef();
bool fnParam();
bool fnDef();
bool stmCompound();
bool stm();
bool expr();
bool exprAssign();
bool exprOrPrim();
bool exprOr();
bool exprAndPrim();
bool exprAnd();
bool exprEqPrim();
bool exprEq();
bool exprRelPrim();
bool exprRel();
bool exprAddPrim();
bool exprAdd();
bool exprMulPrim();
bool exprMul();
bool exprCast();
bool exprUnary();
bool exprPostfixPrim();
bool exprPostfix();
bool exprPrimary();