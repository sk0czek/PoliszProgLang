#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>

// definiowanie typow tokenow, ktore lekser moze rozpoznac
typedef enum {
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_NUMBER, TOKEN_PRINT, TOKEN_INPUT, TOKEN_IF, TOKEN_ASSIGN, TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_LPAREN, TOKEN_RPAREN , TOKEN_SEMICOLON, TOKEN_END, TOKEN_IDENTIFIER
} TokenType;

// struktra reprezentujaca stan lexera, zawiera zrodlowy ciag znakow oraz aktualna pozycje
typedef struct {
    char *source;
    size_t position;
} Lexer;
// struktura reprezentujaca tokem, zawiera typ tokenu oraz jego warrtosc
typedef struct {
    TokenType type;
    char *value;
} Token;

// funkcja tworzaca token
Token *create_token(TokenType type, const char *value){
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = type;
    token->value = strdup(value);
    return token;
}


// funkcja zwalniająca pamiec uzyta przez token
void free_token(Token *token){
    if(token) {
        free(token->value);
        free(token);
    }
}

// funkcja zwalniajaca pamiec dla calej tablicy tokenow
void free_tokens(Token *tokens) {
    for(int i = 0; tokens[i].type != TOKEN_END; i++) {
        if(tokens[i].value) {
            free(tokens[i].value);
        }
    }
    free(tokens);
}

// funkcja tworzaca nowy lexer
Lexer *create_lexer(const char *source){
    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
    lexer->source = strdup(source);
    lexer->position = 0;
    return lexer;
}

//funkcja zwalniajaca pamiec uzywana przez lexer
void free_lexer(Lexer *lexer){
    if(lexer){
        free(lexer->source);
        free(lexer);
    }
}

// funkcja generujaca nastenpny token na podstawie zrodlowego ciagu znakow
Token *lexer_next_token(Lexer *lexer){
    while(lexer->source[lexer->position] != '\0'){
        char current = lexer->source[lexer->position]; // getting char from actural string

        if(isspace(current)){
            lexer->position++;
            continue;
        } 

        if(isalpha(current)){
            size_t start  = lexer->position; // przechodzi na poczatkowa pozycje identyfikatora
            while(isalnum(lexer->source[lexer->position])){ // przesuwa petle dopki nie napotka znaku alfabetu
                lexer->position++;
            }

            size_t length = lexer->position-start;
            char *identifier = strndup(&lexer->source[start], length);

            if(strcmp(identifier, "wyswietl") == 0){
                free(identifier);
                return create_token(TOKEN_PRINT, "wyswietl");
            }

            if(strcmp(identifier, "podaj") == 0){
                free(identifier);
                return create_token(TOKEN_INPUT, "podaj");
            }

            if(strcmp(identifier, "if") == 0){
                free(identifier);
                return create_token(TOKEN_IF, "jezeli");
            }

            return create_token(TOKEN_IDENTIFIER, identifier);
        }
        
        if(isdigit(current)){
            size_t start = lexer->position;

            while(isdigit(lexer->source[lexer->position])){
                lexer->position++;
            }

            size_t length = lexer->position - start;
            char *number = strndup(&lexer->source[start], length);

            return create_token(TOKEN_NUMBER, number);
        }

        switch(current){
            case '+': lexer->position++; return create_token(TOKEN_PLUS, "+");
            case '-': lexer->position++; return create_token(TOKEN_MINUS, "-");
            case '*': lexer->position++; return create_token(TOKEN_STAR, "*");
            case '/': lexer->position++; return create_token(TOKEN_SLASH, "/");
            case '(': lexer->position++; return create_token(TOKEN_LPAREN, "(");
            case ')': lexer->position++; return create_token(TOKEN_RPAREN, ")");
            case '{': lexer->position++; return create_token(TOKEN_LBRACE, "{");
            case '}': lexer->position++; return create_token(TOKEN_RBRACE, "}");
            case ';': lexer->position++; return create_token(TOKEN_SEMICOLON, ";");
            case '=': lexer->position++; return create_token(TOKEN_ASSIGN, "=");
        }

        lexer->position++;
    }
    return create_token(TOKEN_END, "");
}



Token *tokenize(Lexer *lexer) {
    Token *tokens = NULL;
    size_t token_count = 0;

    Token *token;

    while ((token = lexer_next_token(lexer))->type != TOKEN_END) {
        tokens = (Token *)realloc(tokens, (token_count + 1) * sizeof(Token));
        if (tokens == NULL) {
            fprintf(stderr, "Failed to allocate memory for tokens.\n");
            exit(1);
        }

        // Kopiowanie tokenu
        tokens[token_count].type = token->type;
        if (token->value) {
            tokens[token_count].value = strdup(token->value);
        } else {
            tokens[token_count].value = NULL;
        }
        token_count++;

        free_token(token); // Zwolnienie pamięci zaalokowanej dla pojedynczego tokenu
    }

    tokens = (Token *)realloc(tokens, (token_count + 1) * sizeof(Token));
    if (tokens == NULL) {
        fprintf(stderr, "Failed to allocate memory for tokens.\n");
        exit(1);
    }

    // Przechowywanie tokenu TOKEN_END
    tokens[token_count].type = TOKEN_END;
    tokens[token_count].value = NULL;
    token_count++;
    free_token(token);

    return tokens;
}
