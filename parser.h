#pragma once
#include <stdbool.h>
#include "lexer.h"
#include "ad.h"
#include "at.h"

void parse(Token *tokens);
void tkerr(const char *fmt,...);
bool consume(int code);
bool typeBase(Type *t);
bool unit();
bool arrayDecl(Type *t);
bool varDef();
bool structDef();
bool fnParam();
bool fnDef();
bool stmCompound(bool newDomain);
bool stm();
bool expr(Ret *r);
bool exprAssign(Ret *r);
bool exprOrPrim(Ret *r);
bool exprOr(Ret *r);
bool exprAndPrim(Ret *r);
bool exprAnd(Ret *r);
bool exprEqPrim(Ret *r);
bool exprEq(Ret *r);
bool exprRelPrim(Ret *r);
bool exprRel(Ret *r);
bool exprAddPrim(Ret *r);
bool exprAdd(Ret *r);
bool exprMulPrim(Ret *r);
bool exprMul(Ret *r);
bool exprCast(Ret *r);
bool exprUnary(Ret *r);
bool exprPostfixPrim(Ret *r);
bool exprPostfix(Ret *r);
bool exprPrimary(Ret *r);