#include <stdio.h>
#include "utils.h"
#include "lexer.h"

int main(){
    char *buf = loadFile("tests/testlex.c");
    Token *tokens = tokenize(buf);
    showTokens(tokens);
    freeTokens(tokens);
    free(buf);
    return 0;
}