#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
#include "lexer.h"
#include "parser.h"

typedef struct {
    char *name;
    int value;
} Variable;

// tworzy nowa zmienna, alokuje pamiec dla struktury i kopiuje nazwe zmiennej
Variable *create_variable(const char *name, int value){
    Variable *variable = (Variable *)malloc(sizeof(Variable));
    variable->name = strdup(name);
    variable->value = value;

    return variable;
}

// zwalnia pamiec przydzielona na zmienna (w tym pamiec dla nazwy)
void free_variable(Variable *variable){
    if(variable){
        free(variable->name);
        free(variable);
    }
}

// tablica wskaznikow do zmiennych oraz liczb tych zmiennych, przechowuje wszystkie zmienne
typedef struct {
    Variable **variables;
    size_t count;
} Environment;

// tworzy srodowisku i inicjalizuje w nim pusta tablice zmiennych
Environment *create_environment(){
    Environment *env  = (Environment *)malloc(sizeof(Environment));
    env->variables = NULL;
    env->count = 0;

    return env;
}

// zwalnia pamiec przydziolona na srodowisko oraz wszystkie zmienne w nim zawarte
void free_environment(Environment *env){
    if(env){
        for(size_t i=0; i<env->count; i++){
            free_variable(env->variables[i]);
        }
    free(env->variables);
    free(env);
    }
}

// szuka zmiennej w srodowisku po nazwie i zwraca wskaznik do niej
Variable *find_variable(Environment *env, const char *name){
    for(size_t i =0; i<env->count; i++){
        if(strcmp(env->variables[i]->name, name) == 0){
            return env->variables[i];
        }
    }
    return NULL;
}

// dodaje nowa zmienna do srodowiska, rozszerzajac tablice i zwiekszajac licznik
void add_variables(Environment *env, Variable *variable){
    env->variables = (Variable **)realloc(env->variables, (env->count + 1) * sizeof(Variable *));
    env->variables[env->count++] = variable;
}

// ocenia wartosc wyrazenia na podstawie jego typu
int evaluate_expression(Environment *env, ASTNode *node){
    switch(node->data.expression.type){
        case EXPR_NUMBER:
            printf("Evaluating number: %d\n", node->data.expression.value.number);
            return node->data.expression.value.number;
        case EXPR_IDENTIFIER: {
            Variable *variable = find_variable(env, node->data.expression.value.identifier);     
            if(variable){
                return variable->value;
            }
            else {
                fprintf(stderr, "Undefined variables: %s\n", node->data.expression.value.identifier);
                exit(1);
            }
        }
        case EXPR_BINARY: {
                int left = evaluate_expression(env, node->data.expression.value.binary.left);
                int right = evaluate_expression(env, node->data.expression.value.binary.right);
                switch (node->data.expression.value.binary.op){
                    case '+': 
                        return left + right;
                    case '-':
                        return left - right;
                    case '*':
                        return left * right;
                    case '/':
                        return left / right;
                    default:
                        fprintf(stderr, "Unknow binary operator: %c\n", node->data.expression.value.binary.op);
                        exit(1);
                }
        }
        default:
            fprintf(stderr, "Unknow expression type\n");
            exit(1);
    }
}

void interpret_statement(Environment *env, ASTNode *node){
    printf("NODE TYPE: %d\n", node->type);
    switch (node->type){
        case NODE_PRINT:
            printf("Interpreting print statement\n");
            printf("%d\n", evaluate_expression(env, node->data.print_statement.expression));
            break;
        case NODE_INPUT: {
            printf("Interpreting input statement\n");
            int value;
            printf("Enter value for %s: ", node->data.input_statement.identifier);
            scanf("%d", &value);

            Variable *variable = find_variable(env, node->data.input_statement.identifier);
            if(variable){
                variable->value = value;
            }
            else {
                add_variables(env, create_variable(node->data.input_statement.identifier, value));
            }
            break;
        }
        case NODE_ASSIGNMENT: {
            printf("Interpreting assignment\n");
            int value = evaluate_expression(env, node->data.assignment.expression);
            Variable *variable = find_variable(env, node->data.assignment.identifier);
            if(variable){
                variable->value = value;
            }
            else {
                add_variables(env, create_variable(node->data.assignment.identifier, value));
            }
            break;
        }
        case NODE_IF: {
            if(evaluate_expression(env, node->data.if_statement.condition)){ // bug
                interpret_statement(env, node->data.if_statement.body);
            }
            break;
        }
        default:
            fprintf(stderr, "inknow statemet type\n");
    }
}

void interpret_program(ASTNode *program){
    printf("Interpreting dupa stefana\n");
    Environment *env = create_environment();
    ASTNode *current = program;

    while(current){
        printf("Current node type: %d\n", current->type);
        interpret_statement(env, current);
        current = current->next;
    }


    free_environment(env); 
    printf("Finished interpreting program\n");
}
