#ifndef PARSER_H
#define PARSER_H

#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<stdio.h>

typedef struct ASTNode {
    // definiowanie typow wezlow AST
    enum {
        NODE_PRINT, NODE_INPUT, NODE_ASSIGNMENT, NODE_IF, NODE_EXPRESSION
    } type;
    union {
        struct { struct ASTNODE *expression; } print_statement;
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

typedef struct {
    Token *tokens;
    size_t position;
} Parser;

ASTNode *create_node();
void free_node(ASTNode *node);
Parser *create_parser(Token *tokens);
void free_parser(Parser *parser);
Token *current_token(Parser *parser);
Token *next_token(Parser *parser);
Token *expect_token(Parser *parser, TokenType type);
ASTNode *parse_expression(Parser *parser);
ASTNode *parse_print_statement(Parser *parser);
ASTNode *parse_input_statement(Parser *parser);
ASTNode *parse_if_statement(Parser *parser);
ASTNode *parse_assignment(Parser *parser);
ASTNode *parse_statement(Parser *parser);
ASTNode *parse_program(Parser *parser);

#endif
