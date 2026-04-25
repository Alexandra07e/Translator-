#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "utils.h"

int main(){
	char *buf = loadFile("tests/testparser.c");
	Token *tokens = tokenize(buf);

	// showTokens(tokens);   // decomentezi daca vrei sa vezi tokenii
	parse(tokens);
	printf("syntax correct\n");
    freeTokens(tokens);
	free(buf);
	return 0;
}