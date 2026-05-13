#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "utils.h"
#include "ad.h"
#include "vm.h"
#include "at.h"

int main(){
	char *buf = loadFile("tests/testgc.c");
	Token *tokens = tokenize(buf);

	// showTokens(tokens);   // decomentezi daca vrei sa vezi tokenii

	pushDomain();
    vmInit();

	parse(tokens);
	printf("Syntax, domain and type analysis passed successfully.\n");

	Symbol *symMain = findSymbolInDomain(symTable, "main");
	if(!symMain) err("missing main function");

	Instr *entryCode = NULL;
	addInstr(&entryCode, OP_CALL)->arg.instr = symMain->fn.instr;
	addInstr(&entryCode, OP_HALT);

	run(entryCode);
    
	dropDomain();
    freeTokens(tokens);
	free(buf);
	return 0;
}