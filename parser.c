#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

// definicja struktur
typedef struct ASTNode {
    // definiowanie typow wezlow AST
    enum {
        NODE_PRINT, NODE_INPUT, NODE_ASSIGNMENT, NODE_IF, NODE_EXPRESSION
    } type;
    union {
        struct { struct ASTNode *expression; } print_statement;
        struct { char *identifier; } input_statement;
        struct { struct ASTNode *condition; struct ASTNode *body;} if_statement;
        struct { char *identifier; struct ASTNode *expression;} assignment;
        struct {
            enum { EXPR_BINARY, EXPR_NUMBER, EXPR_IDENTIFIER } type;
            union {
                struct {struct ASTNode *left; char op; struct ASTNode *right;} binary;
                int number;
                char *identifier;
            } value;
        } expression;
    } data;
    struct ASTNode *next;
} ASTNode;

// funkcja tworzaca nowy wezel AST, inicjalizując jego pola, alokuje pamiec i zeruje wszystkie pola
ASTNode *create_node(){
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if(!node){
        fprintf(stderr, "Failed to allocate memory for ASTNode. \n");
    }
    memset(node, 0, sizeof(ASTNode));
    return node;
}

// zwalnia pamiec uzywana przez wezel AST, 
void free_node(ASTNode *node){
    while(node){
        printf("Freeing node of type: %d\n", node->type);
        ASTNode *next_node = node->next;
        switch(node->type){
            case NODE_PRINT: free_node(node->data.print_statement.expression); break;
            case NODE_INPUT: free(node->data.input_statement.identifier); break;
            case NODE_ASSIGNMENT: free(node->data.assignment.identifier); free_node(node->data.assignment.expression); break;
            case NODE_IF: free_node(node->data.if_statement.condition); free_node(node->data.if_statement.body); break;
            case NODE_EXPRESSION:
            if(node->data.expression.type == EXPR_BINARY){
                free_node(node->data.expression.value.binary.left);
                free_node(node->data.expression.value.binary.right);
            }
            else if(node->data.expression.type == EXPR_IDENTIFIER){
                free(node->data.expression.value.identifier);
            }
            break;
        }
        free(node);
        node = next_node;
    }
}

typedef struct {
    Token *tokens;
    size_t position;
} Parser;

// funkcja tworzaca nowy parser, inicjalizujac jego pola, alokuje pamiec
Parser *create_parser(Token *tokens){
    Parser *parser  = (Parser *)malloc(sizeof(Parser));
    parser->tokens = tokens;
    parser->position = 0;
    return parser;
}

// zwolnienie pamieci uzywanej przez parser
void free_parser(Parser *parser){
    if(parser){
        free(parser);
    }
}

// funkcja zwraca aktualny token
Token *current_token(Parser *parser){
    return &parser->tokens[parser->position];
}

// funkcja przesuwa parser do nastepnego tokenu
Token *next_token(Parser *parser){
    parser->position++;
    return current_token(parser);
}
// funkcja oczekujaca konkretnego typu tokenu, przesuwajaca parser do nastpnego tokenu i zwracajaca aktualny token
Token *expect_token(Parser *parser, TokenType type){
    Token *token = current_token(parser);
    printf("Current token: %d\n", token->type);
    if(token->type != type){
        fprintf(stderr, "Expected token type %d, but got %d\n", type, token->type);
        exit(1);
    }
    next_token(parser);
    return token; 
}

// funkcja parsujaca wyrazenia. obsluguje liczby i identyfikatory (TOKEN_NUMBER, TOKEN_IDENTIFIER)
ASTNode *parse_expression(Parser *parser){
    printf("Parsing expression\n");
    Token *token = current_token(parser);
    printf("Ezxpression token type: %d\n", token->type);
    if(token->type == TOKEN_NUMBER){
        printf("Parsing number: %s\n", token->value);
        ASTNode *node = create_node();
        node->type = NODE_EXPRESSION;
        node->data.expression.type = EXPR_NUMBER;
        // node->data.expression.value.identifier = strdup(token->value);
        node->data.expression.value.number = atoi(token->value);
        next_token(parser);
        return node;
    }
    else if (token->type == TOKEN_IDENTIFIER){
        printf("Parsing identifier: %s\n", token->value);
        ASTNode *node = create_node();
        node->type = NODE_EXPRESSION;
        node->data.expression.type = EXPR_IDENTIFIER;
        node->data.expression.value.identifier = strdup(token->value);
        next_token(parser);
        return node;
    }
    else {
        fprintf(stderr, "Unexpected token in expression: %s\n", token->value);
        exit(1);
    }
}

ASTNode *parse_print_statement(Parser *parser){
    printf("Parsing print statement (print in parse_print_statment)\n");
    expect_token(parser, TOKEN_PRINT);
    expect_token(parser, TOKEN_LPAREN);
    ASTNode *expression = parse_expression(parser);
    expect_token(parser, TOKEN_RPAREN);
    expect_token(parser, TOKEN_SEMICOLON);
    printf("Finished parsing print statement\n");

    ASTNode *node = create_node();
    node->type = NODE_PRINT;
    node->data.print_statement.expression = expression;

    return node;
}

ASTNode *parse_input_statement(Parser *parser){
    expect_token(parser, TOKEN_INPUT);
    expect_token(parser, TOKEN_LPAREN);
    Token *identifier = expect_token(parser, TOKEN_IDENTIFIER); 
    expect_token(parser, TOKEN_RPAREN);
    expect_token(parser, TOKEN_SEMICOLON);

    ASTNode *node = create_node();
    node->type = NODE_INPUT;
    node->data.input_statement.identifier = strdup(identifier->value);
    
    return node;
}

ASTNode *parse_assignment(Parser *parser){
    Token *identifier = expect_token(parser, TOKEN_IDENTIFIER);
    expect_token(parser, TOKEN_ASSIGN);
    ASTNode *expression = parse_expression(parser);
    expect_token(parser, TOKEN_SEMICOLON);

    ASTNode *node = create_node();
    node->data.assignment.identifier = strdup(identifier->value);
    node->data.assignment.expression = expression;

    return node;
}

ASTNode *parse_if_statement(Parser *parser){
    expect_token(parser, TOKEN_IF); 
    expect_token(parser, TOKEN_LPAREN);
    ASTNode *condition = parse_expression(parser);
    expect_token(parser, TOKEN_RPAREN);
    expect_token(parser, TOKEN_LBRACE);

    ASTNode *body = create_node();
    body->type = NODE_IF;
    
    while(current_token(parser)->type != TOKEN_RBRACE){
        ASTNode *statement = parse_print_statement(parser);
        body->data.if_statement.body = statement;
    }

    expect_token(parser, TOKEN_RBRACE);

    ASTNode *node = create_node();
    node->type = NODE_IF;
    node->data.if_statement.condition = condition;
    node->data.if_statement.body = body;

    return node;
}

ASTNode *parse_statement(Parser *parser){
    Token *token = current_token(parser);
    printf("Parsing statement with token: %d\n", token->type);
    if(token->type == TOKEN_PRINT){
        return parse_print_statement(parser);
    }
    else if(token->type == TOKEN_INPUT){
        return parse_input_statement(parser);
    }
    else if(token->type == TOKEN_IDENTIFIER){
        return parse_assignment(parser);
    }
    else if(token->type == TOKEN_IF){
        return parse_if_statement(parser);
    }
    else {
        fprintf(stderr, "Unexpected token in statement: %s\n", token->value);
        exit(1);
    }
}

ASTNode *parse_program(Parser *parser) {
    ASTNode *program = NULL;
    ASTNode *last_statement = NULL;
    
    while (1) {
        printf("Current token in parse program: %d\n", current_token(parser)->type);
        if (current_token(parser)->type == 14) {
            printf("Reached end of token stream in parse_program.\n");
            break;
        }
        ASTNode *statement = parse_statement(parser);
        if (!statement) break;
        
        if (!program) {
            program = statement;
        } else {
            last_statement->next = statement;
        }
        last_statement = statement;
        printf("Parsed statement added to program\n");
    }
    printf("Finished parsing program\n");
    return program;
}
