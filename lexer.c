/*NUCLEUL ANALIZORULUI LEXICAL*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
#include "utils.h"

Token *tokens;	// single linked list of tokens
Token *lastTk;		// the last token in list

int line=1;		// the current line in the input file

// adds a token to the end of the tokens list and returns it
// sets its code and line
Token *addTk(int code){
	Token *tk=safeAlloc(sizeof(Token));
	tk->code=code;
	tk->line=line;
	tk->next=NULL;
	if(lastTk){
		lastTk->next=tk;
		}else{
		tokens=tk;
		}
	lastTk=tk;
	return tk;
	}

	//DEFINIM 
char *extract(const char *begin,const char *end){
	int len = end - begin;
	char *text = safeAlloc((len+1)*sizeof(char));
	strncpy(text, begin, len);
	text[len] = '\0';
	return text;
}

	//NUCLEUL trebuie completat mai departe (e facut space ul, END, COMMA, EQUAL)
Token *tokenize(const char *pch){
	const char *start;
	Token *tk;
	for(;;){
		switch(*pch){
			case ' ':case '\t':pch++;break;
			case '\r':		// handles different kinds of newlines (Windows: \r\n, Linux: \n, MacOS, OS X: \r or \n)
				if(pch[1]=='\n')pch++;
				// fallthrough to \n
			case '\n':
				line++;
				pch++;
				break;
			//atomi cu un simbol
			case '\0': addTk(END); return tokens;
			case ',': addTk(COMMA); pch++; break;
			case ';': addTk(SEMICOLON); pch++; break;
			case '(': addTk(LPAR); pch++; break;
			case ')': addTk(RPAR); pch++; break;
			case '[': addTk(LBRACKET); pch++; break;
			case ']': addTk(RBRACKET); pch++; break;
			case '{': addTk(LACC); pch++; break;
			case '}': addTk(RACC); pch++; break;
			case '+': addTk(ADD); pch++; break;
			case '-': addTk(SUB); pch++; break;
			case '*': addTk(MUL); pch++; break;
			case '.': addTk(DOT); pch++; break;

			//atom format din 2 simboluri
			case '=':{
				if(pch[1]=='='){
					addTk(EQUAL);
					pch+=2;
					} else {
					addTk(ASSIGN);
					pch++;
					}
				break;
			}
			case '&':{
				if(pch[1] == '&'){
					addTk(AND);
					pch+=2;
				}
				break;
			}
			case '|':{
				if(pch[1] == '|'){
					addTk(OR);
					pch+=2;
				}
				break;
			}
			case '!':{
				if(pch[1] == '='){
					addTk(NOTEQ);
					pch+=2;
				}
				else{
					addTk(NOT);
					pch++;
				}
				break;
			}
			case '<':{
				if(pch[1] == '='){
					addTk(LESSEQ);
					pch+=2;
				}
				else{
					addTk(LESS);
					pch++;
				}
				break;
			}
			case '>':{
				if(pch[1] == '='){
					addTk(GREATEREQ);
					pch+=2;
				}
				else{
					addTk(GREATER);
					pch++;
				}
				break;
			}
			//implementam si // pentru comentarii (sare peste comentariu)
			case '/':{
				if(pch[1] == '/'){
					pch+=2;
					while(*pch != '\n' && *pch != '\r' && *pch != '\0') pch++;
				} else{
					addTk(DIV);
					pch++;
				}
				break;
			}

			//identificator si keywords , aici implementam si constantele
			default:
				if(isalpha(*pch) || *pch=='_'){
					for(start=pch++; isalnum(*pch)||*pch=='_'; pch++){}
					char *text = extract(start,pch);
					if(strcmp(text,"char") == 0) addTk(TYPE_CHAR);
					else if(strcmp(text, "int") == 0) addTk(TYPE_INT);
					else if(strcmp(text, "double") == 0) addTk(TYPE_DOUBLE);
					else if(strcmp(text, "if") == 0) addTk(IF);
					else if(strcmp(text, "else") == 0) addTk(ELSE);
					else if(strcmp(text, "return") == 0) addTk(RETURN);
					else if(strcmp(text, "struct") == 0) addTk(STRUCT);
					else if(strcmp(text, "void") == 0) addTk(VOID);
					else if(strcmp(text, "while") == 0) addTk(WHILE);
					else{
						tk = addTk(ID);
						tk->text = text;
						}
					}
					
				//INT & DOUBLE
				else
				if(isdigit(*pch)){
					start = pch;
					while(isdigit(*pch)) pch++;

					int isDouble=0;
					//12.378
					if(*pch == '.' && isdigit(*(pch+1))){
						isDouble=1;
						pch++;
						while(isdigit(*pch)) pch++;
					}
					//12.37e+8 / 12.37e2 / 12.37e-2
					if(*pch == 'e' || *pch == 'E'){
						const char *p = pch+1;
						if(*p == '+' || *p == '-') p++;
						if(isdigit(*p)){
							isDouble = 1;
							pch = p;
							while(isdigit(*pch)) pch++;
						}
					}
					if(isDouble == 1){
						tk = addTk(DOUBLE);
						tk->text = extract(start, pch);
					}
					else{
						tk = addTk(INT);
						tk->text = extract(start, pch); // extrage din input numarul citit, de la start pana la pch
					}
				}
				//CHAR
				else
					if(*pch == '\''){
						start = pch;
						pch++; //we skip the '
						char c;

						if(*pch == '\\'){
							pch++;
							switch(*pch){
								case 'a': pch++; c='\a'; break;
								case 'b': pch++; c='\b'; break;
								case 'f': pch++; c='\f'; break;
								case 'n': pch++; c='\n'; break;
								case 'r': pch++; c='\r'; break;
								case 't': pch++; c='\t'; break;
								case 'v': pch++; c='\v'; break;
								case '\\': pch++; c = '\\'; break;
								case '\'': pch++; c = '\''; break;
								case '"': pch++; c = '\"'; break;
								case '0': pch++; c = '\0'; break;
								default: err("invalid escape in char constant");
							}
						}
						else
							if(*pch != '\'' && *pch != '\\' && *pch !='\n' && *pch != '\r' && *pch != '\0'){
								c = *pch;
								pch++;
							}
							else err("invalid char constant");

						if(*pch == '\''){
							pch++;
							tk = addTk(CHAR);
							tk->c = c;
							tk->text = extract(start, pch);
						} else err("missing ' in closing");
					}
				//STRING
				else if(*pch == '"'){
    				pch++;   //we skip the "

					char buffer[1000];
					int size = 0;

    				while(*pch != '"'){
						if(*pch == '\\'){
							pch++;
							switch(*pch){
								case 'a': buffer[size++]='\a'; pch++; break;
								case 'b': buffer[size++]='\b'; pch++; break;
								case 'f': buffer[size++]='\f'; pch++; break;
								case 'n': buffer[size++]='\n'; pch++; break;
								case 'r': buffer[size++]='\r'; pch++; break;
								case 't': buffer[size++]='\t'; pch++; break;
								case 'v': buffer[size++]='\v'; pch++; break;
								case '\\': buffer[size++]='\\'; pch++; break;
								case '\'': buffer[size++]='\''; pch++; break;
								case '"': buffer[size++]='\"'; pch++; break;
								case '0': buffer[size++]='\0'; pch++; break;
								default: err("invalid escape in string");
							}
						}
						else if(*pch == '\n' || *pch == '\r' || *pch == '\0')
							err("unterminated string");
						else{
							buffer[size++] = *pch;
							pch++;
						}
					}
					pch++;   //we skip the "
					buffer[size] = '\0';
					tk = addTk(STRING);
					tk->text = safeAlloc(size+1);
					strcpy(tk->text, buffer);
				}
				else err("invalid char: %c (%d)",*pch,*pch); 
			}
		}
}

void showTokens(const Token *tokens){
    for(const Token *tk = tokens; tk; tk = tk->next){
        printf("%d ", tk->line);
        switch(tk->code){
            case ID: printf("ID:%s", tk->text); break;
            case TYPE_INT: printf("TYPE_INT"); break;
            case TYPE_DOUBLE: printf("TYPE_DOUBLE"); break;
            case TYPE_CHAR: printf("TYPE_CHAR"); break;
            case STRUCT: printf("STRUCT"); break;
            case IF: printf("IF"); break;
            case ELSE: printf("ELSE"); break;
            case WHILE: printf("WHILE"); break;
            case RETURN: printf("RETURN"); break;
            case VOID: printf("VOID"); break;
            case INT: printf("INT:%d", atoi(tk->text)); break;
            case DOUBLE: printf("DOUBLE:%g", atof(tk->text)); break;
			case CHAR:{
				if(strcmp(tk->text, "'\\a'") == 0) printf("CHAR:\\a");
				else if(strcmp(tk->text, "'\\b'") == 0) printf("CHAR:\\b");
				else if(strcmp(tk->text, "'\\f'") == 0) printf("CHAR:\\f");
				else if(strcmp(tk->text, "'\\n'") == 0) printf("CHAR:\\n");
				else if(strcmp(tk->text, "'\\r'") == 0) printf("CHAR:\\r");
				else if(strcmp(tk->text, "'\\t'") == 0) printf("CHAR:\\t");
				else if(strcmp(tk->text, "'\\v'") == 0) printf("CHAR:\\v");
				else if(strcmp(tk->text, "'\\\\'") == 0) printf("CHAR:\\\\");
				else if(strcmp(tk->text, "'\\''") == 0) printf("CHAR:'");
				else if(strcmp(tk->text, "'\\\"'") == 0) printf("CHAR:\"");
				else if(strcmp(tk->text, "'\\0'") == 0) printf("CHAR:\\0");
				else printf("CHAR:%c", tk->text[1]); // la un char 'a', imi trebuie al doilea caracter
				break;
			}
            case STRING: printf("STRING:%s", tk->text); break;
            case COMMA: printf("COMMA"); break;
            case SEMICOLON: printf("SEMICOLON"); break;
            case LPAR: printf("LPAR"); break;
            case RPAR: printf("RPAR"); break;
            case LBRACKET: printf("LBRACKET"); break;
            case RBRACKET: printf("RBRACKET"); break;
            case LACC: printf("LACC"); break;
            case RACC: printf("RACC"); break;
            case ADD: printf("ADD"); break;
            case SUB: printf("SUB"); break;
            case MUL: printf("MUL"); break;
            case DIV: printf("DIV"); break;
            case DOT: printf("DOT"); break;
            case AND: printf("AND"); break;
            case OR: printf("OR"); break;
            case NOT: printf("NOT"); break;
            case ASSIGN: printf("ASSIGN"); break;
            case EQUAL: printf("EQUAL"); break;
            case NOTEQ: printf("NOTEQ"); break;
            case LESS: printf("LESS"); break;
            case LESSEQ: printf("LESSEQ"); break;
            case GREATER: printf("GREATER"); break;
            case GREATEREQ: printf("GREATEREQ"); break;
            case END: printf("END"); break;
            default: printf("UNKNOWN");
        }
        printf("\n");
    }
}

//elibereaza memoria pentru lista de tokeni si pentru string-urile alocate
void freeTokens(Token *tokens){
	Token *tk = tokens;
	while(tk){
		Token *next = tk->next;
		if(tk->code==ID || tk->code==INT || tk->code==DOUBLE || tk->code==CHAR || tk->code==STRING){
			free(tk->text);
		}
		free(tk);
		tk = next;
	}
}
