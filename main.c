#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

// Deklaracje funkcji leksera, parsera i interpretera...

int main(int argc, char *argv[]){
    const char *source_code = "wyswietl(123);";
    
    // Tokenizacja (Lexer)
    printf("\nStarting lexer \n");
    Lexer *lexer = create_lexer(source_code);
    Token *tokens = tokenize(lexer);
    printf("Finished lexer \n");

    // Parsowanie (Parser)
    printf("\nStarting parsing \n");
    Parser *parser = create_parser(tokens);
    ASTNode *program = parse_program(parser);
    printf("Finished parsing\n");
    // Interpretacja (Interpreter)

    printf("\nStarting interpretation \n");
    interpret_program(program);
    printf("Finished interpretation \n");

    // Zwolnienie zasobow
    // for(Token *token = tokens; token->type != TOKEN_END; ++token){
    //     free_token(token);
    // }
    free(tokens); // Ponieważ tokenize używa realloc, musimy tylko zwolnić pamięć tokenów
    free_lexer(lexer);
    free_node(program);
    free_parser(parser); 

    return 0;
}
