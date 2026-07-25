#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKENS 256

typedef enum
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_INPUT,
    TOKEN_OUTPUT,
    TOKEN_APPEND,
    TOKEN_END
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
} Token;


void init_tokens(Token tokens[]);

int tokenize(char *input, Token tokens[]);

void print_tokens(const Token tokens[]);

void free_tokens(Token tokens[]) ;

#endif