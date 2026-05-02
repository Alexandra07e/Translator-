#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "utils.h"
#include "ad.h"
#include "vm.h"
#include "at.h"

int main(){
	char *buf = loadFile("tests_type/testat.c");
	Token *tokens = tokenize(buf);

	// showTokens(tokens);   // decomentezi daca vrei sa vezi tokenii
	parse(tokens);
	printf("Syntax, domain and type analysis passed successfully.\n");
    
    freeTokens(tokens);
	free(buf);
	return 0;
}