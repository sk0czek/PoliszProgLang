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
    Lexer *lexer = create_lexer(source_code);
    Token *tokens = tokenize(lexer);

    // Parsowanie (Parser)
    Parser *parser = create_parser(tokens);
    ASTNode *program = parse_program(parser);
    // Interpretacja (Interpreter)

    interpret_program(program);

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
