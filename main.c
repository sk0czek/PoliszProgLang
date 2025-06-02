#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"


int main(int argc, char *argv[]){
    const char *source_code = "b=10;wyswietl(b);podaj(x);wyswietl(x);";
    
    // Tokenizacja (Lexer)
    Lexer *lexer = create_lexer(source_code);
    Token *tokens = tokenize(lexer);

    // Parsowanie (Parser)
    Parser *parser = create_parser(tokens);
    ASTNode *program = parse_program(parser);
    // Interpretacja (Interpreter)

    interpret_program(program);


    free_tokens(tokens);
    free_lexer(lexer);
    free_node(program);
    free_parser(parser); 

    return 0;
}
